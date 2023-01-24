#include "plus.h"
#include "../document.h"

namespace yutovo
{

//Plus

Plus::Plus(Element* _parent) :
    OnlyShapeFormula(_parent, '+')
{
    type = ElementType::PLUS;
}

Plus::Plus(Document* _document) :
    OnlyShapeFormula(_document, '+')
{
    type = ElementType::PLUS;
}

Plus::Plus(const Plus& source) :
    OnlyShapeFormula(source)
{
}

Element* Plus::Clone()
{
    return new Plus(*this);
}

Element* Plus::Create(Element* _parent)
{
    return new Plus(_parent);
}

void Plus::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color;
            float w = float(rect.width);
            window->DrawFillRect(round(r.left + w * 0.1), round(r.top + float(rect.height) / 2 - w / 2 + w * 0.4), 
                round(r.width * 0.8), round(w * 0.15), c);
            window->DrawFillRect(round(r.left + w * 0.4), round(r.top + float(rect.height) / 2 - w / 2 + w * 0.1), 
                round(r.width * 0.15), round(w * 0.8), c);
        };

    OnlyShapeFormula::Draw();
}

std::string Plus::ToHtml()
{
    return "<mo>+</mo>";
}

}
