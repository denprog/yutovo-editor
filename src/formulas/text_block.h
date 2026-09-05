/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __TEXT_BLOCK_H__
#define __TEXT_BLOCK_H__

#include "../block.h"
#include "../style.h"

namespace yutovo
{

//Group of code paragraphs for entering formulas without solving
class TextBlock : public Block
{
public:
    TextBlock(Document* _document, bool add_empty = true);
    TextBlock(Element* parent, bool add_empty = true);
    TextBlock(const TextBlock& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void UpdateDrawRect();

    virtual Color GetBackgroundColor() const;

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);

    virtual bool AfterInsert(bool with_undo);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueVerticalMoving();

    virtual StringFormatPtr GetStringFormat() const;
    virtual FormulaFormatPtr GetFormulaFormat() const;
    virtual ParagraphFormatPtr GetParagraphFormat();

    virtual std::string ToHtml() const;

    virtual void AddEmptyElement();

    virtual bool IsFormula();

    //converts equations and assignments of the inserted elements into their text counterparts
    static void ConvertToText(std::vector<ElementPtr>& _elements);

private:
    static void ConvertStringsToCode(Element* el);
    static void CollectRowElements(Element* el, std::vector<ElementPtr>& out);
    static ElementPtr ConvertElementToText(ElementPtr el);

public:
    CodeFormatPtr code_format;
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

}

#endif
