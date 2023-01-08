#ifndef __NTH_ROOT_H__
#define __NTH_ROOT_H__

#include "middle_shape_formula.h"

namespace yutovo
{

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
