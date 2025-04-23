#ifndef __MINUS_H__
#define __MINUS_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Minus : public OnlyShapeFormula
{
public:
    Minus(Element* _parent);
    Minus(Document* _document);
    Minus(const Minus& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml();
};

}

#endif
