#ifndef __COMMA_H__
#define __COMMA_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Comma : public OnlyShapeFormula
{
public:
    Comma(Element* _parent);
    Comma(Document* _document);
    Comma(const Comma& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml() const;
};

}

#endif