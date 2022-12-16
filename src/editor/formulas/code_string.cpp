#include "code_string.h"

namespace yutovo
{

CodeString::CodeString(Element* parent) :
    String(parent)
{
}

CodeString::CodeString(Element* parent, const std::string str) :
    String(parent, str)
{
}

CodeString::CodeString(Element* parent, const std::string str, const StringFormatPtr _format) :
    String(parent, str, _format)
{
}

CodeString::CodeString(Document* _document, const std::string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
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
        rect.SetRect(0, 0, rect.width + 4, rect.height);
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
