#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "../element.h"
#include <functional>

namespace yutovo
{

class Shape : public Element
{
public:
    Shape(Element* _parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent);

    virtual bool HasCaretState();

public:
    std::function<void(const Rect& rect)> draw_func;
};

typedef std::shared_ptr<Shape> ShapePtr;

}

#endif
