#include "shape.h"

namespace yutovo
{

//Shape

Shape::Shape(Element* _parent) : 
    Element(_parent)
{
    type = ElementType::SHAPE;
}

Element* Shape::Clone()
{
    return new Shape(*this);
}

Element* Shape::Create(Element* parent)
{
    return new Shape(parent);
}

void Shape::Draw() const
{
    draw_func(GetAbsoluteRect());
}

void Shape::Remake(bool with_elements)
{
}

bool Shape::HasCaretState()
{
    return true;
}

}
