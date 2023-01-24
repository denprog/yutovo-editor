#include "formula.h"
#include "../document.h"

namespace yutovo
{

Formula::Formula(Element* _parent) : 
    Element(_parent),
    formula_format(GetFormulaFormat())
{
}

Formula::Formula(Document* _document) :
    Element(_document)
{
    document->GetCurrentFormulaFormat(formula_format);
}

bool Formula::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    return false;
}

bool Formula::DeleteElements(bool left, bool with_undo)
{
    return Element::DeleteElements(left, with_undo);
}

bool Formula::ChangeStringFormat(const StringFormatPtr format, bool with_undo)
{
    return false;
}

bool Formula::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo)
{
    return false;
}

bool Formula::Split(const uint max_left_width)
{
    return false;
}

bool Formula::SplitAt(const uint pos)
{
    return false;
}

bool Formula::Merge(const ElementPtr with_element)
{
    return false;
}

void Formula::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = formula_format->left_margin;
    top = formula_format->top_margin;
    right = formula_format->right_margin;
    bottom = formula_format->bottom_margin;
}

bool Formula::HasCaretState()
{
    return true;
}

bool Formula::HasLastCaretState()
{
    return true;
}

StringFormatPtr Formula::GetStringFormat()
{
    return formula_format->string_format;
}

}
