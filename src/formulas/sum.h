#ifndef __SUM_H__
#define __SUM_H__

#include "iteration.h"

namespace yutovo
{

class CodeRow;
class Assignment;

class Sum : public Iteration
{
public:
    Sum(Element* _parent, bool with_init = true);
    Sum(Document* _document, bool with_init = true);
    Sum(const Sum& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);
};

}

#endif
