#ifndef __CODE_BLOCK_H__
#define __CODE_BLOCK_H__

#include "../block.h"
#include "../style.h"

namespace yutovo
{

//Group of code paragraphs
class CodeBlock : public Block
{
public:
    CodeBlock(Document* _document, uint _code_id, bool add_empty = true, bool list_identifiers = true);
    CodeBlock(Element* parent, uint _code_id, bool add_empty = true);
    CodeBlock(Document* _document, Element* parent, uint _code_id, bool add_empty = true);
    CodeBlock(const CodeBlock& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);

    virtual bool AfterInsert(bool with_undo);
    virtual void ElementIdChanged(const ElementId& last_id);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueVerticalMoving();

    virtual StringFormatPtr GetStringFormat() const;
    virtual FormulaFormatPtr GetFormulaFormat() const;
    virtual ParagraphFormatPtr GetParagraphFormat();

    virtual void AddEmptyElement();

    virtual bool IsFormula();

public:
    uint code_id = 0; //id for unification of code blocks

    CodeFormatPtr code_format;
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

}

#endif
