/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_paragraph.h"
#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

//CodeParagraph

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

CodeParagraph::CodeParagraph(const Paragraph* source) : 
    Paragraph(source->document, false)
{
    type = ElementType::CODE_PARAGRAPH;
    format = document->paragraph_formats->GetFormat("Code");
    for (int i = 0; i < source->elements->Count(); ++i)
    {
        ElementPtr row(new CodeRow(this, false));
        ElementPtr el = source->elements->Get(i);
        for (int j = 0; j < el->elements->Count(); ++j)
        {
            auto r = el->elements->Get(i);
            if (r->type == ElementType::STRING)
            {
                //change type of string
                String* str = (String*)r.get();
                row->elements->Add(ElementPtr(new CodeString(*str)));
            }
            else
                row->elements->Add(r);
        }
        elements->Add(row);
    }
}

Element* CodeParagraph::Clone()
{
    return new CodeParagraph(*this);
}

Element* CodeParagraph::Create(Element* parent)
{
    return new CodeParagraph(parent);
}

Element* CodeParagraph::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
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

void CodeParagraph::Normalize()
{
    Paragraph::Normalize();

    for (int i = 1; i < elements->Count();)
    {
        auto el1 = elements->Get(i - 1);
        auto el2 = elements->Get(i);
        if (el1->type == ElementType::CODE_ROW && el2->type == ElementType::CODE_ROW)
        {
            //merge the two rows
            el1->Merge(el2);
        }
        else
            ++i;
    }
}

bool CodeParagraph::IsFormula()
{
    return true;
}

std::string CodeParagraph::ToHtml() const
{
    std::string s = "<math xmlns='http://www.w3.org/1998/Math/MathML'>";
    s += Element::ToHtml();
    s += "</math>";
    return s;
}

ElementPtr CodeParagraph::GetPlainRow()
{
    ElementPtr row(new CodeRow(parent, false));
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto r = elements->Get(i);
        for (int j = 0; j < r->elements->Count(); ++j)
        {
            auto _el = r->elements->Get(j);
            row->elements->Add(_el);
        }
    }
    return row;
}

}
