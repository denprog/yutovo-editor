#include "page.h"
#include "paragraph.h"
#include "document.h"
#include "row.h"
#include "str.h"

namespace yutovo
{

//Page

Page::Page(Element* parent) :
    Block(parent),
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

void Page::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    Rect v = window->GetRect();
    page_width = v.width - format->right_indent - format->left_indent;

    if (with_elements)
        Element::Remake(true, with_parent, with_undo);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = 0;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(p->rect.left, h + top_m);
        h += p->rect.height + format->paragraph_spacing + bottom_m;
    }

    Element::UpdateRect(false);
    rect.left = format->left_indent;
    rect.top = format->top_indent;

    bool remake = (rect != last_rect && with_parent);
    last_rect = rect;

    UpdateRect();

    if (remake)
    {
        parent->Remake(false, with_parent, with_undo);
        document->Redraw(id, false);
    }
}

void Page::UpdateRect(bool with_elements)
{
    Element::UpdateRect(with_elements);
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

ParagraphFormatPtr Page::GetParagraphFormat()
{
    ParagraphFormatPtr format;
    if (document->GetCurrentParagraphFormat(format))
        return format;
    return nullptr;
}

}
