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

    virtual void Normalize();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual void AddEmptyElement();

    virtual std::u32string ToText() const;
};

}

#endif
