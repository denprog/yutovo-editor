#include "multiply.h"
#include "../document.h"

namespace yutovo
{

//Multiply

Multiply::Multiply(Element* _parent) :
    OnlyShapeFormula(_parent, L'·')
{
    type = ElementType::MULTIPLY;
}

Multiply::Multiply(Document* _document) :
    OnlyShapeFormula(_document, L'·')
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

std::u32string Multiply::ToText()
{
    return U"*";
}

std::string Multiply::ToHtml()
{
    return "<mo>×</mo>";
}

}
