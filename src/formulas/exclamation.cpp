#include "exclamation.h"
#include "../document.h"

namespace yutovo
{

//Exclamation

Exclamation::Exclamation(Element* _parent) :
    OnlyShapeFormula(_parent, '!')
{
    type = ElementType::EXCLAMATION;
}

Exclamation::Exclamation(Document* _document) :
    OnlyShapeFormula(_document, '!')
{
    type = ElementType::EXCLAMATION;
}

Exclamation::Exclamation(const Exclamation& source) :
    OnlyShapeFormula(source)
{
}

Element* Exclamation::Clone()
{
    return new Exclamation(*this);
}

Element* Exclamation::Create(Element* _parent)
{
    return new Exclamation(_parent);
}

std::string Exclamation::ToHtml()
{
    return "<mo>!</mo>";
}

}
