/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __CODE_ROW_H__
#define __CODE_ROW_H__

#include "../row.h"

namespace yutovo
{

class CodeRow : public Row
{
public:
    CodeRow(Document* _document);
    CodeRow(Element* parent, bool with_string = true);
    CodeRow(const CodeRow& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Normalize();

    virtual bool Merge(const ElementPtr with_element);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);

    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);

    virtual void AddEmptyElement();

    virtual bool IsFormula();

    virtual std::string ToHtml() const;
};

}

#endif
