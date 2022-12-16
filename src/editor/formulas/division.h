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

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements);

    virtual void AfterInsert();

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual std::string ToHtml();
    
protected:
    ElementPtr upper, lower;
    ShapePtr shape;
};

}

#endif
