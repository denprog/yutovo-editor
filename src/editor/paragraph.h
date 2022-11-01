#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include "element.h"
#include "style.h"

namespace yutovo
{

class Paragraph : public Element
{
public:
    Paragraph(Element* parent);

    virtual Element* Clone();

    virtual void Draw(const Selections& selections) const;
    virtual void Remake(CaretState& caret_state, bool with_elements);
    virtual void UpdateRect();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo);
    virtual bool DeleteElements(const CaretState& before_state, CaretState& after_state, bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& res, bool selection);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& res, bool selection);

    virtual std::string ToHtml();

protected:
    ParagraphFormatPtr format;
};

}

#endif
