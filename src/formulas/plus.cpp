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

std::string Plus::ToHtml()
{
    return "<mo>+</mo>";
}

}
