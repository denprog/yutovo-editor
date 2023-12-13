#include "text.h"
#include "document.h"
#include "paragraph.h"
#include <assert.h>

namespace yutovo
{

//Text

Text::Text(Document* _document, bool with_paragraph) : 
    Block(_document),
    text_format(document->GetDefaultTextFormat()),
    page_format(document->GetDefaultPageFormat())
{
    type = ElementType::TEXT;

    id.push_back(0);
    logical_id = id;

    if (with_paragraph)
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

Element* Text::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    return new Text(document, false);
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
    page_format = document->GetDefaultPageFormat();

    Rect v = window->GetRect();
    page_width = v.width - page_format->right_indent - page_format->left_indent;

    Block::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = page_format->top_indent;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        if (document->break_remake)
            return false;
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(page_format->left_indent, h + top_m);
        h += p->rect.height + page_format->paragraph_spacing + bottom_m;
    }

    UpdateRect();

    return false;
}

void Text::Select(const CaretState& start, const CaretState& end)
{
    if (start == end)
    {
        caret->SetState(start);
        return;
    }

    int p1 = yutovo::GetChildPos(id, start.id);
    int p2 = yutovo::GetChildPos(id, end.id);

    auto el = elements->Get(p1);
    el->Select(start, end);
    if (p2 - p1 > 1)
        selection->Add(id, p1 + 1, p2 - p1 - 1);
    else if (p1 - p2 > 1)
        selection->Add(id, p2 + 1, p1 - p2 - 1);
    el = elements->Get(p2);
    caret->SetState(end);
    el->Select(start, end);
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

bool Text::GetElementAtCoords(const int x, const int y, ElementId& _id)
{
    Rect r = GetAbsoluteRect();
    if (!r.IsPointInside(x, y))
        return false;
    
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (el->GetElementAtCoords(x, y, _id))
            return true;
    }
    _id = id;
    return true;
}

bool Text::GetNearestElement(const int x, const int y, ElementId& _id, int& dist)
{
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->GetNearestElement(x, y, _id, dist);
    }

    return true;
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
