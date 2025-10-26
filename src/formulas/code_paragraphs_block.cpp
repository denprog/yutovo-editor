/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "code_paragraphs_block.h"
#include "code_paragraph.h"
#include "formula.h"

namespace yutovo
{

//CodeParagraphsBlock

CodeParagraphsBlock::CodeParagraphsBlock(Document* _document, bool add_empty) :
    Block(_document)
{
    type = ElementType::CODE_PARAGRAPHS_BLOCK;
    paragraph_format = document->paragraph_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement();
}

CodeParagraphsBlock::CodeParagraphsBlock(Element* parent, bool add_empty) :
    Block(parent)
{
    type = ElementType::CODE_PARAGRAPHS_BLOCK;
    paragraph_format = document->paragraph_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement();
}

Element* CodeParagraphsBlock::Clone()
{
    return new CodeParagraphsBlock(*this);
}

Element* CodeParagraphsBlock::Create(Element* parent)
{
    return new CodeParagraphsBlock(parent, true);
}

Element* CodeParagraphsBlock::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    return new CodeParagraphsBlock(document, false);
}

void CodeParagraphsBlock::Draw() const
{
    Element::Draw();
    window->DrawRect(GetAbsoluteRect(), document->config.bg_selection_color);
}

bool CodeParagraphsBlock::Remake(bool with_elements)
{
    bool changed = Element::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = formula_format->top_margin;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(left_m + formula_format->left_margin, h + top_m);
        h = p->rect.GetBottom() + bottom_m;
    }

    Element::UpdateRect(false);

    UpdateRect();
    rect.width += formula_format->right_margin;
    rect.height += formula_format->bottom_margin;

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    UpdateDrawRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

ParagraphFormatPtr CodeParagraphsBlock::GetParagraphFormat()
{
    return paragraph_format;
}

void CodeParagraphsBlock::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeParagraph(this)));
}

bool CodeParagraphsBlock::IsFormula()
{
    return true;
}

}
