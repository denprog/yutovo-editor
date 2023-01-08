#ifndef __SQUARE_ROOT_H__
#define __SQUARE_ROOT_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class SquareRoot : public Formula
{
public:
    SquareRoot(Element* _parent);
    SquareRoot(Document* _document);
    SquareRoot(const SquareRoot& source);

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool DeleteElements(bool left, bool with_undo);

    virtual std::string ToHtml();
    virtual std::string ToText();

protected:
    Element *last;
    Shape *shape;
};

}

#endif
