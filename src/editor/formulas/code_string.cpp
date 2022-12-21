#include "code_string.h"

namespace yutovo
{

CodeString::CodeString(Element* parent) :
    String(parent)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Element* parent, const std::string str) :
    String(parent, str)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Element* parent, const std::string str, const StringFormatPtr _format) :
    String(parent, str, _format)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Document* _document, const std::string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(const String& source) :
    String(source)
{
    type = ElementType::CODE_STRING;
}

Element* CodeString::Clone()
{
    return new CodeString(*this);
}

Element* CodeString::Create(Element* parent)
{
    return new CodeString(parent);
}

Element* CodeString::Create(Element* parent, const std::string _str, const StringFormatPtr _format)
{
    return new CodeString(parent, _str, _format);
}

void CodeString::Draw() const
{
    String::Draw();
    if (elements->Count() == 0)
        window->DrawRect(GetAbsoluteRect(), Color::Blue());
}

void CodeString::UpdateRect()
{
    String::UpdateRect();

    if (elements->Count() == 0)
        rect.SetRect(0, 0, rect.width + 6, rect.height);
}

Rect CodeString::GetCaretRect(const uint pos) const
{
    Rect r = String::GetCaretRect(pos);
    if (elements->Count() > 0)
        return r;
    r.left += rect.width / 2;
    return r;
}

void CodeString::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    const FormulaFormatPtr f = GetFormulaFormat();
    left = f->left_margin;
    top = f->top_margin;
    right = f->right_margin;
    bottom = f->bottom_margin;
}

std::string CodeString::ToHtml()
{
    std::string s = "<mi>";
    if (elements->Count() == 0)
        s += "Null";
    else
        s += elements->ToHtml();
    s += "</mi>";
    return s;
}

}
