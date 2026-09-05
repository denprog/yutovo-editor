/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __TEXT_ASSIGNMENT_H__
#define __TEXT_ASSIGNMENT_H__

#include "assignment.h"

namespace yutovo
{

//Assignment that never registers identifiers in the solver - the right part stays a plain editable row
class TextAssignment : public Assignment
{
public:
    TextAssignment(Element* _parent, bool with_init = true);
    TextAssignment(Document* _document, bool with_init = true);
    TextAssignment(const TextAssignment& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

}

#endif
