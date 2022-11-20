#ifndef __PAGE_H__
#define __PAGE_H__

#include "element.h"
#include "style.h"

namespace yutovo
{

class Page : public Element
{
public:
    Page(Element* parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw(const Selections& selections) const;
    virtual void Remake(CaretState& caret_state, bool with_elements);
    virtual void UpdateRect();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo);
    virtual bool DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& res, bool selection);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& res, bool selection);

    virtual ParagraphFormatPtr GetParagraphFormat();

public:
    int page_width = 0;

private:
    PageFormatPtr format;
};

}

#endif
