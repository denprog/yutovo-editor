#ifndef __PLUS_H__
#define __PLUS_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Plus : public OnlyShapeFormula
{
public:
    Plus(Element* _parent);
    Plus(Document* _document);
    Plus(const Plus& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml();
};

}

#endif
