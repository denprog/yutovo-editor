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

void Page::Draw() const
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

    Element::Draw();
}

void Page::Remake(bool with_elements)
{
    Rect v = window->GetRect();
    page_width = v.width - format->right_indent - format->left_indent;

    if (with_elements)
        Element::Remake(true);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = 0;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(p->rect.left, h + top_m);
        h += p->rect.height + format->paragraph_spacing + bottom_m;
    }

    UpdateRect();

    //document->Remake(parent->id, false);
    if (rect != last_rect)
        parent->Remake(false);
    document->Redraw(id, false);
    last_rect = rect;
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

bool Page::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    if (_elements.size() != 1 || _elements[0]->type != ElementType::PARAGRAPH)
        return parent->InsertElements(_elements, with_undo);

    CaretState before_state = caret->GetCaretState();
    ElementPtr insert_element(_elements[0]->Clone());
    ElementPtr el = document->GetParent(before_state.id);
    ElementPtr paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);
    ElementPtr row = document->FindParent(el->id, ElementType::ROW);

    int k = elements->GetElementPos(paragraph->id);
    int p = row->elements->GetElementPos(el->id);
    bool caret_next_row = false;
    if (p == 0 && document->caret->current_pos == 0)
    {
        elements->Insert(insert_element, k);
    }
    else
    {
        elements->Insert(insert_element, k + 1);
        caret_next_row = true;
    }

    ElementPtr new_row = insert_element->elements->Get(0); //move elements into this one row, which will be splitted during paragraph formatting
    if (new_row->elements->Count() == 0)
        new_row->AddElement(ElementPtr(new String(new_row.get())));

    if (p >= 0)
    {
        if (!el->SplitAt(before_state.GetPos()) && before_state.GetPos() == 0) //try to split current element
            --p;
        if (before_state.GetPos() != 0 || p >= 0)
        {
            for (int i = p + 1; i < row->elements->Count();) //move all elements at the right side of the row
                new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
        }
    }

    int r_pos = paragraph->elements->GetElementPos(row->id);
    for (int i = r_pos + 1; i < paragraph->elements->Count(); ++i) //move the rest rows of the paragraph
    {
        ElementPtr r = paragraph->elements->Get(i);
        for (int j = 0; j < r->elements->Count();)
            new_row->elements->Move(r->elements->Get(j), new_row->elements->Count());
    }

    CaretState after;
    if (caret_next_row)
    {
        if (new_row->GetFirstCaretState(after, nullptr))
            document->caret->SetState(after);
    }
    else
    {
        if (row->GetFirstCaretState(after, nullptr))
            document->caret->SetState(after);
    }
    
    document->Remake(id, true);

    if (with_undo)
        document->DeleteElements(true, false, true);
    
    new_row->Normalize(with_undo);
    
#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Page::DeleteElements(bool left, bool with_undo)
{
    CaretState before_state = caret->GetCaretState();
    ElementPtr el = document->GetParent(before_state.id);
    ElementPtr paragraph = document->FindParent(el->id, ElementType::PARAGRAPH);

    int p = elements->GetElementPos(paragraph->id);
    if (left && p <= 0)
        return false;
    if (!left && p == elements->Count() - 1)
        return false;
    
    //merge current paragraph with the above one
    auto dest_p = left ? elements->Get(p - 1) : elements->Get(p);
    auto dest_row = dest_p->elements->Get(dest_p->elements->Count() - 1);
    auto source_p = left ? elements->Get(p) : elements->Get(p + 1);
    for (uint i = 0; i < source_p->elements->Count(); ++i)
    {
        auto row = source_p->elements->Get(i);
        for (uint j = 0; j < row->elements->Count();)
            dest_row->elements->Move(row->elements->Get(j), dest_row->elements->Count());
    }

    elements->RemoveAt(left ? p : p + 1, 1);
    document->Remake(id, true);

    if (with_undo)
        document->InsertParagraph(true, true);
    
    dest_row->Normalize(with_undo);

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Page::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
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
        return parent->GetTopCaretState(x, y, caret_state, select);
    return p->GetTopCaretState(x, y, caret_state, select);
}

bool Page::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr p;
    //find nearest paragraph
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (y > el->GetAbsoluteRect().top)
            break;
        p = el;
    }
    if (!p)
        return parent->GetBottomCaretState(x, y, caret_state, select);
    return p->GetBottomCaretState(x, y, caret_state, select);
}

ParagraphFormatPtr Page::GetParagraphFormat()
{
    ParagraphFormatPtr format;
    if (document->GetCurrentParagraphFormat(format))
        return format;
    return nullptr;
}

}
