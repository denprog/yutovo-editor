#include "minus.h"
#include "../document.h"

namespace yutovo
{

//Minus

Minus::Minus(Element* _parent) :
    OnlyShapeFormula(_parent, '-')
{
    type = ElementType::MINUS;
}

Minus::Minus(Document* _document) :
    OnlyShapeFormula(_document, '-')
{
    type = ElementType::MINUS;
}

Minus::Minus(const Minus& source) :
    OnlyShapeFormula(source)
{
}

Element* Minus::Clone()
{
    return new Minus(*this);
}

Element* Minus::Create(Element* _parent)
{
    return new Minus(_parent);
}

void Minus::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color;
            float w = float(rect.width);
            window->DrawFillRect(round(r.left + w * 0.15), round(r.top + float(rect.height) / 2 - w / 2 + w * 0.15), 
                round(r.width * 0.9), round(w * 0.3), c);
        };

    OnlyShapeFormula::Draw();
}

std::string Minus::ToHtml()
{
    return "<mo>-</mo>";
}

}
