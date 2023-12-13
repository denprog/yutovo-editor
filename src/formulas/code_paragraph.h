#ifndef __CODE_PARAGRAPH_H__
#define __CODE_PARAGRAPH_H__

#include "../paragraph.h"

namespace yutovo
{

//Group of code rows
class CodeParagraph : public Paragraph
{
public:
    CodeParagraph(Element* _parent, bool with_row = true);
    CodeParagraph(Document* _document, bool with_row = true);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void AddEmptyElement();

    virtual void Select(const CaretState& start, const CaretState& end);

    virtual bool IsFormula();

    virtual std::string ToHtml();
};

}

#endif
