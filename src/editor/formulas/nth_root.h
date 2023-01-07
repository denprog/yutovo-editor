#ifndef __NTH_ROOT_H__
#define __NTH_ROOT_H__

#include "middle_shape_formula.h"

namespace yutovo
{

#define ROOT_X_LEFT_OFFSET 2
#define ROOT_Y_OFFSET 4
#define ROOT_X_RIGHT_OFFSET 2

class NthRoot : public MiddleShapeFormula
{
public:
    NthRoot(Element* _parent);
    NthRoot(Document* _document);
    NthRoot(const NthRoot& source);

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
