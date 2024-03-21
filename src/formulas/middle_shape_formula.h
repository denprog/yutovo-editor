#ifndef __MIDDLE_SHAPE_FORMULA_H__
#define __MIDDLE_SHAPE_FORMULA_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class CodeRow;

class MiddleShapeFormula : public Formula
{
public:
    MiddleShapeFormula(Element* _parent, bool with_init = true);
    MiddleShapeFormula(Document* _document, bool with_init = true);
    MiddleShapeFormula(const MiddleShapeFormula& source);

    virtual bool AfterFromJson();

    void Init();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual bool AfterInsert(bool with_undo);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);

protected:
    CodeRow* GetFirst() const;
    Shape* GetShape() const;
    CodeRow* GetLast() const;
};

}

#endif
