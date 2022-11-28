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

    virtual void Draw() const;
    virtual void Remake(bool with_elements);

    virtual void UpdateRect();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);

    virtual std::string ToHtml();

    virtual Rect GetAbsoluteRect() const;
    
private:
    TextFormatPtr format;
};

}

#endif
