#ifndef __TEXT_H__
#define __TEXT_H__

#include <vector>
#include "page.h"
#include "caret.h"
#include "style.h"

namespace yutovo
{

//The root element of the editor
class Text : public Element
{
public:
    Text(Document* _document);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw(const Selections& selections) const;
    virtual void Remake(CaretState& caret_state, bool with_elements);

    virtual void UpdateRect();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo);
    virtual bool DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo);

    virtual std::string ToHtml();

    virtual Rect GetAbsoluteRect() const;
    
private:
    TextFormatPtr format;
};

}

#endif
