#ifndef __POWER_H__
#define __POWER_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class Power : public MiddleShapeFormula
{
public:
    Power(Element* _parent);
    Power(Document* _document);
    Power(const Power& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual void UpdateLevel(uint8_t _level);

    virtual std::string ToHtml();
    virtual std::string ToText();
};

}

#endif
