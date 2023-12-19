#include "code_paragraph.h"
#include "code_row.h"

namespace yutovo
{

CodeParagraph::CodeParagraph(Element* _parent, bool with_row) :
    Paragraph(_parent, false)
{
    type = ElementType::CODE_PARAGRAPH;
    if (with_row)
        AddEmptyElement();
}

CodeParagraph::CodeParagraph(Document* _document, bool with_row) :
    Paragraph(_document, false)
{
    type = ElementType::CODE_PARAGRAPH;
    format = document->paragraph_formats->GetFormat("Code");
    if (with_row)
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

Element* CodeParagraph::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    CodeParagraph* p = nullptr;
    if (parent)
        p = new CodeParagraph(parent, false);
    else
        p = new CodeParagraph(document, false);
    if (value.HasMember("format_name") && value["format_name"].IsString())
    {
        auto format_name = value["format_name"].GetString();
        auto f = document->paragraph_formats->GetFormat(format_name);
        if (f)
            p->format = f;
    }
    return p;
}

void CodeParagraph::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeRow(this)));
}

bool CodeParagraph::IsFormula()
{
    return true;
}

std::string CodeParagraph::ToHtml()
{
    std::string s = "<math xmlns='http://www.w3.org/1998/Math/MathML'>";
    s += Element::ToHtml();
    s += "</math>";
    return s;
}

}
