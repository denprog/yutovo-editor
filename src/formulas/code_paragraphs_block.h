/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_PARAGRAPHS_BLOCK_H__
#define __CODE_PARAGRAPHS_BLOCK_H__

#include "../block.h"

namespace yutovo
{

//Group of code paragraphs
class CodeParagraphsBlock : public Block
{
public:
    CodeParagraphsBlock(Document* _document, bool add_empty);
    CodeParagraphsBlock(Element* parent, bool add_empty);
    CodeParagraphsBlock(const CodeParagraphsBlock& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    ParagraphFormatPtr GetParagraphFormat();

    std::string ToHtml() const;

    virtual void AddEmptyElement();

    virtual bool IsFormula();

public:
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

}

#endif
