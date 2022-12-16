#include "code.h"

namespace yutovo
{

Code::Code(Document* _document) :
    CodeRow(_document)
{
    type = ElementType::CODE;
    document->GetCurrentFormulaFormat(formula_format);
    AddEmptyElement();
}

Code::Code(Element* parent) :
    CodeRow(parent)
{
    type = ElementType::CODE;
    document->GetCurrentFormulaFormat(formula_format);
}

Element* Code::Clone()
{
    return new Code(*this);
}

Element* Code::Create(Element* parent)
{
    return new Code(parent);
}

void Code::Draw() const
{
    Row::Draw();
    window->DrawRect(GetAbsoluteRect(), Color::Red());
}

void Code::AfterInsert()
{
    CaretState c;
    if (GetFirstCaretState(c, nullptr))
        caret->SetState(c);
}

void Code::UpdateRect()
{
    Row::UpdateRect();
    rect.SetRect(0, 0, rect.width, rect.height + 4);
}

bool Code::HasCaretState()
{
    return true;
}

bool Code::HasLastCaretState()
{
    return true;
}

StringFormatPtr Code::GetStringFormat()
{
    return formula_format->string_format;
}

FormulaFormatPtr Code::GetFormulaFormat()
{
    return formula_format;
}

std::string Code::ToHtml()
{
    std::string s = "<math xmlns='http://www.w3.org/1998/Math/MathML'>";
    s += CodeRow::ToHtml();
    s += "</math>";
    return s;
}

}
