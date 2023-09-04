#ifndef __DIVISION_H__
#define __DIVISION_H__

#include "formula.h"
#include "middle_shape_formula.h"
#include "code_row.h"

namespace yutovo
{

class Division : public MiddleShapeFormula
{
public:
    Division(Element* _parent, bool with_init = true);
    Division(Document* _document, bool with_init = true);
    Division(const Division& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual std::string ToHtml();
    virtual std::u32string ToText();
    virtual void ToParserString(ParserString& str);

    void AddNumerator(ElementPtr numerator);
    void AddDenomerator(ElementPtr denomerator);
};

}

#endif
