/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "text_equation.h"
#include "../document.h"

namespace yutovo
{

//TextEquation

TextEquation::TextEquation(Element* _parent, bool with_init) :
    Equation(_parent, yutovo_solver::ResultType::NONE, with_init)
{
    type = ElementType::TEXT_EQUATION;
}

TextEquation::TextEquation(Document* _document, bool with_init) :
    Equation(_document, yutovo_solver::ResultType::NONE, with_init)
{
    type = ElementType::TEXT_EQUATION;
}

Element* TextEquation::Clone()
{
    return new TextEquation(*this);
}

Element* TextEquation::Create(Element* _parent)
{
    return new TextEquation(_parent);
}

void TextEquation::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    MiddleShapeFormula::ToJson(value, alloc);
}

Element* TextEquation::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new TextEquation(parent, false);
    return new TextEquation(document, false);
}

bool TextEquation::AfterFromJson()
{
    //the right part is a plain editable row, there is no result row to restore
    return MiddleShapeFormula::AfterFromJson();
}

bool TextEquation::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        GetFirst()->elements->Clear();
    ready = false;
    for (int i = pos - 1; i >= 0; --i)
    {
        auto el = parent->elements->Get(i);
        if (el->type == ElementType::ASSIGNMENT || el->type == ElementType::EQUATION ||
            el->type == ElementType::TEXT_ASSIGNMENT || el->type == ElementType::TEXT_EQUATION)
            break;
        GetFirst()->elements->Move(el, 0);
    }
    ready = true;

    //the right part is editable - put the caret into it, typing on the shape is not allowed
    CaretState c;
    GetLast()->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    return true;
}

void TextEquation::Solve()
{
}

void TextEquation::ReSolve(bool if_error, bool force)
{
}

}
