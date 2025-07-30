#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include "iteration.h"

namespace yutovo
{

class CodeRow;
class Assignment;

class Product : public Iteration
{
public:
    Product(Element* _parent, bool with_init = true);
    Product(Document* _document, bool with_init = true);
    Product(const Product& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);
};

}

#endif
