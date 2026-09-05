/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "text_assignment.h"
#include "../document.h"

namespace yutovo
{

//TextAssignment

TextAssignment::TextAssignment(Element* _parent, bool with_init) :
    Assignment(_parent, with_init, false)
{
    type = ElementType::TEXT_ASSIGNMENT;
}

TextAssignment::TextAssignment(Document* _document, bool with_init) :
    Assignment(_document, with_init)
{
    type = ElementType::TEXT_ASSIGNMENT;
    auto_solve = false;
}

Element* TextAssignment::Clone()
{
    return new TextAssignment(*this);
}

Element* TextAssignment::Create(Element* _parent)
{
    return new TextAssignment(_parent);
}

void TextAssignment::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    MiddleShapeFormula::ToJson(value, alloc);
}

Element* TextAssignment::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new TextAssignment(parent, false);
    return new TextAssignment(document, false);
}

}
