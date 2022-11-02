#include "page.h"
#include "paragraph.h"
#include "document.h"

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
    if (_elements.size() == 1 && _elements[0]->type == ElementType::PARAGRAPH)
    {
        elements->Insert(_elements[0], before_state.GetPos());
        after_state.SetState(_elements[0]);
        return true;
    }

    return parent->InsertElements(_elements, before_state, after_state, with_undo);
}

bool Page::DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo)
{
    return false;
}

}
