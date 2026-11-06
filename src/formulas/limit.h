/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __LIMIT_FORMULA_H__
#define __LIMIT_FORMULA_H__

#include "formula.h"
#include "code_row.h"

namespace yutovo
{

class CodeString;

//Limit - the mathematical limit: the non-editable "lim" text with the "variable → value" row under it.
class Limit : public Formula
{
public:
    Limit(Element* _parent, bool with_init = true);
    Limit(Document* _document, bool with_init = true);
    Limit(const Limit& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    void Init();

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void Normalize();

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;

    virtual void ToParserString(ParserString& str);

    //The variable and the value rows are filled - the containing row wraps the following segment into a limit(...) call
    bool IsFilled() const;

    //The tail of the limit(...) parser call: ",", the variable, ",", the value, an optional direction argument and ")".
    void AddParserArguments(ParserString& str) const;

protected:
    CodeString* GetLim() const; //non-editable "lim" text
    CodeRow<>* GetVariable() const; //limit variable
    CodeString* GetArrow() const; //non-editable tends-to arrow
    CodeRow<>* GetValue() const; //the value the variable tends to

    bool IsOnSign(const CaretState& caret_state) const; //the state stands on the lim text or the arrow
    bool IsInsideSign(const CaretState& caret_state) const; //the state stands inside the lim text or the arrow
    bool CaretOnChild() const; //the caret stands on or inside one of the children
    bool SkipLeft(CaretState& caret_state, Selection* select) const;
    bool SkipRight(CaretState& caret_state, Selection* select) const;
    CodeRow<>* GetNearestRow(const int x) const; //the variable or the value row closer to the horizontal position
    int GetDirection() const; //0 - no direction, 1 - "+" exponent, -1 - "-" exponent

protected:
    static const std::u32string lim_text;
    const char32_t arrow_symbol = U'→';
    std::string arrow_str;
};

}

#endif
