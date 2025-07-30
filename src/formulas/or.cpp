#include "or.h"
#include "../document.h"

namespace yutovo
{

//Or

Or::Or(Element* _parent) :
    OnlyShapeFormula(_parent, '|')
{
    type = ElementType::OR;
}

Or::Or(Document* _document) :
    OnlyShapeFormula(_document, '|')
{
    type = ElementType::OR;
}

Or::Or(const Or& source) :
    OnlyShapeFormula(source)
{
}

Element* Or::Clone()
{
    return new Or(*this);
}

Element* Or::Create(Element* _parent)
{
    return new Or(_parent);
}

Element* Or::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Or* p = nullptr;
    if (parent)
        p = new Or(parent);
    else
        p = new Or(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Or::ToHtml() const
{
    return "<mo>&</mo>";
}

}
