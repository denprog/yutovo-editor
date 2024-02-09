#include "paragraph.h"
#include "document.h"
#include "row.h"

namespace yutovo
{

//Paragraph

Paragraph::Paragraph(Element* parent, bool with_row) :
    Element(parent),
    format(parent->GetParagraphFormat())
{
    type = ElementType::PARAGRAPH;

    current_string_format = format->default_string_format;

    if (with_row)
        AddEmptyElement(); //paragraph has to have at least one row
}

Paragraph::Paragraph(Document* _document, bool with_row) :
    Element(_document)
{
    type = ElementType::PARAGRAPH;

    document->GetCurrentParagraphFormat(format);
    document->GetCurrentStringFormat(current_string_format);
    if (with_row)
        AddEmptyElement(); //paragraph has to have at least one row
}

Element* Paragraph::Clone()
{
    return new Paragraph(*this);
}

Element* Paragraph::Create(Element* parent)
{
    return new Paragraph(parent);
}

void Paragraph::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);
    rapidjson::Value _format_name(format->name.c_str(), alloc);
    value.AddMember("format_name", _format_name, alloc);
}

Element* Paragraph::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Paragraph* p = nullptr;
    if (parent)
        p = new Paragraph(parent, false);
    else
        p = new Paragraph(document, false);
    
    if (!value.HasMember("format_name") || !value["format_name"].IsString())
        return p;
    auto format_name = value["format_name"].GetString();
    auto f = document->paragraph_formats->GetFormat(format_name);
    if (f)
        p->format = f;
    return p;
}

void Paragraph::Draw() const
{
    auto _id = document->GetFirstVisibleRow(id);
    int pos = elements->GetChildPos(_id);
    for (int i = pos; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (document->IsVisible(el->id))
            el->Draw();
    }
}

bool Paragraph::Remake(bool with_elements)
{
    if (document->break_remake)
        return false;
    
    bool changed = Element::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;

    if (format->word_wrap == ParagraphFormat::WordWrap::Normal)
    {
        int page_width = ((Text*)parent)->page_width;

        for (int i = 0; i < elements->Count(); ++i)
        {
            ElementPtr row = elements->Get(i);
            ElementPtr next_row;
            if (i < elements->Count() - 1)
                next_row = elements->Get(i + 1);

            bool b = true;
            //move or split element if it's more then row width
            while (row->rect.width + format->indent_before > page_width)
            {
                if (document->break_remake)
                    return false;
                ElementPtr el = row->elements->Get(row->elements->Count() - 1);
                if (!el)
                    break;
                if (el->Split(page_width - format->indent_before, true))
                    el = row->elements->Get(row->elements->Count() - 1);

                if (row->elements->Count() == 1)
                {
                    b = false;
                    break;
                }

                if (!next_row)
                {
                    next_row.reset(new Row(this));
                    AddElement(next_row);
                    next_row->elements->Clear();
                }

                //move the element in the next row
                next_row->elements->Move(el, 0);
                row->Remake();
                next_row->Remake(true);
                changed = true;
            }

            if (!b)
                continue;

            //move elements above if they are narrower to be placed in the row
            while (next_row && next_row->elements->Count() > 0)
            {
                if (document->break_remake)
                    return false;
                auto el = next_row->elements->Get(0);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                if (el->rect.width + left_m + right_m >= page_width - row->rect.width - format->indent_before)
                    break;
                
                //move the element from the next row in the current one
                row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                row->Remake(true);
                row->Normalize();
                if (next_row->elements->Count() == 0)
                {
                    elements->RemoveAt(i + 1, 1);
                    --i;
                    next_row.reset();
                }
                else
                {
                    next_row->Remake(true);
                    next_row->Normalize();
                }
                row->UpdateRect();
                changed = true;
            }

            if (next_row)
            {
                //try to split the first element and move it above
                ElementPtr el = next_row->elements->Get(0);
                while (el && el->Split(page_width - row->rect.width - format->indent_before, false))
                {
                    row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                    row->Remake(true);
                    next_row->Remake(true);
                    row->Normalize();
                    next_row->Normalize();
                    el = next_row->elements->Get(0);
                    changed = true;
                }
            }

            if (changed)
            {
                row->Remake();
                if (next_row)
                    next_row->Remake();

                row->Normalize();
                if (next_row)
                    next_row->Normalize();
            }

            UpdateRect();
        }
    }

    int h = 0;
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr row = elements->Get(i);
        row->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        row->rect.Move(format->indent_before, h + top_m); //move the row
        h += row->rect.height + format->line_spacing + top_m + bottom_m;
    }

    UpdateRect();

    if (elements->Count() > 0)
        baseline = elements->Get(0)->baseline;

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Paragraph::Normalize()
{
    Element::Normalize();

    if (elements->Count() == 0)
        AddEmptyElement(); //paragraph has to have at least one row

    for (int i = 0; i < elements->Count();)
    {
        auto el = elements->Get(i);
        if (elements->Count() == 1)
            break;
        if (el->elements->Count() == 1 && document->IsString(el->elements->Get(0)) && el->elements->Get(0)->elements->Count() == 0)
        {
            elements->RemoveAt(i, 1);
            window->OnCaretMoved(document->MakeEditorState());
        }
        else
            ++i;
    }

    //try to merge end of a row above and begin of a row below
    for (int i = 0; i < elements->Count() - 1; ++i)
    {
        auto above = elements->Get(i);
        auto below = elements->Get(i + 1);
        auto el1 = above->elements->Get(above->elements->Count() - 1);
        auto el2 = below->elements->Get(0);
        if (document->IsString(el1) && document->IsString(el2))
        {
            auto str = el1->ToText();
            if (str.length() > 0 && str[str.size() - 1] != U' ')
            {
                if (el1->Merge(el2))
                {
                    above->Remake();
                    below->Remake();
                }
            }
        }
    }
}

bool Paragraph::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() == 1 && document->IsRow(_elements[0]))
    {
        if (!Element::InsertElements(_elements, with_undo, changed_element))
            return false;
        Normalize();
        changed_element = id;
        return true;
    }

    auto el = document->GetElement(caret->GetCaretState().id);
    if (el && document->IsRow(el->id))
        return el->InsertElements(_elements, with_undo, changed_element); //insert in the beginning of current row

    return parent->InsertElements(_elements, with_undo, changed_element);
}

bool Paragraph::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (Element::DeleteElements(left, with_undo, changed_element))
        {
            Normalize();
            changed_element = id;
            return true;
        }
    }

    CaretState c = caret->GetCaretState();
    auto row = document->FindParentRow(c.id);
    if (row)
    {
        CaretState first_state, last_state;
        if (!left && row->GetLastCaretState(last_state, nullptr))
        {
            if (c == last_state)
            {
                int p = elements->GetElementPos(row->id);
                if (p < elements->Count() - 1)
                {
                    //delete first element at the next row
                    auto next_row = elements->Get(p + 1);
                    if (next_row->GetFirstCaretState(first_state, nullptr))
                    {
                        caret->SetState(first_state);
                        return caret->GetElement()->DeleteElements(false, with_undo, changed_element);
                    }
                }
            }
        }
        else if (left && row->GetFirstCaretState(first_state, nullptr))
        {
            if (c == first_state)
            {
                int p = elements->GetElementPos(row->id);
                if (p > 0)
                {
                    //delete last element at the previous row
                    auto prev_row = elements->Get(p - 1);
                    if (prev_row->GetLastCaretState(last_state, nullptr))
                    {
                        caret->SetState(last_state);
                        return caret->GetElement()->DeleteElements(true, with_undo, changed_element);
                    }
                }
            }
        }
    }

    return parent->DeleteElements(left, with_undo, changed_element);
}

bool Paragraph::ChangeParagraphFormat(const ParagraphFormatPtr _format, bool with_undo, ElementId& changed_element)
{
    if (format->name == _format->name)
        return false;
    
    if (with_undo)
        document->StoreUndo(id);
    
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->UpdateStringFormat(format->default_string_format, _format->default_string_format);

    format = _format;
    changed_element = id;
    
    return true;
}

bool Paragraph::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (y < el->GetAbsoluteRect().GetBottom() || el->rect.height == 0)
            break;
        row = el;
    }
    if (!row)
        return parent->GetTopCaretState(x, GetAbsoluteRect().top, caret_state, select);
    return row->GetTopCaretState(x, y, caret_state, select);
}

bool Paragraph::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (y > el->GetAbsoluteRect().top || el->rect.height == 0)
            break;
        row = el;
    }
    if (!row)
        return parent->GetBottomCaretState(x, GetAbsoluteRect().GetBottom(), caret_state, select);
    return row->GetBottomCaretState(x, y, caret_state, select);
}

bool Paragraph::CanContinueSelection()
{
    return true;
}

bool Paragraph::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    int min_dist = std::numeric_limits<int>::max();
    ElementPtr el;
    for (int i = 0; i < elements->Count(); ++i) //TODO: binary search
    {
        auto _el = elements->Get(i);
        Rect r = _el->GetAbsoluteRect();
    	int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            el = _el;
        }
    }

    if (!el)
        return false;
    return el->GetNearestCaretState(x, y, caret_state);
}

void Paragraph::AddEmptyElement()
{
    AddElement(ElementPtr(new Row(this)));
}

bool Paragraph::IsEmpty()
{
    if (elements->Count() != 1)
        return false;
    return elements->Get(0)->IsEmpty();
}

ParagraphFormatPtr Paragraph::GetParagraphFormat()
{
    return format;
}

StringFormatPtr Paragraph::GetStringFormat() const
{
    return current_string_format;
}

std::string Paragraph::ToHtml()
{
    return "<p>" + Element::ToHtml() + "</p>";
}

ElementPtr Paragraph::GetPlainRow()
{
    ElementPtr row(new Row(parent, false));
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto r = elements->Get(i);
        for (int j = 0; j < r->elements->Count(); ++j)
        {
            auto _el = r->elements->Get(j);
            row->elements->Add(_el);
        }
    }
    return row;
}

void Paragraph::MakePlain()
{
    //remake to only one row
    if (elements->Count() <= 1)
        return;
    auto r = GetPlainRow();
    elements->Clear();
    elements->Add(r);
}

}
