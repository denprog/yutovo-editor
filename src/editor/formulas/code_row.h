#ifndef __CODE_ROW_H__
#define __CODE_ROW_H__

#include "../row.h"

namespace yutovo
{

class CodeRow : public Row
{
public:
    CodeRow(Document* _document);
    CodeRow(Element* parent);
    CodeRow(const CodeRow& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);

    virtual void AddEmptyElement();

    virtual std::string ToHtml();
};

}

#endif
