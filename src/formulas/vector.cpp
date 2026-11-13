/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "vector.h"
#include "document.h"
#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

//Vector

Vector::Vector(Element* _parent, bool with_init) :
    Formula(_parent)
{
    type = ElementType::VECTOR;
    if (with_init)
        Init();
}

Vector::Vector(Document* _document, bool with_init) :
    Formula(_document)
{
    type = ElementType::VECTOR;
    if (with_init)
        Init();
}

Vector::Vector(const Vector& source) :
    Formula(source),
    arrow_str(source.arrow_str)
{
}

Element* Vector::Clone()
{
    return new Vector(*this);
}

Element* Vector::Create(Element* _parent)
{
    return new Vector(_parent);
}

Element* Vector::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Vector(parent, false);
    return new Vector(document, false);
}

void Vector::Init()
{
    arrow_str = ToBasicString(std::u32string(1, arrow_symbol));
    elements->Add(ElementPtr(new Shape(this))); //the arrow above the row
    elements->Add(ElementPtr(new CodeRow<>(this, true))); //the vector content row

    UpdateLevel(level);
}

bool Vector::AfterFromJson()
{
    if (elements->Count() != 2)
        return false;
    if (elements->Get(0)->type != ElementType::SHAPE || elements->Get(1)->type != ElementType::CODE_ROW)
        return false;
    arrow_str = ToBasicString(std::u32string(1, arrow_symbol));
    return true;
}

void Vector::Draw() const
{
    GetShape()->draw_func =
        [&](const Rect& r)
        {
            //the arrow color is the text color of the formula
            Color c = document->selection.IsSelected(id) ? GetBackgroundColor() :
                (draw_string_format ? draw_string_format->text_color : Color::Black());
            int head_w = std::max(4, (int)std::round(5 * document->config.scale));
            int shaft_h = std::max(1, (int)std::round(document->config.scale));
            int mid = r.top + r.height / 2;
            window->DrawFillRect(r.left, mid - shaft_h / 2, std::max(1, r.width - head_w + 1), shaft_h, c);
            std::list<Point> path;
            path.push_back(Point{r.GetRight(), mid});
            path.push_back(Point{r.GetRight() - head_w, r.top});
            path.push_back(Point{r.GetRight() - head_w, r.GetBottom()});
            window->DrawFillPath(path, c);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Vector::Remake(bool with_elements)
{
    //the method is also called while Init builds the children - guard the incomplete element
    if (elements->Count() < 2)
        return Formula::Remake(with_elements);

    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);
    Shape* arrow = GetShape();
    CodeRow<>* row = GetRow();

    //the arrow band spans the row width above the row
    int arrow_h = std::max(3, (int)std::round(4 * document->config.scale));
    int gap = std::max(1, (int)std::round(document->config.scale));
    arrow->rect.SetRect(0, 0, row->rect.width, arrow_h);
    row->rect.Move(0, arrow_h + gap);
    baseline = arrow_h + gap + row->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Vector::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    //entering from the left first stops on the arrow, from there the row is entered
    caret_state.SetState(GetShape()->id);
    return true;
}

bool Vector::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    //entering from the right goes to the end of the content row - no stop on the arrow
    return GetRow()->GetLastCaretState(caret_state, select);
}

bool Vector::HasCaretState()
{
    //no own stop: entering from the left goes straight onto the arrow
    return false;
}

bool Vector::IsOnArrow(const CaretState& caret_state) const
{
    return caret_state.id == GetShape()->id;
}

bool Vector::IsInsideArrow(const CaretState& caret_state) const
{
    return caret_state.IsInsideElement(GetShape()->id);
}

bool Vector::CaretOnChild() const
{
    return caret->IsOnElement(GetShape()->id) || caret->IsOnElement(GetRow()->id) ||
        caret->IsInsideElement(GetShape()->id) || caret->IsInsideElement(GetRow()->id);
}

bool Vector::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetShape()->id) || caret->IsInsideElement(GetShape()->id)))
    {
        //from the arrow straight out of the element
        caret_state.SetState(id);
        return true;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetLeftCaretState(caret_state, select);
    if (res && select && caret_state.id == GetShape()->id)
    {
        //the arrow is never selected alone - the whole vector is selected as one element
        caret_state.SetState(id);
        select->Clear();
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return res;
    }
    if (res && !select && (IsOnArrow(caret_state) || IsInsideArrow(caret_state)))
    {
        caret_state.SetState(GetShape()->id);
        return true;
    }
    return res;
}

bool Vector::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetShape()->id) || caret->IsInsideElement(GetShape()->id)))
    {
        //from the arrow straight into the content row
        CaretState c;
        if (GetRow()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetRightCaretState(caret_state, select);
    if (res && select && caret_state.id == GetShape()->id)
    {
        //the arrow is never selected alone - the whole vector is selected as one element
        caret_state.SetState(id);
        select->Clear();
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return res;
    }
    if (res && !select && (IsOnArrow(caret_state) || IsInsideArrow(caret_state)))
    {
        CaretState c;
        if (GetRow()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }
    return res;
}

bool Vector::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetShape()->id) || caret->IsInsideElement(GetShape()->id)))
    {
        //from the arrow straight out of the element
        caret_state.SetState(id);
        return true;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetWordLeftCaretState(caret_state, select);
    if (res && select && caret_state.id == GetShape()->id)
    {
        caret_state.SetState(id);
        select->Clear();
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return res;
    }
    if (res && !select && (IsOnArrow(caret_state) || IsInsideArrow(caret_state)))
    {
        caret_state.SetState(GetShape()->id);
        return true;
    }
    return res;
}

bool Vector::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!select && (caret->IsOnElement(GetShape()->id) || caret->IsInsideElement(GetShape()->id)))
    {
        //from the arrow straight into the content row
        CaretState c;
        if (GetRow()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }

    if (select && CaretOnChild())
        return false;

    bool res = Formula::GetWordRightCaretState(caret_state, select);
    if (res && select && caret_state.id == GetShape()->id)
    {
        caret_state.SetState(id);
        select->Clear();
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return res;
    }
    if (res && !select && (IsOnArrow(caret_state) || IsInsideArrow(caret_state)))
    {
        CaretState c;
        if (GetRow()->GetFirstCaretState(c, select))
        {
            caret_state = c;
            return true;
        }
        return false;
    }
    return res;
}

bool Vector::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Vector::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select)
    {
        select->Add(id);
        caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
        return true;
    }

    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool Vector::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    //a click on the arrow lands on the shape itself - deleting from there dissolves the vector
    Rect arrow_rect = GetShape()->GetAbsoluteRect();
    if (arrow_rect.IsPointInside(x, y))
    {
        caret_state.SetState(GetShape()->id);
        return true;
    }
    return Formula::GetNearestCaretState(x, y, caret_state);
}

bool Vector::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (caret->GetPos() == 0 && caret->GetElement()->id == id)
        return false;
    return Formula::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

bool Vector::AfterInsert(bool with_undo)
{
    if (!caret)
        return false;

    if (!selection->IsEmpty())
    {
        ElementSelection& select = selection->selection[0];
        if (select.start > 0 && document->IsString(select.element))
            select.element->SplitAt(select.start);

        //move the selected elements into the content row
        GetRow()->elements->RemoveAt(0, 1);
        for (int i = selection->selection.size() - 1; i >= 0; --i)
        {
            ElementSelection el_s = selection->selection[i];
            for (int j = el_s.size - 1; j >= 0; --j)
                GetRow()->elements->Move(document->GetElement(GetChild(el_s.element->id, el_s.start + j)), 0);
        }

        selection->Clear();
        CaretState c;
        if (GetRow()->GetFirstCaretState(c, nullptr))
            caret->SetState(c);

        UpdateLevel(level);
        parent->parent->Normalize();
        return true;
    }

    CaretState c;
    if (GetRow()->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

bool Vector::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    //the caret on the arrow or on the element itself dissolves the vector:
    //the row content returns into the containing row, the wrapper disappears
    CaretState cs = caret->GetCaretState();
    if (cs.id == id || caret->IsOnElement(GetShape()->id))
    {
        if (with_undo)
            document->StoreUndo(parent->id);

        //return the row elements into the containing row in place of the vector
        int p = parent->elements->GetElementPos(id);
        uint count = GetRow()->elements->Count();

        caret->SetState(id);
        parent->elements->Move(*GetRow()->elements, p);
        CaretState c;
        if (parent->elements->Get(p)->GetFirstCaretState(c, nullptr))
            caret->SetState(c);

        auto t = parent->elements->Get(p + count); //for not removing this element until this function ends
        parent->elements->Remove(id);
        parent->Normalize();
        changed_element = parent->id;
        return true;
    }

    uint start, size;
    if (selection->Has(id, start, size))
    {
        //the content row is selected - clear it back to its placeholder
        if (start == 1 && size == 1)
        {
            if (with_undo)
                document->StoreUndo(id);
            GetRow()->elements->Clear();
            Normalize();
            caret->SetState(GetRow()->elements->Get(0)->id);
            selection->Clear();
            changed_element = id;
            return true;
        }
        return false;
    }

    //the caret inside the content row - the row handles the deletion itself
    return false;
}

bool Vector::UseSelection()
{
    return true;
}

std::string Vector::ToHtml() const
{
    if (elements->Count() < 2)
        return "";
    std::string s =
        "<mover accent='true'>"
            + GetRow()->ToHtml() +
            "<mo>" + arrow_str + "</mo>"
        "</mover>";
    return s;
}

std::u32string Vector::ToText() const
{
    if (elements->Count() < 2)
        return U"";
    return GetRow()->ToText();
}

void Vector::ToParserString(ParserString& str)
{
    //the vector is a notation only - the parser output is the row content
    if (elements->Count() < 2)
        return;
    GetRow()->ToParserString(str);
}

Shape* Vector::GetShape() const
{
    return (Shape*)elements->Get(0).get();
}

CodeRow<>* Vector::GetRow() const
{
    return (CodeRow<>*)elements->Get(1).get();
}

}
