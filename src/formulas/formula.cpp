#include "formula.h"
#include "../document.h"

namespace yutovo
{

//Formula

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

bool Formula::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    return parent->InsertElements(_elements, with_undo, changed_element);
}

bool Formula::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element)
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

void Formula::ElementIdChanged(const ElementId& last_id)
{
    int start, size;
    if (document->HasErrorMark(last_id, start, size))
    {
        document->RemoveErrorMarks(last_id);
        document->AddErrorMark(id, start, size);
    }
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

bool Formula::GetElementAtCoords(const int x, const int y, ElementId& _id)
{
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (el->GetElementAtCoords(x, y, _id))
            return true;
    }
    Rect r = parent->GetAbsoluteRect(GetCaretRect());
    if (r.IsPointInside(x, y))
    {
        _id = id;
        return true;
    }
    return false;
}

StringFormatPtr Formula::GetStringFormat() const
{
    return formula_format->string_format;
}

bool Formula::IsFormula()
{
    return true;
}

}
