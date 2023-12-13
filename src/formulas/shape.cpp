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

Element* Shape::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
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

void Shape::Select(const CaretState& start, const CaretState& end)
{
    parent->Select(start, end);
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
