#ifndef __ROW_H__
#define __ROW_H__

#include "element.h"

namespace yutovo
{

class Row : public Element
{
public:
    Row(Element* parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Remake(CaretState& caret_state, bool with_elements);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo);
    virtual bool DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo);

    virtual bool GetBeginCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetEndCaretState(const CaretState& before_state, CaretState& after_state, bool selection);

    virtual bool CanContinueSelection();
    
    //virtual bool GetTopCaretState(const int x, const int y, CaretState& res);
    //virtual bool GetBottomCaretState(const int x, const int y, CaretState& res);
};

typedef std::shared_ptr<Row> RowPtr;

}

#endif
