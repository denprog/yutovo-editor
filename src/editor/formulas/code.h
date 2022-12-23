#ifndef __CODE_H__
#define __CODE_H__

#include "code_row.h"

namespace yutovo
{

class Code : public CodeRow
{
public:
    Code(Document* _document);
    Code(Element* parent);
    Code(const Code& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;

    virtual bool AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueSelection();

    virtual StringFormatPtr GetStringFormat();
    virtual FormulaFormatPtr GetFormulaFormat() const;

    virtual std::string ToHtml();

protected:
    FormulaFormatPtr code_format;
    FormulaFormatPtr formula_format;
};

}

#endif
