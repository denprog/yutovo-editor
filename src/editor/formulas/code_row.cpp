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
    Row(parent)
{
    type = ElementType::CODE_ROW;
    elements->Clear();
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

bool CodeRow::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    for (auto el : _elements)
    {
        if (el->type == ElementType::CODE)
            return false;
    }
    return Row::InsertElements(_elements, with_undo);
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
