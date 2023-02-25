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

std::string Minus::ToHtml()
{
    return "<mo>-</mo>";
}

}
