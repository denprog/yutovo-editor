#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "middle_shape_formula.h"
#include "result.h"

namespace yutovo
{

class Equation : public MiddleShapeFormula
{
public:
    Equation(Element* _parent);
    Equation(Document* _document, ResultType _result_type);
    Equation(const Equation& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml();
    virtual std::string ToText();

protected:
    AutoResultPtr auto_result;
    ResultType result_type;
};

}

#endif
