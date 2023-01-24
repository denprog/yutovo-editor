#include "code_paragraph.h"
#include "code_row.h"

namespace yutovo
{

CodeParagraph::CodeParagraph(Element* _parent) :
    Paragraph(_parent, false)
{
    type = ElementType::CODE_PARAGRAPH;
    AddEmptyElement();
}

CodeParagraph::CodeParagraph(Document* _document) :
    Paragraph(_document, false)
{
    type = ElementType::CODE_PARAGRAPH;
    format = document->paragraph_formats->GetFormat("Code");
    AddEmptyElement();
}

Element* CodeParagraph::Clone()
{
    return new CodeParagraph(*this);
}

Element* CodeParagraph::Create(Element* parent)
{
    return new CodeParagraph(parent);
}

void CodeParagraph::UpdateRect(bool with_elements)
{
    Paragraph::UpdateRect(with_elements);

    // if (!format)
    //     format = parent->GetParagraphFormat();
    rect.left = format->indent_before;
}

// StringFormatPtr CodeParagraph::GetStringFormat()
// {
//     return parent->GetStringFormat();
// }

void CodeParagraph::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeRow(this)));
}

std::string CodeParagraph::ToHtml()
{
    std::string s = "<math xmlns='http://www.w3.org/1998/Math/MathML'>";
    s += Element::ToHtml();
    s += "</math>";
    return s;
}

}
