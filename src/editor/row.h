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

    virtual void Remake(bool with_elements);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);

    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);

    virtual bool CanContinueSelection();
};

typedef std::shared_ptr<Row> RowPtr;

}

#endif
