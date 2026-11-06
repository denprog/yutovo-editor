/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "limit.h"
#include "document.h"
#include "code_row.h"
#include "code_string.h"
#include "power.h"

namespace yutovo
{

//Limit

const std::u32string Limit::lim_text = U"lim";

Limit::Limit(Element* _parent, bool with_init) :
    Formula(_parent)
{
    type = ElementType::LIMIT;
    if (with_init)
        Init();
}

Limit::Limit(Document* _document, bool with_init) :
    Formula(_document)
{
    type = ElementType::LIMIT;
    if (with_init)
        Init();
}

Limit::Limit(const Limit& source) :
    Formula(source),
    arrow_str(source.arrow_str)
{
}

Element* Limit::Clone()
{
    return new Limit(*this);
}

Element* Limit::Create(Element* _parent)
{
    return new Limit(_parent);
}

Element* Limit::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Limit(parent, false);
    return new Limit(document, false);
}

void Limit::Init()
{
    arrow_str = ToBasicString(std::u32string(1, arrow_symbol));
    ElementPtr lim(new CodeString(this, lim_text)); //the "lim" sign
    lim->editable = false;
    lim->can_merge = false;
    elements->Add(lim);
    elements->Add(ElementPtr(new CodeRow<>(this, true))); //limit variable
    ElementPtr arrow(new CodeString(this, std::u32string(1, arrow_symbol))); //tends-to arrow
    arrow->editable = false;
    arrow->can_merge = false;
    elements->Add(arrow);
    elements->Add(ElementPtr(new CodeRow<>(this, true))); //the value the variable tends to

    UpdateLevel(level);
}

bool Limit::AfterFromJson()
{
    if (elements->Count() != 4)
        return false;
    if (elements->Get(0)->type != ElementType::CODE_STRING || elements->Get(1)->type != ElementType::CODE_ROW ||
        elements->Get(2)->type != ElementType::CODE_STRING || elements->Get(3)->type != ElementType::CODE_ROW)
        return false;
    //the editable flag is not serialized - restore it on the lim text and the arrow
    elements->Get(0)->editable = false;
    elements->Get(0)->can_merge = false;
    elements->Get(2)->editable = false;
    elements->Get(2)->can_merge = false;
    arrow_str = ToBasicString(std::u32string(1, arrow_symbol));
    return true;
}

void Limit::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Limit::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);
    CodeString* lim = GetLim();
    CodeRow<>* variable = GetVariable();
    CodeString* arrow = GetArrow();
    CodeRow<>* value = GetValue();

    //the "variable → value" row is centered under the lim text, its parts are aligned by the baselines
    int gap = std::max(2, (int)std::round(2 * document->config.scale));
    int row_width = variable->rect.width + gap + arrow->rect.width + gap + value->rect.width;
    int width = std::max(lim->rect.width, row_width);

    if (lim->rect.width < width)
        lim->rect.Move((width - lim->rect.width) / 2, 0);
    else
        lim->rect.Move(0, 0);

    int row_top = lim->rect.height + gap;
    int row_baseline = std::max(variable->baseline, std::max(arrow->baseline, value->baseline));
    int x = std::max(0, (width - row_width) / 2);
    variable->rect.Move(x, row_top + row_baseline - variable->baseline);
    arrow->rect.Move(x + variable->rect.width + gap, row_top + row_baseline - arrow->baseline);
    value->rect.Move(x + variable->rect.width + gap + arrow->rect.width + gap, row_top + row_baseline - value->baseline);

    baseline = lim->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Limit::Normalize()
{
    Element::Normalize(); //skip Formula::Normalize()

    if (elements->Count() > 4)
        elements->Get(3)->Merge(elements->Get(4));
}

bool Limit::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    //the lim text and the arrow are not caret stops - entering the limit starts in the variable row
    return GetVariable()->GetFirstCaretState(caret_state, select);
}

bool Limit::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    //entering from the right goes to the end of the value row - no stop on the lim text or the arrow
    return GetValue()->GetLastCaretState(caret_state, select);
}

bool Limit::IsOnSign(const CaretState& caret_state) const
{
    return caret_state.id == GetLim()->id || caret_state.id == GetArrow()->id;
}

bool Limit::IsInsideSign(const CaretState& caret_state) const
{
    return caret_state.IsInsideElement(GetLim()->id) || caret_state.IsInsideElement(GetArrow()->id);
}

bool Limit::CaretOnChild() const
{
    return caret->IsOnElement(GetLim()->id) || caret->IsOnElement(GetVariable()->id) || caret->IsOnElement(GetArrow()->id) || 
        caret->IsOnElement(GetValue()->id) || caret->IsInsideElement(GetLim()->id) || caret->IsInsideElement(GetVariable()->id) || 
        caret->IsInsideElement(GetArrow()->id) || caret->IsInsideElement(GetValue()->id);
}

bool Limit::SkipRight(CaretState& caret_state, Selection* select) const
{
    //a landing on the arrow goes to the value row, a landing on the lim text - to the variable row
    CaretState c;
    if (caret_state.id == GetArrow()->id || caret_state.IsInsideElement(GetArrow()->id))
    {
        if (GetValue()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }
    if (GetVariable()->GetFirstCaretState(c, select))
    {
        caret_state = c;
        return true;
    }
    return false;
}

bool Limit::SkipLeft(CaretState& caret_state, Selection* select) const
{
    //a landing on the arrow goes to the end of the variable row, a landing on the lim text - out of the element
    CaretState c;
    if (caret_state.id == GetArrow()->id || caret_state.IsInsideElement(GetArrow()->id))
    {
        if (GetVariable()->GetLastCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }
    //the single stop before the lim text is the limit itself
    caret_state.SetState(id);
    return true;
}

bool Limit::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetLim()->id) || caret->IsInsideElement(GetLim()->id)))
    {
        //from the lim text straight out of the element
        caret_state.SetState(id);
        return true;
    }
    if (!select && (caret->IsOnElement(GetArrow()->id) || caret->IsInsideElement(GetArrow()->id)))
    {
        CaretState c;
        if (GetVariable()->GetLastCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetLeftCaretState(caret_state, select);
    if (!select && res && (IsOnSign(caret_state) || IsInsideSign(caret_state)))
        return SkipLeft(caret_state, select);
    return res;
}

bool Limit::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetLim()->id) || caret->IsInsideElement(GetLim()->id)))
    {
        //from the lim text straight into the variable row
        CaretState c;
        if (GetVariable()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }
    if (!select && (caret->IsOnElement(GetArrow()->id) || caret->IsInsideElement(GetArrow()->id)))
    {
        //from the arrow straight into the value row
        CaretState c;
        if (GetValue()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetRightCaretState(caret_state, select);
    if (!select && res && (IsOnSign(caret_state) || IsInsideSign(caret_state)))
        return SkipRight(caret_state, select);
    return res;
}

bool Limit::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetLim()->id) || caret->IsInsideElement(GetLim()->id)))
    {
        //from the lim text straight out of the element
        caret_state.SetState(id);
        return true;
    }
    if (!select && (caret->IsOnElement(GetArrow()->id) || caret->IsInsideElement(GetArrow()->id)))
    {
        CaretState c;
        if (GetVariable()->GetLastCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetWordLeftCaretState(caret_state, select);
    if (!select && res && (IsOnSign(caret_state) || IsInsideSign(caret_state)))
        return SkipLeft(caret_state, select);
    return res;
}

bool Limit::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetLim()->id) || caret->IsInsideElement(GetLim()->id)))
    {
        //from the lim text straight into the variable row
        CaretState c;
        if (GetVariable()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }
    if (!select && (caret->IsOnElement(GetArrow()->id) || caret->IsInsideElement(GetArrow()->id)))
    {
        //from the arrow straight into the value row
        CaretState c;
        if (GetValue()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetWordRightCaretState(caret_state, select);
    if (!select && res && (IsOnSign(caret_state) || IsInsideSign(caret_state)))
        return SkipRight(caret_state, select);
    return res;
}

CodeRow<>* Limit::GetNearestRow(const int x) const
{
    //the variable and the value rows share the band under the lim text - vertical moves enter the closer row
    CodeRow<>* variable = GetVariable();
    CodeRow<>* value = GetValue();
    Rect var_rect = variable->GetAbsoluteRect();
    Rect value_rect = value->GetAbsoluteRect();
    int var_dist = 0;
    int value_dist = 0;
    if (x < var_rect.left)
        var_dist = var_rect.left - x;
    else if (x > var_rect.GetRight())
        var_dist = x - var_rect.GetRight();
    if (x < value_rect.left)
        value_dist = value_rect.left - x;
    else if (x > value_rect.GetRight())
        value_dist = x - value_rect.GetRight();
    return value_dist < var_dist ? value : variable;
}

bool Limit::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    CodeRow<>* row = GetNearestRow(x);
    if (row->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(row->id))
        return row->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Limit::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    CodeRow<>* row = GetNearestRow(x);
    if (row->GetAbsoluteRect().top >= y)
        return row->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool Limit::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    //the caret on the lim text, on the arrow or on the element itself (the signs are not caret stops,
    //such a state carries the limit id) deletes the whole element
    CaretState cs = caret->GetCaretState();
    if (cs.id == id || caret->IsOnElement(GetLim()->id) || caret->IsOnElement(GetArrow()->id))
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

    if (caret->IsOnElement(GetVariable()->id) || caret->IsOnElement(GetValue()->id)) //don't delete these rows
        return false;

    uint start, size;
    if (selection->Has(id, start, size))
    {
        CodeRow<>* row = nullptr;
        if (start == 1 && size == 1)
            row = GetVariable();
        else if (start == 3 && size == 1)
            row = GetValue();
        if (row)
        {
            if (with_undo)
                document->StoreUndo(row->id);
            row->elements->Clear();
            Normalize();
            selection->Remove(id, start, size);
            changed_element = id;
            return true;
        }
        return false;
    }

    return false;
}

void Limit::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    GetVariable()->UpdateLevel(_level + 1);
    GetArrow()->UpdateLevel(_level + 1);
    GetValue()->UpdateLevel(_level + 1);
}

bool Limit::AfterInsert(bool with_undo)
{
    if (!caret)
        return false;
    CaretState c;
    if (GetVariable()->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

std::string Limit::ToHtml() const
{
    std::string s =
        "<munder>"
            "<mi>lim</mi>"
            "<mrow>"
                + GetVariable()->ToHtml() +
                "<mo>" + arrow_str + "</mo>"
                + GetValue()->ToHtml() +
            "</mrow>"
        "</munder>";
    return s;
}

std::u32string Limit::ToText() const
{
    CodeRow<>* variable = GetVariable();
    CodeRow<>* value = GetValue();
    if (!variable || !value)
        return U"";
    return U"lim[" + variable->ToText() + std::u32string(1, arrow_symbol) + value->ToText() + U"]";
}

void Limit::ToParserString(ParserString& str)
{
    //the parser output is handled by the containing row - the limited function stands after the element
}

bool Limit::IsFilled() const
{
    if (GetVariable()->ToText().empty())
        return false;
    if (GetDirection() != 0)
    {
        Power* power = (Power*)GetValue()->elements->Get(GetValue()->elements->Count() - 1).get();
        return !power->GetBaseRow()->ToText().empty();
    }
    return !GetValue()->ToText().empty();
}

int Limit::GetDirection() const
{
    CodeRow<>* value = GetValue();
    if (value->elements->Count() == 0)
        return 0;
    Element* last = value->elements->Get(value->elements->Count() - 1).get();
    if (last->type != ElementType::POWER)
        return 0;
    Power* power = (Power*)last;
    std::u32string exponent = power->GetExponentRow()->ToText();
    if (exponent == U"+")
        return 1;
    if (exponent == U"-")
        return -1;
    return 0;
}

void Limit::AddParserArguments(ParserString& str) const
{
    str.Add(id, U",");
    GetVariable()->ToParserString(str);
    str.Add(id, U",");
    //a trailing power with the exponent "+" or "-" is the approach direction - only its base is the value
    int direction = GetDirection();
    CodeRow<>* value = GetValue();
    int last = value->elements->Count() - (direction != 0 ? 1 : 0);
    for (int i = 0; i < last; ++i)
        value->elements->Get(i)->ToParserString(str);
    if (direction != 0)
    {
        Power* power = (Power*)value->elements->Get(value->elements->Count() - 1).get();
        power->GetBaseRow()->ToParserString(str);
    }
    if (direction > 0)
        str.Add(id, U",1)");
    else if (direction < 0)
        str.Add(id, U",-1)");
    else
        str.Add(id, U")");
}

CodeString* Limit::GetLim() const
{
    return (CodeString*)elements->Get(0).get();
}

CodeRow<>* Limit::GetVariable() const
{
    return (CodeRow<>*)elements->Get(1).get();
}

CodeString* Limit::GetArrow() const
{
    return (CodeString*)elements->Get(2).get();
}

CodeRow<>* Limit::GetValue() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

}
