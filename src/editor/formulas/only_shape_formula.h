#ifndef __ONLY_SHAPE_FORMULA_H__
#define __ONLY_SHAPE_FORMULA_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class OnlyShapeFormula : public Formula
{
public:
    OnlyShapeFormula(Element* _parent, char _symbol);
    OnlyShapeFormula(Document* _document, char _symbol);
    OnlyShapeFormula(const OnlyShapeFormula& source);

    virtual bool AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);

    virtual void UpdateRect(bool with_elements = false);

protected:
    Shape *shape;
    char symbol;
};

}

#endif
