/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __MATRIX_FORMULA_H__
#define __MATRIX_FORMULA_H__

#include "formula.h"
#include "shape.h"
#include "code_row.h"

namespace yutovo
{

//Matrix - a grid of editable cells between vertical brackets
class Matrix : public Formula
{
public:
    Matrix(Element* _parent, bool with_init = true, uint _rows = 2, uint _columns = 2);
    Matrix(Document* _document, bool with_init = true, uint _rows = 2, uint _columns = 2);
    Matrix(const Matrix& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    void Init();

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void Normalize();

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool HasLastCaretState();
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

    void Resize(uint _rows, uint _columns);

    uint GetRows() const;
    uint GetColumns() const;

protected:
    Shape* GetLeftShape() const;
    Shape* GetRightShape() const;
    CodeRow<>* GetCell(const uint row, const uint column) const;

    bool IsInside(const CaretState& caret_state) const;

    virtual std::u32string BuildText() const;
    void BuildParserString(ParserString& str) const;

    CodeRow<>* FindNearestCell(const uint row, const int x) const;
    bool GetCurrentCell(uint& row, uint& column) const;

protected:
    StringFormatPtr bracket_format;

    uint rows = 2;
    uint columns = 2;

    const char32_t left_bracket = U'(';
    const char32_t right_bracket = U')';

    std::string left_bracket_str;
    std::string right_bracket_str;

    std::vector<int> row_tops; //vertical positions of the grid rows, relative to the element rect
    std::vector<int> row_heights;
    std::vector<int> column_lefts; //horizontal positions of the grid columns, relative to the element rect
    std::vector<int> column_widths;

    static const std::string family_name;
};

//Determinant - a matrix with straight vertical line brackets
class Determinant : public Matrix
{
public:
    Determinant(Element* _parent, bool with_init = true, uint _rows = 2, uint _columns = 2);
    Determinant(Document* _document, bool with_init = true, uint _rows = 2, uint _columns = 2);
    Determinant(const Determinant& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;

    virtual void ToParserString(ParserString& str);

protected:
    virtual std::u32string BuildText() const;

    const char32_t left_bracket = U'|';
    const char32_t right_bracket = U'|';
};

}

#endif
