#ifndef __NOT_H__
#define __NOT_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Exclamation : public OnlyShapeFormula
{
public:
    Exclamation(Element* _parent);
    Exclamation(Document* _document);
    Exclamation(const Exclamation& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml();
};

}

#endif
