#ifndef __FENCES_H__
#define __FENCES_H__

#include "only_shape_formula.h"

namespace yutovo
{

#define BRACES_Y_OFFSET 0.1

class OpenFence : public OnlyShapeFormula
{
public:
    OpenFence(Element* _parent);
    OpenFence(Document* _document);
    OpenFence(const OpenFence& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual std::string ToHtml();

private:
    StringFormatPtr format;
    static const std::string family_name;
};

class CloseFence : public OnlyShapeFormula
{
public:
    CloseFence(Element* _parent);
    CloseFence(Document* _document);
    CloseFence(const CloseFence& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual std::string ToHtml();

private:
    StringFormatPtr format;
    static const std::string family_name;
};

}

#endif
