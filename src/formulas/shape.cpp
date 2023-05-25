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
    if (draw_func)
        draw_func(GetAbsoluteRect());
}

bool Shape::Remake(bool with_elements)
{
    return false;
}

bool Shape::HasCaretState()
{
    return true;
}

bool Shape::GetElementAtCoords(const int x, const int y, ElementId& _id)
{
    Rect r = parent->GetAbsoluteRect(GetCaretRect());
    if (r.IsPointInside(x, y))
    {
        _id = id;
        return true;
    }
   return false;
}

}
