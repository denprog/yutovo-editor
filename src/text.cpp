#include "text.h"
#include "document.h"
#include "paragraph.h"
#include <assert.h>

namespace yutovo
{

//Text

Text::Text(Document* _document) : 
    Block(_document),
    text_format(document->GetDefaultTextFormat()),
    page_format(document->GetDefaultPageFormat())
{
    type = ElementType::TEXT;

    id.push_back(0);
    logical_id = id;

    AddElement(ElementPtr(new Paragraph(this))); //text has to have at least one paragraph
}

Element* Text::Clone()
{
    return new Text(*this);
}

Element* Text::Create(Element* parent)
{
    return nullptr;
}

void Text::Draw() const
{
    Rect v = window->GetRect();
    window->BeginDrawOutside();
    window->DrawFillRect(v, Color::White());
    window->EndDrawOutside();

    v.left += page_format->left_indent;
    v.top += page_format->top_indent;
    v.width -= page_format->right_indent + page_format->left_indent;
    v.height -= page_format->bottom_indent + page_format->top_indent;
    window->SetViewPort(v);

    Block::Draw();

    window->BeginDrawOutside();
    window->DrawRect(Rect(v.left - 1, v.top - 1, v.width + 2, v.height + 2), Color::Blue());
    window->EndDrawOutside();
}

bool Text::Remake(bool with_elements)
{
    Rect v = window->GetRect();
    page_width = v.width - page_format->right_indent - page_format->left_indent;

    Block::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = page_format->top_indent;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(page_format->left_indent, h + top_m);
        h += p->rect.height + page_format->paragraph_spacing + bottom_m;
    }

    UpdateRect();

    return false;
}

void Text::UpdateRect(bool with_elements)
{
    Block::UpdateRect(with_elements);

    Rect v = window->GetRect();
    page_width = v.width - page_format->right_indent - page_format->left_indent;

    v.width -= page_format->right_indent + page_format->left_indent;
    v.height -= page_format->bottom_indent + page_format->right_indent;
    if (rect.width < v.width)
        rect.width = v.width;
    if (rect.height < v.height)
        rect.height = v.height;
    
    Point p = window->GetDocumentPoint();
    if (p.x + v.width > rect.width)
        rect.width = p.x + v.width;
}

void Text::UpdateDrawRect()
{
    Block::UpdateDrawRect();
    draw_rect.width += page_format->right_indent + page_format->left_indent;
    draw_rect.height += page_format->bottom_indent + page_format->right_indent;
}

Rect Text::GetCaretRect(const uint pos) const
{
    return Rect{0, 0, 0, 0};
}

ParagraphFormatPtr Text::GetParagraphFormat()
{
    ParagraphFormatPtr format;
    if (document->GetCurrentParagraphFormat(format))
        return format;
    return nullptr;
}

std::string Text::ToHtml()
{
    return "<body>" + Block::ToHtml() + "</body>";
}

Rect Text::GetAbsoluteRect() const
{
    Rect w = window->GetRect();
    Rect r = Block::GetAbsoluteRect();
    if (r.width < w.width)
        r.width = w.width;
    if (r.height < w.height)
        r.height = w.height;

    Point p = window->GetDocumentPoint();
    if (p.x + w.width > r.width)
        r.width = p.x + w.width;
    r.height += page_format->bottom_indent + page_format->right_indent;
    return r;
}

}
