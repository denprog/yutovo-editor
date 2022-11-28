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

    virtual void Draw() const;
    virtual void Remake(bool with_elements);
    virtual void UpdateRect();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual ParagraphFormatPtr GetParagraphFormat();

public:
    int page_width = 0;

private:
    PageFormatPtr format;
};

}

#endif
