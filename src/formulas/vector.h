/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __VECTOR_FORMULA_H__
#define __VECTOR_FORMULA_H__

#include "formula.h"
#include "shape.h"
#include "code_row.h"

namespace yutovo
{

//Vector - the vector notation: an arrow over an editable row, the parser output is the row content.
class Vector : public Formula
{
public:
    Vector(Element* _parent, bool with_init = true);
    Vector(Document* _document, bool with_init = true);
    Vector(const Vector& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    void Init();

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool HasCaretState();
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual bool GetNearestCaretState(const int x, const int y, CaretState& caret_state);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);

    virtual bool AfterInsert(bool with_undo);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual bool UseSelection();

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;

    virtual void ToParserString(ParserString& str);

protected:
    Shape* GetShape() const; //the arrow above the row
    CodeRow<>* GetRow() const; //the vector content row
    bool IsOnArrow(const CaretState& caret_state) const; //the state stands on the arrow
    bool IsInsideArrow(const CaretState& caret_state) const; //the state stands inside the arrow
    bool CaretOnChild() const; //the caret stands on or inside one of the children

protected:
    const char32_t arrow_symbol = U'⃗'; //combining right arrow above
    std::string arrow_str;
};

}

#endif
