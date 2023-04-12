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

    current_string_format = format->string_format;

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

void Paragraph::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    if (with_elements)
        Element::Remake(with_elements, with_parent, with_undo);

    ElementPtr clone;
    bool remake = false;

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
                if (with_undo && !clone)
                    clone.reset(Clone());
                
                ElementPtr el = row->elements->Get(row->elements->Count() - 1);
                if (!el)
                    break;
                if (el->Split(page_width - format->indent_before))
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
                row->Remake(true, false, with_undo);
                next_row->Remake(true, false, with_undo);
                remake = true;
            }

            if (!b)
                continue;
            
            //move elements above if they are narrower to be placed in the row
            while (next_row && next_row->elements->Count() > 0 && next_row->elements->Get(0)->rect.width < page_width - row->rect.width - format->indent_before)
            {
                if (with_undo && !clone)
                    clone.reset(Clone());

                //move the element from the next row in the current one
                row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                row->Remake(true, false, with_undo);
                row->Normalize(false);
                if (next_row->elements->Count() == 0)
                {
                    elements->RemoveAt(i + 1, 1);
                    --i;
                    next_row.reset();
                }
                else
                {
                    next_row->Remake(true, false, with_undo);
                    next_row->Normalize(false);
                }
                row->UpdateRect();
                remake = true;
            }

            if (next_row)
            {
                //try to split the first element and move it above
                ElementPtr el = next_row->elements->Get(0);
                while (el && el->Split(page_width - row->rect.width - format->indent_before))
                {
                    if (with_undo && !clone)
                        clone.reset(Clone());
                    
                    row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                    row->Remake(true, false, with_undo);
                    next_row->Remake(true, false, with_undo);
                    row->Normalize(false);
                    next_row->Normalize(false);
                    el = next_row->elements->Get(0);
                    remake = true;
                }
            }

            if (remake)
            {
                row->Remake(true, false, with_undo);
                if (next_row)
                    next_row->Remake(true, false, with_undo);

                row->Normalize(false);
                if (next_row)
                    next_row->Normalize(false);
            }

            UpdateRect();
        }
    }

    int h = 0;
    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr row = elements->Get(i);
        row->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        row->rect.Move(format->indent_before, h + top_m); //move the row
        h += row->rect.height + format->line_spacing + top_m + bottom_m;
    }

    if (with_undo && clone)
    {
        document->CallFunc(ElementId{}, 
            [d = document](const ElementId id)
            {
                d->can_normalize = true;
            },
            true);
        for (int i = 0; i < clone->elements->Count(); ++i)
            document->InsertElement(clone->elements->Get(i));
        document->ClearElements(id, false, true);
        document->PushEditorState(CaretState(id, 0), true);
        document->CallFunc(ElementId{}, 
            [d = document](const ElementId id)
            {
                d->can_normalize = false;
            },
            true);
    }

    UpdateRect();

    if (elements->Count() > 0)
        baseline = elements->Get(0)->baseline;

    if (rect != last_rect && document->IsVisible(id))
    {
        if (with_parent)
        {
            parent->Remake(false, true, with_undo);
            document->Redraw(parent->id, false);
        }
        else
            document->Redraw(id, false);
    }
    last_rect = rect;
}

void Paragraph::Normalize(bool with_undo)
{
    if (!document->can_normalize)
        return;
    
    Element::Normalize(with_undo);

    if (elements->Count() == 0)
    {
        AddEmptyElement(); //paragraph has to have at least one row
        if (with_undo)
        {
            document->DeleteElements(false, false, true);
            document->PushEditorState(SelectionState(id, 0, elements->Count()), true);
        }
    }

    for (int i = 0; i < elements->Count();)
    {
        auto el = elements->Get(i);
        if (elements->Count() == 1)
            break;
        if (el->elements->Count() == 1 && document->IsString(el->elements->Get(0)) && el->elements->Get(0)->elements->Count() == 0)
        {
            elements->RemoveAt(i, 1);
            window->OnCaretMoved(document->GetEditorState());
        }
        else
            ++i;
    }
}

bool Paragraph::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    if (_elements.size() == 1 && document->IsRow(_elements[0]))
    {
        if (!Element::InsertElements(_elements, with_undo))
            return false;
        Normalize(with_undo);
        return true;
    }

    return parent->InsertElements(_elements, with_undo);
}

bool Paragraph::DeleteElements(bool left, bool with_undo)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (Element::DeleteElements(left, with_undo))
        {
            Normalize(with_undo);
            return true;
        }
    }
    return parent->DeleteElements(left, with_undo);
}

bool Paragraph::ChangeParagraphFormat(const ParagraphFormatPtr _format, bool with_undo)
{
    if (format->name == _format->name)
        return false;
    
    if (with_undo)
        document->ChangeParagraphFormat(format, false, true);

    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->UpdateStringFormat(format->string_format, _format->string_format);

    format = _format;
    Remake(true, true, with_undo);
    
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

void Paragraph::AddEmptyElement()
{
    AddElement(ElementPtr(new Row(this)));
}

StringFormatPtr Paragraph::GetStringFormat() const
{
    return current_string_format;
}

std::string Paragraph::ToHtml()
{
    return "<p>" + Element::ToHtml() + "</p>";
}

}
