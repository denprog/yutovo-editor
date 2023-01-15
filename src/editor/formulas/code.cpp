#include "code.h"

namespace yutovo
{

Code::Code(Document* _document) :
    CodeRow(_document)
{
    type = ElementType::CODE;
    code_format = document->formula_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");

    AddEmptyElement();
}

Code::Code(Element* parent) :
    CodeRow(parent)
{
    type = ElementType::CODE;
    code_format = document->formula_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");
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

bool Code::AfterInsert(bool with_undo)
{
    CaretState c;
    if (GetFirstCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

void Code::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = code_format->left_margin;
    top = code_format->top_margin;
    right = code_format->right_margin;
    bottom = code_format->bottom_margin;
}

bool Code::HasCaretState()
{
    return true;
}

bool Code::HasLastCaretState()
{
    return true;
}

bool Code::CanContinueSelection()
{
    return false;
}

StringFormatPtr Code::GetStringFormat()
{
    return formula_format->string_format;
}

FormulaFormatPtr Code::GetFormulaFormat() const
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
