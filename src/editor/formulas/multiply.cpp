#include "multiply.h"
#include "../document.h"

namespace yutovo
{

//Multiply

Multiply::Multiply(Element* _parent) :
    OnlyShapeFormula(_parent, '*')
{
    type = ElementType::MULTIPLY;
}

Multiply::Multiply(Document* _document) :
    OnlyShapeFormula(_document, '*')
{
    type = ElementType::MULTIPLY;
}

Multiply::Multiply(const Multiply& source) :
    OnlyShapeFormula(source)
{
}

Element* Multiply::Clone()
{
    return new Multiply(*this);
}

Element* Multiply::Create(Element* _parent)
{
    return new Multiply(_parent);
}

void Multiply::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color;
            float w = float(rect.width);
            window->DrawFillEllipse(round(r.left + w / 2 - w * 0.15), round(r.top + float(rect.height) / 2 - w * 0.15), 
                round(r.width * 0.1), round(w * 0.1), c);
        };

    OnlyShapeFormula::Draw();
}

std::string Multiply::ToHtml()
{
    return "<mo>×</mo>";
}

}
