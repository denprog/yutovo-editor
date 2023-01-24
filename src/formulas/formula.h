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

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo);

    virtual bool Split(const uint max_left_width);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();

    virtual StringFormatPtr GetStringFormat();

protected:
    FormulaFormatPtr formula_format;
};

}

#endif
