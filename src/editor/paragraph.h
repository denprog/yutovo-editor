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

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements);
    virtual void UpdateRect();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr _format, bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual StringFormatPtr GetStringFormat();
    
    virtual std::string ToHtml();

protected:
    ParagraphFormatPtr format;
};

}

#endif
