#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "middle_shape_formula.h"
#include "code_row.h"

namespace yutovo
{

class Assignment : public MiddleShapeFormula
{
public:
    Assignment(Element* _parent, bool with_init = true, bool _auto_solve = true);
    Assignment(Document* _document, bool with_init = true);
    Assignment(const Assignment& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual bool Remake(bool with_elements = false);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    
    virtual bool AfterInsert(bool with_undo);
    virtual void BeforeDelete();

    virtual void Solve();
    virtual void ReSolve(bool if_error = false);
    void PutResult(Result result);

    virtual std::string ToHtml();
    virtual std::u32string ToText();
    virtual void ToParserString(ParserString& str);

protected:
    friend class ResultTask;
    
    std::u32string solve_sign = U"=";
    std::string draw_sign = ":=";

    std::u32string last_identifier;
    ParserString last_expression;
    bool last_error = false;
    bool delay = false; //don't delay on the first calculation
    bool auto_solve = true;
};

}

#endif
