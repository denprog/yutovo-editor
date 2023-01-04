#ifndef __MINUS_H__
#define __MINUS_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Minus : public OnlyShapeFormula
{
public:
    Minus(Element* _parent);
    Minus(Document* _document);
    Minus(const Minus& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

    virtual std::string ToHtml();
};

}

#endif
