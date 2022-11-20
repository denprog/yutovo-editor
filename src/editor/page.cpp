#include "page.h"
#include "paragraph.h"
#include "document.h"
#include "row.h"
#include "str.h"

namespace yutovo
{

//Page

Page::Page(Element* parent) :
    Element(parent),
    format(document->GetDefaultPageFormat())
{
    type = ElementType::PAGE;

    AddElement(ElementPtr(new Paragraph(this))); //page has to have at least one paragraph
}

Element* Page::Clone()
{
    return new Page(*this);
}

Element* Page::Create(Element* parent)
{
    return new Page(parent);
}

void Page::Draw(const Selections& selections) const
{
    Rect v = window->GetRect();
    v.left += format->left_indent;
    v.top += format->top_indent;
    v.width -= format->right_indent + format->left_indent;
    v.height -= format->bottom_indent + format->right_indent;
    window->SetViewPort(v);

    window->BeginDrawOutside();
    window->DrawRect(Rect(v.left - 1, v.top - 1, v.width + 2, v.height + 2), Color::Blue());
    window->EndDrawOutside();

    Element::Draw(selections);
}

void Page::Remake(CaretState& caret_state, bool with_elements)
{
    Rect v = window->GetRect();
    page_width = v.width - format->right_indent - format->left_indent;

    if (with_elements)
        Element::Remake(caret_state, true);

    int h = 0;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->rect.Move(p->rect.left, h);
        h += p->rect.height + format->paragraph_spacing;
    }

    UpdateRect();

    document->Remake(parent->id, false);
}

void Page::UpdateRect()
{
    Element::UpdateRect();
    rect.left = format->left_indent;
    rect.top = format->top_indent;

    Rect v = window->GetRect();
    v.width -= format->right_indent + format->left_indent;
    v.height -= format->bottom_indent + format->right_indent;
    if (rect.width < v.width)
        rect.width = v.width;
    if (rect.height < v.height)
        rect.height = v.height;
}

bool Page::InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    if (_elements.size() != 1 || _elements[0]->type != ElementType::PARAGRAPH)
        return parent->InsertElements(_elements, before_state, after_state, with_undo);

    ElementPtr insert_element(_elements[0]->Clone());
    ElementPtr el = document->GetParent(before_state.id);
    ElementPtr paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);
    ElementPtr row = document->FindParent(el->id, ElementType::ROW);
    ElementPtr new_row = insert_element->elements->Get(0); //move elements into this one row, which will be splitted during paragraph formatting
    if (new_row->elements->Count() == 0)
        new_row->AddElement(ElementPtr(new String(new_row.get())));

    el->SplitAt(before_state.GetPos()); //try to split current element
    uint p = row->elements->GetElementPos(el->id);
    if (before_state.GetPos() != 0 || p != 0)
    {
        for (int i = p + 1; i < row->elements->Count();) //move all elements at the right side of the row
            new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
    }

    uint k = elements->GetElementPos(paragraph->id);
    for (int i = k + 1; i < paragraph->elements->Count(); ++i) //move the rest rows of the paragraph
    {
        ElementPtr r = paragraph->elements->Get(i);
        for (int j = 0; j < r->elements->Count();)
            new_row->elements->Move(r->elements->Get(j), new_row->elements->Count());
    }

    if (p == 0 && before_state.GetPos() == 0)
        elements->Insert(insert_element, k);
    else
        elements->Insert(insert_element, k + 1);

    if (after_state.IsEmpty())
    {
        if (p == 0 && before_state.GetPos() == 0)
            row->GetFirstCaretState(after_state, false);
        else
            new_row->GetFirstCaretState(after_state, false);
    }
    
    document->Remake(id, true);

    if (with_undo)
        document->DeleteElements(after_state, (CaretState&)before_state, true, false, true);
    
#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Page::DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo)
{
    ElementPtr el = document->GetParent(before_state.id);
    ElementPtr paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);

    int p = elements->GetElementPos(paragraph->id);
    if (left && p <= 0)
        return false;
    if (!left && p == elements->Count() - 1)
        return false;
    
    CaretState c;
    if (after_state.IsEmpty())
        c = before_state;
    
    //merge current paragraph with the above one
    auto dest_p = left ? elements->Get(p - 1) : elements->Get(p);
    auto dest_row = dest_p->elements->Get(dest_p->elements->Count() - 1);
    auto source_p = left ? elements->Get(p) : elements->Get(p + 1);
    for (uint i = 0; i < source_p->elements->Count(); ++i)
    {
        auto row = source_p->elements->Get(i);
        for (uint j = 0; j < row->elements->Count();)
        {
            if (after_state.IsEmpty())
                dest_row->elements->Move(row->elements->Get(j), dest_row->elements->Count(), c);
            else
                dest_row->elements->Move(row->elements->Get(j), dest_row->elements->Count());
        }
    }

    if (after_state.IsEmpty())
        after_state = c;

    elements->RemoveAt(left ? p : p + 1, 1, after_state);
    document->Remake(id, true);

    if (with_undo)
    {
        CaretState s = before_state;
        document->InsertParagraph(after_state, s);
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Page::GetTopCaretState(const int x, const int y, CaretState& res, bool selection)
{
    ElementPtr p;
    //find nearest paragraph
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (y < el->GetAbsoluteRect().GetBottom())
            break;
        p = el;
    }
    if (!p)
        return parent->GetTopCaretState(x, y, res, selection);
    return p->GetTopCaretState(x, y, res, selection);
}

bool Page::GetBottomCaretState(const int x, const int y, CaretState& res, bool selection)
{
    return false;
}

ParagraphFormatPtr Page::GetParagraphFormat()
{
    return document->current_paragraph_format;
}

}
