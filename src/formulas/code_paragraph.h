/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_PARAGRAPH_H__
#define __CODE_PARAGRAPH_H__

#include "../paragraph.h"

namespace yutovo
{

//Group of code rows
class CodeParagraph : public Paragraph
{
public:
    CodeParagraph(Element* _parent, bool with_row = true);
    CodeParagraph(Document* _document, bool with_row = true);
    CodeParagraph(const Paragraph* source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Normalize();

    virtual void AddEmptyElement();

    virtual bool IsFormula();

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml() const;

    virtual ElementPtr GetPlainRow();
};

}

#endif
