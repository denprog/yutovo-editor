#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "../element.h"
#include <functional>

namespace yutovo
{

#define ROOT_X_LEFT_OFFSET 2
#define ROOT_Y_OFFSET 4
#define ROOT_X_RIGHT_OFFSET 2

class Shape : public Element
{
public:
    Shape(Element* _parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool HasCaretState();

public:
    std::function<void(const Rect& rect)> draw_func;
};

typedef std::shared_ptr<Shape> ShapePtr;

}

#endif
