/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "matrix.h"
#include "code_string.h"
#include "../document.h"
#include <limits>

namespace yutovo
{

static const uint max_matrix_size = 100;

//Matrix

const std::string Matrix::family_name = "Arial";

Matrix::Matrix(Element* _parent, bool with_init, uint _rows, uint _columns) :
    Formula(_parent),
    rows(_rows < 1 ? 1 : (_rows > max_matrix_size ? max_matrix_size : _rows)),
    columns(_columns < 1 ? 1 : (_columns > max_matrix_size ? max_matrix_size : _columns)),
    left_bracket_str(ToBasicString(std::u32string(1, left_bracket))),
    right_bracket_str(ToBasicString(std::u32string(1, right_bracket)))
{
    type = ElementType::MATRIX;
    if (with_init)
        Init();
}

Matrix::Matrix(Document* _document, bool with_init, uint _rows, uint _columns) :
    Formula(_document),
    rows(_rows < 1 ? 1 : (_rows > max_matrix_size ? max_matrix_size : _rows)),
    columns(_columns < 1 ? 1 : (_columns > max_matrix_size ? max_matrix_size : _columns)),
    left_bracket_str(ToBasicString(std::u32string(1, left_bracket))),
    right_bracket_str(ToBasicString(std::u32string(1, right_bracket)))
{
    type = ElementType::MATRIX;
    if (with_init)
        Init();
}

Matrix::Matrix(const Matrix& source) :
    Formula(source),
    rows(source.rows),
    columns(source.columns),
    left_bracket_str(source.left_bracket_str),
    right_bracket_str(source.right_bracket_str)
{
}

Element* Matrix::Clone()
{
    return new Matrix(*this);
}

Element* Matrix::Create(Element* _parent)
{
    return new Matrix(_parent);
}

Element* Matrix::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    Matrix* el = parent ? new Matrix(parent, false) : new Matrix(document, false);
    if (value.HasMember("rows") && value["rows"].IsUint())
        el->rows = value["rows"].GetUint();
    if (value.HasMember("columns") && value["columns"].IsUint())
        el->columns = value["columns"].GetUint();
    el->rows = el->rows < 1 ? 1 : (el->rows > max_matrix_size ? max_matrix_size : el->rows);
    el->columns = el->columns < 1 ? 1 : (el->columns > max_matrix_size ? max_matrix_size : el->columns);
    return el;
}

void Matrix::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Formula::ToJson(value, alloc);
    value.AddMember("rows", rows, alloc);
    value.AddMember("columns", columns, alloc);
}

void Matrix::Init()
{
    ElementPtr left(new Shape(this));
    left->editable = false;
    elements->Add(left);
    for (uint i = 0; i < rows * columns; ++i)
        elements->Add(ElementPtr(new CodeRow<>(this))); //each cell with its empty string placeholder
    ElementPtr right(new Shape(this));
    right->editable = false;
    elements->Add(right);

    UpdateLevel(level);
}

bool Matrix::AfterFromJson()
{
    if (elements->Count() != 2 + rows * columns)
        return false;
    if (elements->Get(0)->type != ElementType::SHAPE || elements->Get(elements->Count() - 1)->type != ElementType::SHAPE)
        return false;
    for (uint i = 1; i + 1 < elements->Count(); ++i)
    {
        if (elements->Get(i)->type != ElementType::CODE_ROW)
            return false;
    }

    //keep the placeholder invariant - a cell may have been saved without elements
    for (uint i = 1; i + 1 < elements->Count(); ++i)
    {
        if (elements->Get(i)->elements->Count() == 0)
            elements->Get(i)->AddEmptyElement();
    }

    UpdateLevel(level);
    return true;
}

void Matrix::Draw() const
{
    Color color = document->selection.IsSelected(id) ? GetBackgroundColor() : document->config.shapes_color;
    Color bg_color = document->selection.IsSelected(id) ? document->config.bg_selection_color : GetBackgroundColor();

    GetLeftShape()->draw_func =
        [&](const Rect& r)
        {
            if (bracket_format)
                window->DrawText(left_bracket_str, bracket_format, r, color, bg_color, false);
        };
    GetRightShape()->draw_func =
        [&](const Rect& r)
        {
            if (bracket_format)
                window->DrawText(right_bracket_str, bracket_format, r, color, bg_color, false);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Matrix::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    row_tops.assign(rows, 0);
    row_heights.assign(rows, 0);
    column_lefts.assign(columns, 0);
    column_widths.assign(columns, 0);

    for (uint r = 0; r < rows; ++r)
    {
        for (uint c = 0; c < columns; ++c)
        {
            CodeRow<>* cell = GetCell(r, c);
            if (!cell)
                return changed;
            if (cell->rect.width > column_widths[c])
                column_widths[c] = cell->rect.width;
            if (cell->rect.height > row_heights[r])
                row_heights[r] = cell->rect.height;
        }
    }

    const int h_gap = std::max(4, (int)std::round(4 * document->config.scale));
    const int v_gap = std::max(2, (int)std::round(2 * document->config.scale));

    int grid_w = 0;
    for (uint c = 0; c < columns; ++c)
    {
        column_lefts[c] = grid_w;
        grid_w += column_widths[c];
        if (c + 1 < columns)
            grid_w += h_gap;
    }
    int grid_h = 0;
    for (uint r = 0; r < rows; ++r)
    {
        row_tops[r] = grid_h;
        grid_h += row_heights[r];
        if (r + 1 < rows)
            grid_h += v_gap;
    }

    auto* left_shape = GetLeftShape();
    auto* right_shape = GetRightShape();
    const int bracket_h = grid_h + grid_h / 4 + std::max(4, (int)std::round(4 * document->config.scale));
    const int bracket_gap = std::max(2, (int)std::round(2 * document->config.scale));
    Size s;
    int shape_baseline = 0;
    int size = window->GetSymbolSize(left_bracket, bracket_h, family_name, s, shape_baseline);
    if (size != 0)
    {
        left_shape->rect.SetRect(0, 0, s.width, s.height);
        bracket_format = document->GetStringFormat(family_name, size, false, false, false, false, false, false, Color::Black(),
            Color::White(), Color::Blue());
    }

    const int grid_top = std::max(0, left_shape->rect.height * 7 / 12 - grid_h / 2);
    for (uint r = 0; r < rows; ++r)
    {
        row_tops[r] += grid_top;
        for (uint c = 0; c < columns; ++c)
        {
            CodeRow<>* cell = GetCell(r, c);
            cell->rect.Move(s.width + bracket_gap + column_lefts[c] + (column_widths[c] - cell->rect.width) / 2,
                row_tops[r] + (row_heights[r] - cell->rect.height) / 2);
        }
    }

    size = window->GetSymbolSize(right_bracket, bracket_h, family_name, s, shape_baseline);
    if (size != 0)
    {
        right_shape->rect.SetSize(s.width, s.height);
        right_shape->rect.Move(left_shape->rect.width + grid_w + h_gap * 2, 0);
    }

    baseline = grid_top + grid_h / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Matrix::Normalize()
{
    Element::Normalize(); //skip Formula::Normalize - it would merge the grid cells into single rows
}

bool Matrix::IsInside(const CaretState& caret_state) const
{
    for (uint i = 0; i < elements->Count(); ++i)
    {
        if (caret_state.id == elements->Get(i)->id || caret_state.IsInsideElement(elements->Get(i)->id))
            return true;
    }
    return false;
}

bool Matrix::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsInside(caret->GetCaretState()))
        return false;

    bool res = Formula::GetLeftCaretState(caret_state, select);
    if (res && select && (caret_state.id == GetLeftShape()->id || caret_state.id == GetRightShape()->id))
    {
        //a bracket is not selected alone - the whole matrix is selected as one element
        caret_state.SetState(id);
        select->Clear();
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return res;
    }
    //the brackets are not caret stops - the single stop before the grid is the matrix itself
    if (res && !select && caret_state.id == GetLeftShape()->id)
        caret_state.SetState(id);
    //entering from the right goes straight to the end of the last cell - no stop on the right bracket
    if (res && !select && caret_state.id == GetRightShape()->id)
    {
        CaretState last;
        if (GetCell(rows - 1, columns - 1)->GetLastCaretState(last, nullptr))
            caret_state = last;
    }
    return res;
}

bool Matrix::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    //the brackets are not caret stops - entering the matrix starts in the first cell
    return GetCell(0, 0)->GetFirstCaretState(caret_state, select);
}

bool Matrix::HasLastCaretState()
{
    return false;
}

bool Matrix::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsInside(caret->GetCaretState()))
        return false;

    bool res = Formula::GetRightCaretState(caret_state, select);
    if (res && select && (caret_state.id == GetLeftShape()->id || caret_state.id == GetRightShape()->id))
    {
        caret_state.SetState(id);
        select->Clear();
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return res;
    }
    //the brackets are not caret stops - the single stop after the grid is the position after the matrix
    if (res && !select && caret_state.id == GetRightShape()->id)
        caret_state.SetState(id, elements->Count(), true);
    return res;
}

bool Matrix::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsInside(caret->GetCaretState()))
        return false;

    return Formula::GetWordLeftCaretState(caret_state, select);
}

bool Matrix::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select && IsInside(caret->GetCaretState()))
        return false;

    return Formula::GetWordRightCaretState(caret_state, select);
}

bool Matrix::GetCurrentCell(uint& row, uint& column) const
{
    CaretState cs = caret->GetCaretState();
    if (cs.id.size() <= id.size())
        return false;
    uint child_index = cs.id[id.size()];
    if (child_index < 1 || child_index + 1 >= elements->Count())
        return false;
    row = (child_index - 1) / columns;
    column = (child_index - 1) % columns;
    return true;
}

CodeRow<>* Matrix::FindNearestCell(const uint row, const int x) const
{
    Rect abs_rect = GetAbsoluteRect();
    CodeRow<>* nearest = GetCell(row, 0);
    int min_dist = std::numeric_limits<int>::max();
    for (uint c = 0; c < columns; ++c)
    {
        int left = abs_rect.left + column_lefts[c];
        int right = left + column_widths[c];
        int dist = x < left ? left - x : (x > right ? x - right : 0);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = GetCell(row, c);
        }
    }
    return nearest;
}

bool Matrix::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    uint row, column;
    if (GetCurrentCell(row, column))
    {
        if (row == 0)
            return parent->GetTopCaretState(x, GetAbsoluteRect().top, caret_state, select);
        return GetCell(row - 1, column)->GetTopCaretState(x, y, caret_state, select);
    }

    //entering from outside: the bottom-most grid row that is entirely above y
    Rect abs_rect = GetAbsoluteRect();
    int top_row = -1;
    for (uint r = 0; r < rows; ++r)
    {
        if (y < abs_rect.top + row_tops[r] + row_heights[r])
            break;
        top_row = r;
    }
    if (top_row < 0)
        return parent->GetTopCaretState(x, abs_rect.top, caret_state, select);
    return FindNearestCell(top_row, x)->GetTopCaretState(x, y, caret_state, select);
}

bool Matrix::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    uint row, column;
    if (GetCurrentCell(row, column))
    {
        if (row + 1 >= rows)
            return parent->GetBottomCaretState(x, GetAbsoluteRect().GetBottom(), caret_state, select);
        return GetCell(row + 1, column)->GetBottomCaretState(x, y, caret_state, select);
    }

    //entering from outside: the top-most grid row that is entirely below y
    Rect abs_rect = GetAbsoluteRect();
    int bottom_row = -1;
    for (int r = rows - 1; r >= 0; --r)
    {
        if (y > abs_rect.top + row_tops[r])
            break;
        bottom_row = r;
    }
    if (bottom_row < 0)
        return parent->GetBottomCaretState(x, abs_rect.GetBottom(), caret_state, select);
    return FindNearestCell(bottom_row, x)->GetBottomCaretState(x, y, caret_state, select);
}

bool Matrix::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    //the caret before or after the matrix (the brackets are not caret stops, such a state carries the matrix id)
    //or on a bracket deletes the whole element
    CaretState cs = caret->GetCaretState();
    if (cs.id == id || caret->IsOnElement(GetLeftShape()->id) || caret->IsOnElement(GetRightShape()->id))
    {
        if (with_undo)
            document->StoreUndo(parent->id);
        int p = parent->elements->GetElementPos(id);
        caret->SetState(id);
        auto t = parent->elements->Get(p); //for not removing this element until this function ends
        parent->elements->Remove(id);
        parent->Normalize();
        changed_element = parent->id;
        return true;
    }

    uint start, size;
    if (selection->Has(id, start, size))
    {
        //a selected cell is cleared back to its placeholder - the cell itself cannot be deleted
        if (size == 1 && start >= 1 && start + 1 < elements->Count() && elements->Get(start)->type == ElementType::CODE_ROW)
        {
            CodeRow<>* cell = (CodeRow<>*)elements->Get(start).get();
            if (with_undo)
                document->StoreUndo(cell->id);
            cell->elements->Clear();
            cell->Normalize(); //restores the empty string placeholder
            selection->Remove(id, start, size);
            changed_element = id;
            return true;
        }
        return false;
    }

    //the caret is inside a cell - the cell and its placeholder are never deleted
    return false;
}

void Matrix::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    for (uint r = 0; r < rows; ++r)
    {
        for (uint c = 0; c < columns; ++c)
            GetCell(r, c)->UpdateLevel(_level + 1);
    }
}

bool Matrix::AfterInsert(bool with_undo)
{
    if (!caret)
        return false;
    CaretState c;
    if (GetCell(0, 0)->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

std::string Matrix::ToHtml() const
{
    if (elements->Count() != 2 + rows * columns)
        return "";

    std::string open_fence = "<mo>(</mo>";
    std::string close_fence = "<mo>)</mo>";
    std::string s = open_fence + "<mtable>";
    for (uint r = 0; r < rows; ++r)
    {
        s += "<mtr>";
        for (uint c = 0; c < columns; ++c)
            s += "<mtd>" + GetCell(r, c)->ToHtml() + "</mtd>";
        s += "</mtr>";
    }
    s += "</mtable>" + close_fence;
    return s;
}

std::u32string Matrix::BuildText() const
{
    if (elements->Count() != 2 + rows * columns)
        return U"";

    std::u32string s = U"[";
    for (uint r = 0; r < rows; ++r)
    {
        if (r > 0)
            s += U",";
        s += U"[" + GetCell(r, 0)->ToText();
        for (uint c = 1; c < columns; ++c)
            s += U"," + GetCell(r, c)->ToText();
        s += U"]";
    }
    s += U"]";
    return s;
}

std::u32string Matrix::ToText() const
{
    return BuildText();
}

void Matrix::BuildParserString(ParserString& str) const
{
    str.Add(id, U"([[");
    for (uint r = 0; r < rows; ++r)
    {
        if (r > 0)
            str.Add(id, U"],[");
        GetCell(r, 0)->ToParserString(str);
        for (uint c = 1; c < columns; ++c)
        {
            str.Add(id, U",");
            GetCell(r, c)->ToParserString(str);
        }
    }
    str.Add(id, U"]])");
}

void Matrix::ToParserString(ParserString& str)
{
    BuildParserString(str);
}

void Matrix::Resize(uint _rows, uint _columns)
{
    _rows = _rows < 1 ? 1 : (_rows > max_matrix_size ? max_matrix_size : _rows);
    _columns = _columns < 1 ? 1 : (_columns > max_matrix_size ? max_matrix_size : _columns);
    if (_rows == rows && _columns == columns)
        return;

    CaretState cs = caret->GetCaretState();

    uint old_rows = rows;
    uint old_columns = columns;
    std::vector<ElementPtr> cells;
    for (uint i = 1; i + 1 < elements->Count(); ++i)
        cells.push_back(elements->Get(i));
    ElementPtr right = elements->Get(elements->Count() - 1);

    //remove all children except the left bracket
    elements->RemoveAt(1, elements->Count() - 1);

    rows = _rows;
    columns = _columns;
    for (uint r = 0; r < rows; ++r)
    {
        for (uint c = 0; c < columns; ++c)
        {
            if (r < old_rows && c < old_columns)
                elements->Add(cells[r * old_columns + c]); //keep the surviving cell with its content
            else
                elements->Add(ElementPtr(new CodeRow<>(this))); //new empty cell with its placeholder
        }
    }
    elements->Add(right);

    UpdateLevel(level);

    //the renumbered ids invalidate the caret - keep it in the same (or the nearest surviving) cell
    if (cs.id == id || cs.IsInsideElement(id))
    {
        uint child_index = cs.id.size() > id.size() ? cs.id[id.size()] : 0;
        bool in_cell = child_index >= 1 && child_index + 1 < 2 + old_rows * old_columns;
        if (in_cell)
        {
            uint r = std::min((child_index - 1) / old_columns, rows - 1);
            uint c = std::min((child_index - 1) % old_columns, columns - 1);
            CaretState c_state;
            if (GetCell(r, c)->GetLastCaretState(c_state, nullptr))
                caret->SetState(c_state);
        }
        else
            caret->SetState(GetLeftShape()->id);
    }
}

uint Matrix::GetRows() const
{
    return rows;
}

uint Matrix::GetColumns() const
{
    return columns;
}

Shape* Matrix::GetLeftShape() const
{
    return (Shape*)elements->Get(0).get();
}

Shape* Matrix::GetRightShape() const
{
    return (Shape*)elements->Get(elements->Count() - 1).get();
}

CodeRow<>* Matrix::GetCell(const uint row, const uint column) const
{
    return (CodeRow<>*)elements->Get(1 + row * columns + column).get();
}

//Determinant

Determinant::Determinant(Element* _parent, bool with_init, uint _rows, uint _columns) :
    Matrix(_parent, with_init, _rows, _columns)
{
    type = ElementType::DETERMINANT;
    left_bracket_str = ToBasicString(std::u32string(1, left_bracket));
    right_bracket_str = ToBasicString(std::u32string(1, right_bracket));
}

Determinant::Determinant(Document* _document, bool with_init, uint _rows, uint _columns) :
    Matrix(_document, with_init, _rows, _columns)
{
    type = ElementType::DETERMINANT;
    left_bracket_str = ToBasicString(std::u32string(1, left_bracket));
    right_bracket_str = ToBasicString(std::u32string(1, right_bracket));
}

Determinant::Determinant(const Determinant& source) :
    Matrix(source)
{
}

Element* Determinant::Clone()
{
    return new Determinant(*this);
}

Element* Determinant::Create(Element* _parent)
{
    return new Determinant(_parent);
}

Element* Determinant::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    Determinant* el = parent ? new Determinant(parent, false) : new Determinant(document, false);
    if (value.HasMember("rows") && value["rows"].IsUint())
        el->rows = value["rows"].GetUint();
    if (value.HasMember("columns") && value["columns"].IsUint())
        el->columns = value["columns"].GetUint();
    el->rows = el->rows < 1 ? 1 : (el->rows > max_matrix_size ? max_matrix_size : el->rows);
    el->columns = el->columns < 1 ? 1 : (el->columns > max_matrix_size ? max_matrix_size : el->columns);
    return el;
}

std::u32string Determinant::BuildText() const
{
    return U"det(" + Matrix::BuildText() + U")";
}

std::string Determinant::ToHtml() const
{
    if (elements->Count() != 2 + rows * columns)
        return "";

    std::string open_fence = "<mo>|</mo>";
    std::string close_fence = "<mo>|</mo>";
    std::string s = open_fence + "<mtable>";
    for (uint r = 0; r < rows; ++r)
    {
        s += "<mtr>";
        for (uint c = 0; c < columns; ++c)
            s += "<mtd>" + GetCell(r, c)->ToHtml() + "</mtd>";
        s += "</mtr>";
    }
    s += "</mtable>" + close_fence;
    return s;
}

std::u32string Determinant::ToText() const
{
    return BuildText();
}

void Determinant::ToParserString(ParserString& str)
{
    str.Add(id, U"det(");
    BuildParserString(str);
    str.Add(id, U")");
}

}
