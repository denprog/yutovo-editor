#ifndef __MIDDLE_SHAPE_FORMULA_H__
#define __MIDDLE_SHAPE_FORMULA_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class MiddleShapeFormula : public Formula
{
public:
    MiddleShapeFormula(Element* _parent);
    MiddleShapeFormula(Document* _document);
    MiddleShapeFormula(const MiddleShapeFormula& source);

    void Init();

    virtual bool DeleteElements(bool left, bool with_undo);

    virtual bool AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);

protected:
    Element *first, *last;
    Shape *shape;
};

}

#endif
