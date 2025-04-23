#ifndef __XOR_H__
#define __XOR_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Xor : public OnlyShapeFormula
{
public:
    Xor(Element* _parent);
    Xor(Document* _document);
    Xor(const Xor& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml();
};

}

#endif
