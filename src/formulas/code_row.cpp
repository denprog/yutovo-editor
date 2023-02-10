#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

CodeRow::CodeRow(Document* _document) :
    Row(_document)
{
    type = ElementType::CODE_ROW;
}

CodeRow::CodeRow(Element* parent) :
    Row(parent, false)
{
    type = ElementType::CODE_ROW;
    AddEmptyElement();
}

Element* CodeRow::Clone()
{
    return new CodeRow(*this);
}

Element* CodeRow::Create(Element* parent)
{
    return new CodeRow(parent);
}

void CodeRow::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    Row::Remake(with_elements, with_parent, with_undo);
    
    document->Remake(parent->id, false, with_undo, false);
}

bool CodeRow::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    for (auto el : _elements)
    {
        if (el->type == ElementType::CODE_BLOCK)
            return false;
    }
    return Row::InsertElements(_elements, with_undo);
}

bool CodeRow::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (Row::GetBeginCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetBeginCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

bool CodeRow::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (Row::GetEndCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetEndCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

void CodeRow::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeString(this)));
}

std::string CodeRow::ToHtml()
{
    std::string s = "<mrow>";
    s += elements->ToHtml();
    s += "</mrow>";
    return s;
}

}
