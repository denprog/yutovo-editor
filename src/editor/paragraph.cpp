#include "paragraph.h"
#include "document.h"
#include "page.h"
#include "row.h"

namespace yutovo
{

//Paragraph

Paragraph::Paragraph(Element* parent) :
    Element(parent),
    format(parent->GetParagraphFormat())
{
    type = ElementType::PARAGRAPH;

    AddElement(ElementPtr(new Row(this))); //paragraph has to have at least one row
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
    Element::Draw();
}

void Paragraph::Remake(bool with_elements)
{
    if (with_elements)
        Element::Remake(with_elements);

    bool remake = false;

    if (format->word_wrap == ParagraphFormat::WordWrap::Normal)
    {
        Page* page = (Page*)parent;

        for (int i = 0; i < elements->Count(); ++i)
        {
            ElementPtr row = elements->Get(i);
            ElementPtr next_row;
            if (i < elements->Count() - 1)
                next_row = elements->Get(i + 1);

            bool b = true;
            //move or split element if it's more then row width
            while (row->rect.width + format->indent_before > page->page_width)
            {
                ElementPtr el = row->elements->Get(row->elements->Count() - 1);
                if (el->Split(page->page_width - format->indent_before))
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
                }

                //move the element in the next row
                next_row->elements->Move(el, 0);
                row->Remake(false);
                next_row->Remake(true);
                remake = true;
            }

            if (!b)
                continue;

            //move elements above if they are narrower to be placed in the row
            while (next_row && next_row->elements->Get(0)->rect.width < page->page_width - row->rect.width - format->indent_before)
            {
                //move the element from the next row in the current one
                row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                row->Remake(true);
                if (next_row->elements->Count() == 0)
                {
                    elements->RemoveAt(i + 1, 1);
                    next_row.reset();
                }
                else
                    next_row->Remake(true);
                remake = true;
            }

            if (next_row)
            {
                //try to split the first element and move it above
                ElementPtr el = next_row->elements->Get(0);
                while (el->Split(page->page_width - row->rect.width - format->indent_before))
                {
                    row->elements->Move(next_row->elements->Get(0), row->elements->Count());
                    row->Remake(true);
                    next_row->Remake(true);
                    el = next_row->elements->Get(0);
                    remake = true;
                }
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
        row->rect.Move(row->rect.left, h + top_m); //move the row
        h += row->rect.height + format->line_spacing + top_m + bottom_m;
    }

    UpdateRect();

    if (remake)
        document->Remake(id, true);
    document->Remake(parent->id, false);
}

void Paragraph::UpdateRect()
{
    Element::UpdateRect();

    rect.left = format->indent_before;
    rect.top = 0;
}

void Paragraph::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int max_left_m = 0, max_right_m = 0;
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr row = elements->Get(i);
        row->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        if (max_left_m < left_m)
            max_left_m = left_m;
        if (max_right_m < right_m)
            max_right_m = right_m;
        if (i == 0)
            top = top_m;
        if (i == elements->Count() - 1)
            bottom = bottom_m;
    }
}

bool Paragraph::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    if (_elements.size() == 1 && _elements[0]->type == ElementType::ROW)
    {
        return Element::InsertElements(_elements, with_undo);
    }

    return parent->InsertElements(_elements, with_undo);
}

bool Paragraph::DeleteElements(bool left, bool with_undo)
{
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
    Remake(true);
    
    return true;
}

bool Paragraph::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (y < el->GetAbsoluteRect().GetBottom())
            break;
        row = el;
    }
    if (!row)
        return parent->GetTopCaretState(x, y, caret_state, select);
    return row->GetTopCaretState(x, y, caret_state, select);
}

bool Paragraph::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr row;
    //find nearest row
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (y > el->GetAbsoluteRect().top)
            break;
        row = el;
    }
    if (!row)
        return parent->GetBottomCaretState(x, y, caret_state, select);
    return row->GetBottomCaretState(x, y, caret_state, select);
}

bool Paragraph::CanContinueSelection()
{
    return true;
}

StringFormatPtr Paragraph::GetStringFormat()
{
    return format->string_format;
}

std::string Paragraph::ToHtml()
{
    return "<p>" + Element::ToHtml() + "</p>";
}

}
