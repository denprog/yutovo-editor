/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "evalution_bar.h"
#include "code_paragraphs_block.h"
#include "code_paragraph.h"
#include "code_row.h"
#include "code_string.h"
#include "str.h"

namespace yutovo
{

//EvalutionBarSubscript

const std::string EvalutionBarSubscript::family_name = "DejaVu Serif";

EvalutionBarSubscript::EvalutionBarSubscript(Element* _parent, bool with_init) :
    Formula(_parent)
{
    type = ElementType::EVALUTION_BAR_SUBSCRIPT;
    editable = false;
    remake_always = true;
    if (with_init)
        Init();
}

EvalutionBarSubscript::EvalutionBarSubscript(Document* _document, bool with_init) :
    Formula(_document)
{
    type = ElementType::EVALUTION_BAR_SUBSCRIPT;
    editable = false;
    remake_always = true;
    if (with_init)
        Init();
}

EvalutionBarSubscript::EvalutionBarSubscript(const EvalutionBarSubscript& source) :
    Formula(source),
    symbol_str(source.symbol_str)
{
    editable = false;
    remake_always = true;
}

Element* EvalutionBarSubscript::Clone()
{
    return new EvalutionBarSubscript(*this);
}

Element* EvalutionBarSubscript::Create(Element* _parent)
{
    return new EvalutionBarSubscript(_parent);
}

Element* EvalutionBarSubscript::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, 
    rapidjson::Document::AllocatorType& alloc)
{
    EvalutionBarSubscript* p = nullptr;
    if (parent)
        p = new EvalutionBarSubscript(parent, false);
    else
        p = new EvalutionBarSubscript(document, false);
    p->editable = false;
    p->remake_always = true;
    return p;
}

void EvalutionBarSubscript::Init()
{
    symbol_str = ToBasicString(std::u32string(1, symbol));
    elements->Add(ElementPtr(new Shape(this))); //evalution bar symbol
    elements->Get(0)->editable = false;
    elements->Add(ElementPtr(new CodeParagraphsBlock<Assignment>(this, true))); //block of variables
    elements->Get(1)->editable = false;

    UpdateLevel(level);
}

bool EvalutionBarSubscript::AfterFromJson()
{
    if (elements->Count() != 2)
        return false;
    if (elements->Get(1)->type != ElementType::CODE_PARAGRAPHS_BLOCK_ASSIGNMENT)
        return false;
    return true;
}

void EvalutionBarSubscript::Draw() const
{
    GetShape()->draw_func =
        [&](const Rect& r)
        {
            window->DrawFillRect(r, document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    GetShape()->Draw();
    GetBlock()->Element::Draw();
}

bool EvalutionBarSubscript::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);
    Shape* shape = GetShape();
    CodeParagraphsBlock<Assignment>* block = GetBlock();

    int block_h = block ? block->rect.height : 0;
    int left_h = 0;
    int left_baseline = 0;
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
    {
        Element* left = parent->elements->Get(pos - 1).get();
        left_h = left->rect.height;
        left_baseline = left->baseline;
    }

    int bar_h;
    if (left_h > 0)
        bar_h = left_h + block_h;
    else
        bar_h = 2 * block_h;

    int bar_w = std::max(2, (int)std::round(2 * document->config.scale));
    shape->rect.SetRect(0, 0, bar_w, bar_h);

    int block_top = bar_h - block_h;
    int gap = std::max(2, (int)std::round(2 * document->config.scale));
    if (block)
        block->rect.Move(bar_w + gap, block_top);

    if (left_h > 0)
        baseline = left_baseline;
    else
        baseline = bar_h / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void EvalutionBarSubscript::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetBlock())
        GetBlock()->UpdateLevel(_level + 1);
}

bool EvalutionBarSubscript::AfterInsert(bool with_undo)
{
    CodeParagraphsBlock<Assignment>* block = GetBlock();
    if (!block)
        return false;

    CaretState c;
    if (block->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

bool EvalutionBarSubscript::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    CodeParagraphsBlock<Assignment>* block = GetBlock();
    if (!block)
        return false;
    return block->GetFirstCaretState(caret_state, select);
}

bool EvalutionBarSubscript::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    CodeParagraphsBlock<Assignment>* block = GetBlock();
    if (!block)
        return false;
    return block->GetLastCaretState(caret_state, select);
}

std::string EvalutionBarSubscript::ToHtml() const
{
    CodeParagraphsBlock<Assignment>* block = GetBlock();
    std::string sub;
    if (block)
    {
        bool first = true;
        for (int i = 0; i < block->elements->Count(); ++i)
        {
            CodeRow<Assignment>* row = GetParagraphRow(i);
            if (!row)
                continue;
            std::string row_html = row->ToHtml();
            if (row_html.empty())
                continue;
            if (!first)
                sub += "<mo>,</mo>";
            first = false;
            sub += row_html;
        }
    }

    std::string s = "<msub>";
    s += "<mo fence=\"false\" stretchy=\"true\">|</mo>";
    s += "<mrow>" + sub + "</mrow>";
    s += "</msub>";
    return s;
}

std::u32string EvalutionBarSubscript::ToText() const
{
    std::vector<std::u32string> assignments = GetAssignments();
    if (assignments.empty())
        return U"";

    std::u32string result = U"[";
    for (size_t i = 0; i < assignments.size(); ++i)
    {
        if (i > 0)
            result += U",";
        result += assignments[i];
    }
    result += U"]";
    return result;
}

void EvalutionBarSubscript::ToParserString(ParserString& str)
{
    //the parser output is handled by the preceding expression (e.g. a derivative fraction)
}

std::vector<std::u32string> EvalutionBarSubscript::GetAssignments() const
{
    std::vector<std::u32string> result;
    CodeParagraphsBlock<Assignment>* block = GetBlock();
    if (!block)
        return result;

    for (int i = 0; i < block->elements->Count(); ++i)
    {
        CodeRow<Assignment>* row = GetParagraphRow(i);
        if (!row)
            continue;
        std::u32string text = row->ToText();
        if (!text.empty() && text != U"=")
            result.push_back(text);
    }

    return result;
}

Shape* EvalutionBarSubscript::GetShape() const
{
    return (Shape*)elements->Get(0).get();
}

CodeParagraphsBlock<Assignment>* EvalutionBarSubscript::GetBlock() const
{
    return (CodeParagraphsBlock<Assignment>*)(elements->Get(1).get());
}

CodeParagraph<Assignment>* EvalutionBarSubscript::GetParagraph(uint index) const
{
    CodeParagraphsBlock<Assignment>* block = GetBlock();
    if (!block || index >= block->elements->Count())
        return nullptr;
    return dynamic_cast<CodeParagraph<Assignment>*>(block->elements->Get(index).get());
}

CodeRow<Assignment>* EvalutionBarSubscript::GetParagraphRow(uint index) const
{
    CodeParagraph<Assignment>* p = GetParagraph(index);
    if (!p || p->elements->Count() == 0)
        return nullptr;
    return dynamic_cast<CodeRow<Assignment>*>(p->elements->Get(0).get());
}

}
