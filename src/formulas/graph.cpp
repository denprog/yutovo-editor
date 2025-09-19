/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "graph.h"
#include "document.h"
#include "code_row.h"

namespace yutovo
{

//Graph

Graph::Graph(Element* _parent, bool with_init) : 
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::GRAPH_2D;
    if (with_init)
        Init();
}

Graph::Graph(Document* _document, bool with_init) : 
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::GRAPH_2D;
    if (with_init)
        Init();
}

Graph::Graph(const Graph& source) : 
    MiddleShapeFormula(source)
{
}

void Graph::Init()
{
    elements->Add(ElementPtr(new CodeRow(this)));
    elements->Add(ElementPtr(new CodeRow(this)));
    elements->Add(ElementPtr(new CodeRow(this)));
    elements->Add(ElementPtr(new CodeRow(this)));
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
}

bool Graph::AfterFromJson()
{
    if (elements->Count() != 7)
        return false;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    return true;
}

Element* Graph::Clone()
{
    return new Graph(*this);
}

Element* Graph::Create(Element* _parent)
{
    return new Graph(_parent);
}

Element* Graph::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Graph(parent, false);
    return new Graph(document, false);
}

void Graph::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            graph.SetSize(r.width, r.height);
            graph.Axis();
            graph.Grid();
            graph.Box();
            const unsigned char* picture = graph.GetRGBA();
            std::vector<unsigned char> arr(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
            window->DrawImage(r.left + 1, r.top + 1, r.width, r.height, arr);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    MiddleShapeFormula::Draw();
}

bool Graph::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);

    int x_left = std::max(GetFirst()->rect.width, std::max(GetYUp()->rect.width, GetYDown()->rect.width));
    GetShape()->rect.SetRect(0, 0, 200, 200);
    GetFirst()->rect.Move(0, GetShape()->rect.height / 2 - GetFirst()->rect.height / 2);
    GetLast()->rect.Move(GetFirst()->rect.width + 2 + GetShape()->rect.width / 2 - GetLast()->rect.width / 2,
        GetShape()->rect.height + 2);
    GetShape()->rect.Move(x_left - GetFirst()->rect.width + 2, 0);
    GetXLeft()->rect.Move(GetShape()->rect.left, GetShape()->rect.height + 2);
    GetXRight()->rect.Move(GetShape()->rect.GetRight() - GetXRight()->rect.width, GetShape()->rect.height + 2);
    GetYDown()->rect.Move(x_left - GetYDown()->rect.width, GetShape()->rect.GetBottom() - GetYDown()->rect.height);
    GetYUp()->rect.Move(x_left - GetYUp()->rect.width, GetShape()->rect.top);

    baseline = GetShape()->rect.GetBottom() - GetShape()->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Graph::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetXLeft())
        GetXLeft()->UpdateLevel(_level + 1);
    if (GetXRight())
        GetXRight()->UpdateLevel(_level + 1);
    if (GetYDown())
        GetYDown()->UpdateLevel(_level + 1);
    if (GetYUp())
        GetYUp()->UpdateLevel(_level + 1);
}

void Graph::Solve()
{
    MiddleShapeFormula::Solve();

    ParserString func_str, arg_str, x_left_str, x_right_str, y_down_str, y_up_str;
    GetFirst()->ToParserString(func_str);
    GetLast()->ToParserString(arg_str);
    GetXLeft()->ToParserString(x_left_str);
    GetXRight()->ToParserString(x_right_str);
    GetYDown()->ToParserString(y_down_str);
    GetYUp()->ToParserString(y_up_str);
    if (last_func_expr.Length() != 0 || last_arg_expr.Length() != 0 || last_x_left_expr.Length() != 0 || 
        last_x_right_expr.Length() != 0 || last_y_down_expr.Length() != 0 || last_y_up_expr.Length() != 0)
    {
        empty = false;
    }
    if (last_func_expr != func_str || last_arg_expr != arg_str || last_x_left_expr != x_left_str || 
        last_x_right_expr != x_right_str || last_y_down_expr != y_down_str || last_y_up_expr != y_up_str || empty)
    {
        document->AddResolveElement(id);
    }

    last_func_expr = func_str;
    last_arg_expr = arg_str;
    last_x_left_expr = x_left_str;
    last_x_right_expr = x_right_str;
    last_y_down_expr = y_down_str;
    last_y_up_expr = y_up_str;
}

void Graph::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    Solve();
}

void Graph::ToParserString(ParserString& str)
{
    str.Add(id, U"graph(");
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, U",");
    if (elements->Count() < 7)
    {
        str.Add(id, U")");
        return;
    }
    elements->Get(2)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(3)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(4)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(5)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(6)->ToParserString(str);
    str.Add(id, U")");

    if (elements->Count() > 0)
    {
        int start = str.Length();
        elements->Get(0)->ToParserString(str);
        str.Annotate(id, start, str.Length());
    }
}

CodeRow* Graph::GetXLeft() const
{
    return (CodeRow*)elements->Get(3).get();
}

CodeRow* Graph::GetXRight() const
{
    return (CodeRow*)elements->Get(4).get();
}

CodeRow* Graph::GetYDown() const
{
    return (CodeRow*)elements->Get(5).get();
}

CodeRow* Graph::GetYUp() const
{
    return (CodeRow*)elements->Get(6).get();
}

}
