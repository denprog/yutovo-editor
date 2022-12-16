#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

CodeRow::CodeRow(Document* _document) :
    Row(_document)
{
}

CodeRow::CodeRow(Element* parent) :
    Row(parent)
{
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
