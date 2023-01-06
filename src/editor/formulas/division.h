#ifndef __DIVISION_H__
#define __DIVISION_H__

#include "formula.h"
#include "middle_shape_formula.h"

namespace yutovo
{

class Division : public MiddleShapeFormula
{
public:
    Division(Element* _parent);
    Division(Document* _document);
    Division(const Division& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual std::string ToHtml();
    virtual std::string ToText();
};

}

#endif
