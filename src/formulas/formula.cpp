/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "formula.h"
#include "../document.h"

namespace yutovo
{

//Formula

Formula::Formula(Element* _parent) : 
    Element(_parent)
{
    if (document && document->FindCodeBlock(parent->id) == 0)
        document->GetCurrentFormulaFormat(formula_format);
    else
        formula_format = GetFormulaFormat();
    has_caret_hilight = true;
}

Formula::Formula(Document* _document) :
    Element(_document)
{
    document->GetCurrentFormulaFormat(formula_format);
    has_caret_hilight = true;
}

bool Formula::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool Formula::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element)
{
    return false;
}

bool Formula::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    if (*formula_format->string_format == *format)
        return false;
    if (with_undo)
        document->StoreUndo(id);
    formula_format = document->formula_formats->GetFormat(formula_format->name, format, formula_format->inter_spacing, 
        formula_format->left_margin, formula_format->top_margin, formula_format->right_margin, formula_format->bottom_margin, 
        formula_format->color, formula_format->bg_color, formula_format->bg_selection_color);
    changed_element = id;
    document->CaretMoved();
    return true;
}

void Formula::Normalize()
{
    Element::Normalize();

    for (int i = 1; i < elements->Count();)
    {
        auto el1 = elements->Get(i - 1);
        auto el2 = elements->Get(i);
        if (el1->type == ElementType::CODE_ROW && el2->type == ElementType::CODE_ROW)
        {
            //merge the two rows
            if (!el1->Merge(el2))
                ++i;
        }
        else
            ++i;
    }
}

void Formula::Rescale() const
{
    Element::Rescale();
    draw_string_format = document->string_formats->GetFormat(formula_format->string_format, document->config.scale);
}

void Formula::UpdateRect(bool with_elements)
{
    Element::UpdateRect(with_elements);
    if (!draw_string_format)
        Rescale();
}

bool Formula::SplitAt(const uint pos)
{
    return false;
}

void Formula::MoveSelectionToRow(Element* target_row)
{
    //isolate the selected fragment of every partially selected string
    for (size_t i = 0; i < selection->selection.size(); ++i)
    {
        ElementSelection& s = selection->selection[i];
        if (document->IsString(s.element) && s.start > 0)
        {
            s.element->can_merge = true;
            s.element->SplitAt(s.start);
        }
    }
    for (size_t i = 0; i < selection->selection.size(); ++i)
    {
        ElementSelection& s = selection->selection[i];
        if (document->IsString(s.element) && s.size < (uint)s.element->elements->Count())
        {
            s.element->can_merge = true;
            s.element->SplitAt(s.size);
        }
    }

    auto move_row_elements =
        [&](Element* row_el)
        {
            for (int k = 0; k < row_el->elements->Count();)
            {
                if (row_el->elements->Get(k).get() == this)
                {
                    ++k; //the new formula itself stays in its row
                    continue;
                }
                target_row->elements->Move(row_el->elements->Get(k), target_row->elements->Count());
            }
        };

    //move the selected elements into the target row
    target_row->elements->RemoveAt(0, 1);
    for (int i = selection->selection.size() - 1; i >= 0; --i)
    {
        ElementSelection el_s = selection->selection[i];
        if (document->IsString(el_s.element))
        {
            //the entry covers its whole string after the isolation splits - move it once
            ElementPtr el = document->GetElement(el_s.element->id);
            if (el)
                target_row->elements->Move(el, 0);
            continue;
        }

        for (int j = el_s.size - 1; j >= 0; --j)
        {
            auto el = document->GetElement(GetChild(el_s.element->id, el_s.start + j));
            if (!el)
                continue; //stale index - the splits above shifted the following entries

            if (IsChild(el->id, id))
            {
                //the selection was optimized above the row of the formula: wrap the code rows inside it, never the element itself - 
                //moving a parent of the formula into its own row destroys the formula together with the whole subdocument
                std::vector<Element*> pending{el.get()};
                std::vector<Element*> rows;
                while (!pending.empty())
                {
                    Element* cur = pending.back();
                    pending.pop_back();
                    for (int c = 0; c < cur->elements->Count(); ++c)
                    {
                        Element* child = cur->elements->Get(c).get();
                        if (child->type == ElementType::CODE_ROW)
                            rows.push_back(child);
                        else if (child->type == ElementType::CODE_PARAGRAPH || child->type == ElementType::CODE_BLOCK || 
                            child->type == ElementType::TEXT_BLOCK)
                            pending.push_back(child);
                        //text rows, strings and formulas are never moved into a formula row
                    }
                }
                for (auto row_el : rows)
                    move_row_elements(row_el);
            }
            else if (el->type == ElementType::CODE_ROW)
                move_row_elements(el.get());
            else
                target_row->elements->Move(el, 0);
        }
    }
    if (target_row->elements->Count() == 0)
        target_row->Normalize(); //give the row its placeholder back when nothing was moved
}

bool Formula::Merge(const ElementPtr with_element)
{
    return false;
}

void Formula::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = std::round(formula_format->left_margin * document->config.scale);
    top = std::round(formula_format->top_margin * document->config.scale);
    right = std::round(formula_format->right_margin * document->config.scale);
    bottom = std::round(formula_format->bottom_margin * document->config.scale);
}

bool Formula::HasCaretState()
{
    return true;
}

bool Formula::HasLastCaretState()
{
    return true;
}

bool Formula::GetElementAtCoords(const int x, const int y, const int margin, ElementId& _id)
{
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (el->GetElementAtCoords(x, y, margin, _id))
            return true;
    }
    Rect r = parent->GetAbsoluteRect(GetCaretRect());
    if (r.IsPointInside(x, y))
    {
        _id = id;
        return true;
    }
    return false;
}

StringFormatPtr Formula::GetStringFormat() const
{
    return formula_format->string_format;
}

bool Formula::IsFormula()
{
    return true;
}

bool Formula::MatchDependency(const std::vector<std::string>& dependencies, const std::string& identifier)
{
    if (identifier.empty())
        return false;
    auto base =
        [](const std::string& name) -> std::string
        {
            auto pos = name.find('{');
            return pos == std::string::npos ? name : name.substr(0, pos);
        };
    std::string identifier_base = base(identifier);
    for (auto& d : dependencies)
    {
        if (base(d) == identifier_base)
            return true;
    }
    return false;
}

}
