/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "comparison.h"
#include "../document.h"

namespace yutovo
{

//Comparison

Comparison::Comparison(Element* _parent, const std::u32string& _sign, const std::u32string& _parser_sign, ElementType _type, bool with_init) :
    MiddleShapeFormula(_parent, with_init),
    sign(_sign),
    parser_sign(_parser_sign)
{
    type = _type;
    remake_always = true;
}

Comparison::Comparison(Document* _document, const std::u32string& _sign, const std::u32string& _parser_sign, ElementType _type, bool with_init) :
    MiddleShapeFormula(_document, with_init),
    sign(_sign),
    parser_sign(_parser_sign)
{
    type = _type;
    remake_always = true;
}

void Comparison::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    MiddleShapeFormula::ToJson(value, alloc);
}

bool Comparison::AfterFromJson()
{
    //the right part is a plain editable row, there is no result row to restore
    return MiddleShapeFormula::AfterFromJson();
}

void Comparison::Draw() const
{
    GetShape()->draw_func =
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
                window->DrawText(ToBasicString(sign), draw_string_format, r, GetBackgroundColor(), document->config.bg_selection_color, false);
            else
                window->DrawText(ToBasicString(sign), draw_string_format, r, document->config.shapes_color, GetBackgroundColor(), false);
        };

    MiddleShapeFormula::Draw();

    //if there are two or more formulas in a row, draw a frame for better view
    if (has_frame_hilight)
    {
        Rect r = GetAbsoluteRect();
        window->DrawRect(r.left, r.top, r.width, r.height, document->config.formula_frame_color);
    }
}

void Comparison::UpdateRect(bool with_elements)
{
    if (!draw_string_format)
        Rescale();
    Size s = parent->window->GetTextSize(sign, draw_string_format);
    GetShape()->rect.SetSize(s.width, s.height * 3 / 4);
    GetShape()->baseline = GetShape()->rect.height / 3 * 2;

    MiddleShapeFormula::UpdateRect(false);
}

bool Comparison::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);
    if (!with_elements)
        GetLast()->Remake(true);

    UpdateRect();

    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    GetFirst()->rect.Move(0, baseline - GetFirst()->baseline);
    GetShape()->rect.Move(GetFirst()->rect.width, baseline - GetShape()->baseline);
    GetLast()->rect.Move(GetFirst()->rect.width + GetShape()->rect.width, baseline - GetLast()->baseline);

    UpdateRect();

    has_frame_hilight = (parent->type == ElementType::CODE_ROW && parent->parent->type == ElementType::CODE_PARAGRAPH && 
        parent->elements->Count() > 1);

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Comparison::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        GetFirst()->elements->Clear();
    for (int i = pos - 1; i >= 0; --i)
    {
        auto el = parent->elements->Get(i);
        //a statement element ends the wrapped part only at the top level of the row
        if ((el->type == ElementType::ASSIGNMENT || el->type == ElementType::EQUATION ||
            el->type == ElementType::TEXT_ASSIGNMENT || el->type == ElementType::TEXT_EQUATION || IsComparison(el->type)) &&
            BracketDepth(parent->elements.get(), i + 1) == 0)
            break;
        //the wrapped left part is bracket balanced: an unclosed bracket of an enclosing call (if(, for(, a function, an array) ends the wrap
        if (BracketDepth(parent->elements.get(), pos) - BracketDepth(parent->elements.get(), i) > 0)
            break;
        //a comma of the same argument list ends the wrapped part: the operand does not cross it
        if (el->type == ElementType::COMMA &&
            BracketDepth(parent->elements.get(), i + 1) == BracketDepth(parent->elements.get(), pos))
            break;
        GetFirst()->elements->Move(el, 0);
    }

    //the right part is editable - put the caret into it, typing on the shape is not allowed
    CaretState c;
    GetLast()->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    return true;
}

std::string Comparison::ToHtml() const
{
    std::string s = GetFirst()->ToHtml();
    s += "<mo>" + EscapeHtml(ToBasicString(sign)) + "</mo>";
    if (GetLast())
        s += GetLast()->ToHtml();
    return s;
}

std::u32string Comparison::ToText() const
{
    std::u32string s;
    if (elements->Count() > 0)
        s = elements->Get(0)->ToText();
    s += sign;
    if (elements->Count() == 3)
        s += elements->Get(2)->ToText();
    return s;
}

void Comparison::ToParserString(ParserString& str)
{
    //an incomplete comparison changes nothing while editing
    ParserString left;
    GetFirst()->ToParserString(left);
    ParserString right;
    GetLast()->ToParserString(right);
    if (left.Text().empty() || right.Text().empty())
        return;

    //the calculator parses a comparison only in parentheses
    int start = str.Length();
    str.Add(id, U"(");
    str.Add(left);
    str.Add(id, parser_sign);
    str.Add(right);
    str.Add(id, U")");
    str.Annotate(id, start, str.Length() - start);
}

//Equal

Equal::Equal(Element* _parent, bool with_init) :
    Comparison(_parent, U"=", U"==", ElementType::EQUAL, with_init)
{
}

Equal::Equal(Document* _document, bool with_init) :
    Comparison(_document, U"=", U"==", ElementType::EQUAL, with_init)
{
}

Element* Equal::Clone()
{
    return new Equal(*this);
}

Element* Equal::Create(Element* _parent)
{
    return new Equal(_parent);
}

void Equal::Rescale() const
{
    Formula::Rescale();
    //the equal comparison sign is drawn bolder than the solving equation sign
    auto f = draw_string_format;
    draw_string_format = document->GetStringFormat(f->family, f->size, true, f->italic, f->underline, f->strikethrough,
        f->subscript, f->superscript, f->text_color, f->text_bg_color, f->text_bg_selection_color);
}

Element* Equal::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Equal(parent, false);
    return new Equal(document, false);
}

//Greater

Greater::Greater(Element* _parent, bool with_init) :
    Comparison(_parent, U">", U">", ElementType::GREATER, with_init)
{
}

Greater::Greater(Document* _document, bool with_init) :
    Comparison(_document, U">", U">", ElementType::GREATER, with_init)
{
}

Element* Greater::Clone()
{
    return new Greater(*this);
}

Element* Greater::Create(Element* _parent)
{
    return new Greater(_parent);
}

Element* Greater::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Greater(parent, false);
    return new Greater(document, false);
}

//Less

Less::Less(Element* _parent, bool with_init) :
    Comparison(_parent, U"<", U"<", ElementType::LESS, with_init)
{
}

Less::Less(Document* _document, bool with_init) :
    Comparison(_document, U"<", U"<", ElementType::LESS, with_init)
{
}

Element* Less::Clone()
{
    return new Less(*this);
}

Element* Less::Create(Element* _parent)
{
    return new Less(_parent);
}

Element* Less::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Less(parent, false);
    return new Less(document, false);
}

//GreaterEqual

GreaterEqual::GreaterEqual(Element* _parent, bool with_init) :
    Comparison(_parent, U"\u2A7E", U">=", ElementType::GREATER_EQUAL, with_init)
{
}

GreaterEqual::GreaterEqual(Document* _document, bool with_init) :
    Comparison(_document, U"\u2A7E", U">=", ElementType::GREATER_EQUAL, with_init)
{
}

Element* GreaterEqual::Clone()
{
    return new GreaterEqual(*this);
}

Element* GreaterEqual::Create(Element* _parent)
{
    return new GreaterEqual(_parent);
}

Element* GreaterEqual::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new GreaterEqual(parent, false);
    return new GreaterEqual(document, false);
}

//LessEqual

LessEqual::LessEqual(Element* _parent, bool with_init) :
    Comparison(_parent, U"\u2A7D", U"<=", ElementType::LESS_EQUAL, with_init)
{
}

LessEqual::LessEqual(Document* _document, bool with_init) :
    Comparison(_document, U"\u2A7D", U"<=", ElementType::LESS_EQUAL, with_init)
{
}

Element* LessEqual::Clone()
{
    return new LessEqual(*this);
}

Element* LessEqual::Create(Element* _parent)
{
    return new LessEqual(_parent);
}

Element* LessEqual::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new LessEqual(parent, false);
    return new LessEqual(document, false);
}

//NotEqual

NotEqual::NotEqual(Element* _parent, bool with_init) :
    Comparison(_parent, U"\u2260", U"<>", ElementType::NOT_EQUAL, with_init)
{
}

NotEqual::NotEqual(Document* _document, bool with_init) :
    Comparison(_document, U"\u2260", U"<>", ElementType::NOT_EQUAL, with_init)
{
}

Element* NotEqual::Clone()
{
    return new NotEqual(*this);
}

Element* NotEqual::Create(Element* _parent)
{
    return new NotEqual(_parent);
}

Element* NotEqual::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
    rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new NotEqual(parent, false);
    return new NotEqual(document, false);
}

}
