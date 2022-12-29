#ifndef __DIVISION_H__
#define __DIVISION_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class Division : public Formula
{
public:
    Division(Element* _parent);
    Division(Document* _document);
    Division(const Division& source);

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent);

    virtual bool DeleteElements(bool left, bool with_undo);
    
    virtual bool AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual std::string ToHtml();
    
protected:
    Element *upper, *lower;
    Shape *shape;
};

}

#endif
