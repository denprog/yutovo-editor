/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "text_block.h"
#include "code_paragraph.h"
#include "code_string.h"
#include "text_equation.h"
#include "text_assignment.h"
#include "result.h"
#include "formula.h"
#include "../document.h"

namespace yutovo
{

//TextBlock

TextBlock::TextBlock(Document* _document, bool add_empty) :
    Block(_document)
{
    type = ElementType::TEXT_BLOCK;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //text block has to have at least one code paragraph

    document->SetLocale(document->config.language, false);
}

TextBlock::TextBlock(Element* parent, bool add_empty) :
    Block(parent)
{
    type = ElementType::TEXT_BLOCK;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code", document->config.language);
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //text block has to have at least one code paragraph

    document->SetLocale(document->config.language, false);
}

Element* TextBlock::Clone()
{
    return new TextBlock(*this);
}

Element* TextBlock::Create(Element* parent)
{
    return new TextBlock(parent);
}

bool TextBlock::InsertElements(std::vector<ElementPtr>& _elements, bool insert_mode, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(id) && document->pasting)
    {
        std::vector<ElementPtr> _els;
        for (auto& el : _elements)
        {
            if (document->IsRow(el))
            {
                ElementPtr p(new Paragraph(parent, false));
                p->elements->Add(el);
                _els.push_back(p);
            }
            else
                _els.push_back(el);
        }
        return parent->InsertElements(_els, insert_mode, with_undo, changed_element);
    }
    return Block::InsertElements(_elements, insert_mode, with_undo, changed_element);
}

void TextBlock::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Block::ToJson(value, alloc);
}

Element* TextBlock::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new TextBlock(parent, false);
    return new TextBlock(document, false);
}

bool TextBlock::AfterFromJson()
{
    //documents saved by older versions may keep plain strings inside a text block
    ConvertStringsToCode(this);
    return Block::AfterFromJson();
}

void TextBlock::CollectRowElements(Element* el, std::vector<ElementPtr>& out)
{
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        auto child = el->elements->Get(i);
        if (child->type == ElementType::CODE_ROW || child->type == ElementType::CODE_COLUMN || dynamic_cast<ResultRow*>(child.get()))
            CollectRowElements(child.get(), out); //results wrap their content into nested rows
        else
            out.push_back(child);
    }
}

void TextBlock::ConvertStringsToCode(Element* el)
{
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        auto child = el->elements->Get(i);
        if (child->type == ElementType::STRING)
        {
            ElementPtr code(new CodeString(*(String*)child.get()));
            el->elements->Replace(code, i);
        }
        else if (child->type != ElementType::CODE_STRING && child->type != ElementType::LINK)
            ConvertStringsToCode(child.get()); //string and link elements keep their own element lists
    }
}

void TextBlock::Draw() const
{
    window->DrawFillRect(GetAbsoluteRect(), document->config.text_block.background_color);
    if (document->config.code_block_border)
        window->DrawRect(GetAbsoluteRect(), document->config.text_block.frame_color);
    Element::Draw();
}

bool TextBlock::Remake(bool with_elements)
{
    bool changed = Element::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = code_format->top_indent;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(left_m + code_format->left_indent, h + top_m);
        h = p->rect.GetBottom() + bottom_m;
    }

    Element::UpdateRect(false);

    UpdateRect();
    rect.width += code_format->right_indent;
    rect.height += code_format->bottom_indent;

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    UpdateDrawRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void TextBlock::UpdateDrawRect()
{
    Block::UpdateDrawRect();
    draw_rect.width += 2;
    draw_rect.height += 2;
}

Color TextBlock::GetBackgroundColor() const
{
    return document->config.text_block.background_color;
}

bool TextBlock::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (select && select->IsSelected(id))
        caret->SetState(id);
    return Block::GetTopCaretState(x, y, caret_state, select);
}

bool TextBlock::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetLeftCaretState(caret_state, select))
            return true;
        if (select)
        {
            ElementSelection s;
            if (!selection->Has(id, s) || s.size != elements->Count())
                select->Add(id);
            caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
            return true;
        }
    }
    if (parent)
        return parent->GetLeftCaretState(caret_state, select);
    return false;
}

bool TextBlock::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (!IsVisible())
        return false;
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetRightCaretState(caret_state, select))
            return true;
        if (select)
        {
            ElementSelection s;
            if (!selection->Has(id, s) || s.size != elements->Count())
                select->Add(id);
            caret_state.SetState(parent->id, parent->elements->GetElementPos(id) + 1, true);
            return true;
        }
    }
    if (parent)
        return parent->GetRightCaretState(caret_state, select);
    return false;
}

bool TextBlock::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (caret_state.IsInsideElement(id))
    {
        if (caret_state.GetPos() == 0)
        {
            if (select)
                select->Add(id);
            caret_state.SetState(parent->id, parent->elements->GetElementPos(id));
            return true;
        }
    }
    return Block::GetWordLeftCaretState(caret_state, select);
}

bool TextBlock::AfterInsert(bool with_undo)
{
    CaretState c;
    if (GetFirstCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

void TextBlock::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = std::round(code_format->left_margin * document->config.scale);
    top = std::round(code_format->top_margin * document->config.scale);
    right = std::round(code_format->right_margin * document->config.scale);
    bottom = std::round(code_format->bottom_margin * document->config.scale);
}

bool TextBlock::HasCaretState()
{
    return true;
}

bool TextBlock::HasLastCaretState()
{
    return true;
}

bool TextBlock::CanContinueVerticalMoving()
{
    return true;
}

StringFormatPtr TextBlock::GetStringFormat() const
{
    return formula_format->string_format;
}

FormulaFormatPtr TextBlock::GetFormulaFormat() const
{
    return formula_format;
}

ParagraphFormatPtr TextBlock::GetParagraphFormat()
{
    return paragraph_format;
}

std::string TextBlock::ToHtml() const
{
    if (elements->Count() <= 1)
        return Block::ToHtml();

    std::string html = "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">";
    for (int i = 0; i < elements->Count(); ++i)
    {
        const auto& el = elements->Get(i);
        if (!el->IsVisible())
            continue;
        html += el->ToHtml();
        if (i < elements->Count() - 1)
            html += "<br>";
    }
    html += "</span>";
    return html;
}

void TextBlock::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeParagraph<>(this, true)));
}

bool TextBlock::IsFormula()
{
    return true;
}

void TextBlock::ConvertToText(std::vector<ElementPtr>& _elements)
{
    for (auto& el : _elements)
        el = ConvertElementToText(el);
}

ElementPtr TextBlock::ConvertElementToText(ElementPtr el)
{
    if (el->type == ElementType::EQUATION || el->type == ElementType::ASSIGNMENT)
    {
        ElementPtr text(el->type == ElementType::EQUATION ? (Element*)new TextEquation(el->document) : (Element*)new TextAssignment(el->document));
        text->level = el->level;
        text->editable = el->editable;
        text->can_merge = el->can_merge;
        text->can_move_picture = el->can_move_picture;
        text->can_resize = el->can_resize;
        text->visible = el->visible;
        text->has_caret_hilight = el->has_caret_hilight;
        text->has_frame_hilight = el->has_frame_hilight;
        text->elements->Get(0)->elements->ReplaceAll(*el->elements->Get(0)->elements);
        if (el->elements->Count() == 3)
        {
            //the right part is copied as is; a solved result row is flattened into a plain editable row
            auto right = text->elements->Get(2).get();
            right->elements->Clear();
            auto last = el->elements->Get(2).get();
            for (int i = 0; i < last->elements->Count(); ++i)
            {
                auto child = last->elements->Get(i);
                if (dynamic_cast<ResultRow*>(child.get()))
                {
                    std::vector<ElementPtr> collected;
                    CollectRowElements(child.get(), collected);
                    for (auto& e : collected)
                        right->elements->Add(e);
                }
                else
                    right->elements->Add(child);
            }
        }
        return text;
    }
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        auto child = el->elements->Get(i);
        ElementPtr converted = ConvertElementToText(child);
        if (converted != child)
            el->elements->Replace(converted, i);
    }
    return el;
}

}
