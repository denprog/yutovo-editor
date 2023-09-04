#ifndef __SQUARE_ROOT_H__
#define __SQUARE_ROOT_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class SquareRoot : public Formula
{
public:
    SquareRoot(Element* _parent, bool with_init = true);
    SquareRoot(Document* _document, bool with_init = true);
    SquareRoot(const SquareRoot& source);

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);
    
    virtual bool AfterInsert(bool with_undo);
    
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual std::string ToHtml();
    virtual std::u32string ToText();
    virtual void ToParserString(ParserString& str);

protected:
    Element *last = nullptr;
    Shape *shape = nullptr;
};

}

#endif
