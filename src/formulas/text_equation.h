/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __TEXT_EQUATION_H__
#define __TEXT_EQUATION_H__

#include "equation.h"

namespace yutovo
{

//Equation that never solves anything - the right part stays a plain editable row
class TextEquation : public Equation
{
public:
    TextEquation(Element* _parent, bool with_init = true);
    TextEquation(Document* _document, bool with_init = true);
    TextEquation(const TextEquation& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);

    virtual bool AfterFromJson();

    virtual bool AfterInsert(bool with_undo);

    virtual void Solve();
    virtual void ReSolve(bool if_error = false, bool force = false);
};

}

#endif
