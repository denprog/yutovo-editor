#ifndef __MULTIPLY_H__
#define __MULTIPLY_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Multiply : public OnlyShapeFormula
{
public:
    Multiply(Element* _parent);
    Multiply(Document* _document);
    Multiply(const Multiply& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

    virtual std::string ToHtml();
};

}

#endif
