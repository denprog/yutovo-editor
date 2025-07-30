#ifndef __AND_H__
#define __AND_H__

#include "only_shape_formula.h"

namespace yutovo
{

class And : public OnlyShapeFormula
{
public:
    And(Element* _parent);
    And(Document* _document);
    And(const And& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml() const;
};

}

#endif
