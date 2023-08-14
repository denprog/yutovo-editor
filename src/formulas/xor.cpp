#include "xor.h"
#include "../document.h"

namespace yutovo
{

//Xor

Xor::Xor(Element* _parent) :
    OnlyShapeFormula(_parent, '^')
{
    type = ElementType::XOR;
}

Xor::Xor(Document* _document) :
    OnlyShapeFormula(_document, '^')
{
    type = ElementType::XOR;
}

Xor::Xor(const Xor& source) :
    OnlyShapeFormula(source)
{
}

Element* Xor::Clone()
{
    return new Xor(*this);
}

Element* Xor::Create(Element* _parent)
{
    return new Xor(_parent);
}

std::string Xor::ToHtml()
{
    return "<mo>^</mo>";
}

}
