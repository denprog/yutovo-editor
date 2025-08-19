/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include "element.h"
#include "style.h"
#include "document.h"

namespace yutovo
{

//Group of rows
class Paragraph : public Element
{
public:
    Paragraph(Element* _parent, bool with_row = true);
    Paragraph(Document* _document, bool with_row = true);
    Paragraph(Document* _document, ParagraphFormatPtr _format, bool with_row = true);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;

    virtual bool Remake(bool with_elements = false);
    virtual void Normalize();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr _format, bool with_undo, ElementId& changed_element);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);

    virtual bool CanContinueSelection();

    virtual bool GetNearestCaretState(const int x, const int y, CaretState& caret_state);

    virtual void AddEmptyElement();

    virtual bool IsEmpty();

    virtual ParagraphFormatPtr GetParagraphFormat();
    virtual StringFormatPtr GetStringFormat() const;
    
    virtual std::string ToHtml() const;

    virtual ElementPtr GetPlainRow();
    virtual void MakePlain();

public:
    ParagraphFormatPtr format;
    StringFormatPtr current_string_format;
};

}

#endif
