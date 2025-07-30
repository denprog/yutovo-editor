#ifndef __PERCENT_H__
#define __PERCENT_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Percent : public OnlyShapeFormula
{
public:
    Percent(Element* _parent);
    Percent(Document* _document);
    Percent(const Percent& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml() const;
};

}

#endif