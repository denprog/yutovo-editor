/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "division.h"
#include "code_row.h"
#include "shape.h"

namespace yutovo
{

//Division

Division::Division(Element* _parent, bool with_init) : 
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::DIVISION;
}

Division::Division(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::DIVISION;
}

Division::Division(const Division& source) :
    MiddleShapeFormula(source)
{
}

Element* Division::Clone()
{
    return new Division(*this);
}

Element* Division::Create(Element* parent)
{
    return new Division(parent);
}

Element* Division::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Division(parent, false);
    return new Division(document, false);
}

void Division::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color;
            if (r.height == 0)
                window->DrawLine(r.left, r.top, r.left + r.width, r.top, c);
            else
                window->DrawFillRect(r.left, r.top, r.width, r.height, c);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    MiddleShapeFormula::Draw();
}

bool Division::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);
    
    int w = std::max(GetFirst()->rect.width + 2, GetLast()->rect.width + 2);
    if (w < 200)
        GetShape()->rect.SetRect(0, 0, w, 0);
    else
        GetShape()->rect.SetRect(0, 0, w, w / 200 > 2 ? 2 : w / 200);
    GetFirst()->rect.Move((w - GetFirst()->rect.width) / 2, 0);
    GetShape()->rect.Move(0, GetFirst()->rect.height + GetShape()->rect.height + 4);
    GetLast()->rect.Move((w - GetLast()->rect.width) / 2, GetFirst()->rect.height + GetShape()->rect.height + (GetShape()->rect.height + 4) * 2);
    baseline = GetShape()->rect.GetBottom() - GetShape()->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Division::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetShape()->id))
            return false;
        
        CaretState c;
        if ((GetLast()->GetFirstCaretState(c, nullptr) && caret_state == c) || (GetFirst()->GetFirstCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(id);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetLeftCaretState(caret_state, select);
}

bool Division::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        if (caret->IsOnElement(GetShape()->id))
            return false;
        
        CaretState c;
        if ((GetLast()->GetLastCaretState(c, nullptr) && caret_state == c) || (GetFirst()->GetLastCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetRightCaretState(caret_state, select);
}

bool Division::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (GetLast()->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(GetLast()->id))
        return GetLast()->GetTopCaretState(x, y, caret_state, select);
    if (GetShape()->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(GetShape()->id))
        return GetShape()->GetTopCaretState(x, y, caret_state, select);
    if (GetFirst()->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(GetFirst()->id))
        return GetFirst()->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Division::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (GetFirst()->GetAbsoluteRect().top >= y)
        return GetFirst()->GetBottomCaretState(x, y, caret_state, select);
    if (GetShape()->GetAbsoluteRect().top >= y)
        return GetShape()->GetBottomCaretState(x, y, caret_state, select);
    if (GetLast()->GetAbsoluteRect().top >= y)
        return GetLast()->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool Division::UseSelection()
{
    return true;
}

std::string Division::ToHtml() const
{
    std::string s = "<mfrac>";
    s += GetFirst()->ToHtml();
    s += GetLast()->ToHtml();
    s += "</mfrac>";
    return s;
}

std::u32string Division::ToText() const
{
    if (!GetFirst() || !GetLast())
        return U"";
    return U"(" + GetFirst()->ToText() + U")/(" + GetLast()->ToText() + U")";
}

void Division::ToParserString(ParserString& str)
{
    str.Add(id, U"(");
    GetFirst()->ToParserString(str);
    if (GetFirst()->elements->Count() == 1 && document->IsString(GetFirst()->elements->Get(0)->id) && 
        GetLast()->elements->Count() == 1 && document->IsString(GetLast()->elements->Get(0)->id) && 
        GetFirst()->elements->Get(0)->ToText().find_first_not_of(U"0123456789.") == std::string::npos && 
        GetLast()->elements->Get(0)->ToText().find_first_not_of(U"0123456789.") == std::string::npos)
    {
        str.Add(id, U"/");
    }
    else
    {
        str.Add(id, U")/(");
    }
    GetLast()->ToParserString(str);
    str.Add(id, U")");
}

void Division::AddNumerator(ElementPtr numerator)
{
    if (GetFirst()->IsEmpty())
        GetFirst()->elements->Clear();
    GetFirst()->elements->Add(numerator);
}

void Division::AddDenomerator(ElementPtr denomerator)
{
    if (GetLast()->IsEmpty())
        GetLast()->elements->Clear();
    GetLast()->elements->Add(denomerator);
}

}
