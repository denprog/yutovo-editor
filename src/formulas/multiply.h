#ifndef __MULTIPLY_H__
#define __MULTIPLY_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Multiply : public OnlyShapeFormula
{
public:
    Multiply(Element* _parent);
    Multiply(Document* _document);
    Multiply(const Multiply& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::u32string ToText();
    virtual std::string ToHtml();
};

}

#endif
