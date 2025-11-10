/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
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
    if (document)
        document->GetCurrentFormulaFormat(formula_format);
    else
        formula_format = GetFormulaFormat();
}

Formula::Formula(Document* _document) :
    Element(_document)
{
    document->GetCurrentFormulaFormat(formula_format);
}

bool Formula::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    return parent->InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool Formula::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, ElementId& changed_element)
{
    return false;
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

bool Formula::SplitAt(const uint pos)
{
    return false;
}

bool Formula::Merge(const ElementPtr with_element)
{
    return false;
}

void Formula::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = formula_format->left_margin;
    top = formula_format->top_margin;
    right = formula_format->right_margin;
    bottom = formula_format->bottom_margin;
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

}
