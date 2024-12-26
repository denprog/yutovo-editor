#ifndef __FORMULA_H__
#define __FORMULA_H__

#include "../element.h"

namespace yutovo
{

class Formula : public Element
{
public:
    Formula(Element* _parent);
    Formula(Document* _document);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element);

    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();

    virtual bool GetElementAtCoords(const int x, const int y, ElementId& _id);

    virtual StringFormatPtr GetStringFormat() const;

    virtual bool IsFormula();

public:
    FormulaFormatPtr formula_format;
};

}

#endif
