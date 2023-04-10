#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "element.h"

namespace yutovo
{

//Group of paragraphs
class Block : public Element
{
public:
    Block(Element* _parent);
    Block(Document* _document);

    virtual void Normalize(bool with_undo);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual void AddEmptyElement();
};

}

#endif
