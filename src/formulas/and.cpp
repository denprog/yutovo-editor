#include "and.h"
#include "../document.h"

namespace yutovo
{

//And

And::And(Element* _parent) :
    OnlyShapeFormula(_parent, '&')
{
    type = ElementType::AND;
}

And::And(Document* _document) :
    OnlyShapeFormula(_document, '&')
{
    type = ElementType::AND;
}

And::And(const And& source) :
    OnlyShapeFormula(source)
{
}

Element* And::Clone()
{
    return new And(*this);
}

Element* And::Create(Element* _parent)
{
    return new And(_parent);
}

std::string And::ToHtml()
{
    return "<mo>&</mo>";
}

}
