#include "text.h"
#include "document.h"
#include "page.h"
#include <assert.h>

namespace yutovo
{

//Text

Text::Text(Document* _document) : 
    Element(nullptr),
    format(document->GetDefaultTextFormat())
{
    type = ElementType::TEXT;

    document = _document;
    window = document->window;

    id.push_back(0);

    AddElement(ElementPtr(new Page(this))); //text has to have at least one page
}

Element* Text::Clone()
{
    return new Text(*this);
}

void Text::Draw(const Selections& selections) const
{
    window->BeginDrawOutside();
    window->DrawFillRect(window->GetRect(), Color::White());
    window->EndDrawOutside();

    Element::Draw(selections);
}

void Text::Remake(CaretState& caret_state, bool with_elements)
{
    Element::Remake(caret_state, with_elements);
}

void Text::UpdateRect()
{
    Element::UpdateRect();

    Rect v = window->GetRect();
    if (rect.width < v.width)
        rect.width = v.width;
    if (rect.height < v.height)
        rect.height = v.height;
    
    Point p = window->GetDocumentPoint();
    if (p.x + v.width > rect.width)
        rect.width = p.x + v.width;
}

bool Text::InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    // if (_elements.size() == 1 && _elements[0]->type == ElementType::PAGE)
    // {
    //     if (format->paging == TextFormat::Paging::ONE_PAGE)
    //     {
    //         if (elements->Count() == 0)
    //         {
    //             elements->Add(_elements[0]);
    //             after_state.SetState(_elements[0]);
    //             document->Remake(id);
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    return false;
}

bool Text::DeleteElements(const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    return false;
}

std::string Text::ToHtml()
{
    return "<body>" + Element::ToHtml() + "</body>";
}

Rect Text::GetAbsoluteRect() const
{
    Rect w = window->GetRect();
    //Rect r = elements->GetRect();
    Rect r = Element::GetAbsoluteRect();
    if (r.width < w.width)
        r.width = w.width;
    if (r.height < w.height)
        r.height = w.height;

    Point p = window->GetDocumentPoint();
    if (p.x + w.width > r.width)
        r.width = p.x + w.width;
    return r;
}

}
