#ifndef __ITERATION_H__
#define __ITERATION_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class CodeRow;
class Assignment;

class Iteration : public Formula
{
public:
    Iteration(Element* _parent, char32_t _symbol, bool with_init = true);
    Iteration(Document* _document, char32_t _symbol, bool with_init = true);
    Iteration(const Iteration& source);

    void Init();

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml();

protected:
    Assignment *lower = nullptr;
    CodeRow *upper = nullptr, *right = nullptr;
    Shape *shape = nullptr;

    StringFormatPtr format;

    const char32_t symbol;
    std::string symbol_str;
    
    static const std::string family_name;
};

}

#endif
