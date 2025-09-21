/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "graph.h"
#include "document.h"
#include "formulas/code_block.h"
#include "code_row.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//Graph

Graph::Graph(Element* _parent, bool with_init) : 
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::GRAPH_LINE;
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
    if (with_init)
        Init();
}

Graph::Graph(Document* _document, bool with_init) : 
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::GRAPH_LINE;
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
    if (with_init)
        Init();
}

Graph::Graph(const Graph& source) : 
    MiddleShapeFormula(source),
    last_expression(source.last_expression),
    dependencies(source.dependencies)
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
            graph.NewFrame();
            graph.Axis();
            graph.SetRanges(-1, 1, -1, 1);
            graph.Grid();
            graph.Box();

            if (!x.empty() && !y.empty())
            {
                mglData x_data(x.size());
                mglData y_data(y.size());
                for (size_t i = 0; i < x.size() && i < y.size(); ++i)
                {
                    if (y[i] >= -1 && y[i] <= 1)
                    {
                        x_data.a[i] = x[i];
                        y_data.a[i] = y[i];
                    }
                }
                graph.Plot(x_data, y_data, "r");
            }
            
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

    int x_left = std::max({GetFirst()->rect.width, GetYUp()->rect.width, GetYDown()->rect.width});
    GetShape()->rect.SetRect(0, 0, 400, 400);
    GetYUp()->rect.Move(x_left - GetYUp()->rect.width, GetShape()->rect.top);
    GetFirst()->rect.Move(x_left - GetFirst()->rect.width, GetShape()->rect.height / 2 - GetFirst()->rect.height / 2);
    GetYDown()->rect.Move(x_left - GetYDown()->rect.width, GetShape()->rect.GetBottom() - GetYDown()->rect.height);
    GetShape()->rect.Move(x_left, 0);
    GetXLeft()->rect.Move(GetShape()->rect.left, GetShape()->rect.GetBottom() + 2);
    GetLast()->rect.Move(x_left + 2 + GetShape()->rect.width / 2 - GetLast()->rect.width / 2, GetShape()->rect.GetBottom() + 2);
    GetXRight()->rect.Move(GetShape()->rect.GetRight() - GetXRight()->rect.width, GetShape()->rect.GetBottom() + 2);

    baseline = GetShape()->rect.GetBottom() - GetShape()->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Graph::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return false;
}

void Graph::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetXLeft())
        GetXLeft()->UpdateLevel(_level + 2);
    if (GetXRight())
        GetXRight()->UpdateLevel(_level + 2);
    if (GetYDown())
        GetYDown()->UpdateLevel(_level + 2);
    if (GetYUp())
        GetYUp()->UpdateLevel(_level + 2);
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

    ParserString str;
    str.Add(id, U"graph_line(");
    str.Add(func_str);
    str.Add(id, U",");
    str.Add(arg_str);
    str.Add(id, U",");
    str.Add(x_left_str);
    str.Add(id, U",");
    str.Add(x_right_str);
    str.Add(id, U",");
    str.Add(y_down_str);
    str.Add(id, U",");
    str.Add(y_up_str);
    str.Add(id, U",");
    str.Add(id, ToUtfString(std::to_string(graph.GetWidth())));
    str.Add(id, U",");
    str.Add(id, ToUtfString(std::to_string(x_pos)));
    str.Add(id, U",");
    str.Add(id, ToUtfString(std::to_string(x_inc)));
    str.Add(id, U")");
   
    if (last_expression != str)
    {
        last_expression = str;
        document->AddResolveElement(id);
    }
}

void Graph::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);
    document->Solve(logical_id, guid, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    document->AddChangedElement(id);
}

void Graph::PutResult(Result& _result)
{
    last_error_code = _result.error.error_code;
    if (_result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expression.Reset();
        return;
    }

    if (_result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        //put error message
    }
    else
    {
        document->RemoveErrorMarks(id);
    }

    std::locale::global(std::locale::classic());

    x.clear();
    y.clear();

    for (size_t i = 0, j = 1; i < _result.values.size() && j < _result.values.size(); i += 2, j += 2)
    {
        auto to_double = 
            [](Value& v)
            {
                size_t p = 0;
                double r;
                std::string m = v.value["mantissa"];
                std::string e = v.value["exponent"];
                try
                {
                    r = std::stod(m + e, &p);
                }
                catch (const std::exception& ex)
                {
                    r = std::numeric_limits<double>::quiet_NaN();
                }
                return r;
            };
        
        x.push_back(to_double(_result.values[i]));
        y.push_back(to_double(_result.values[j]));
    }

    document->Redraw(id, false);
}

bool Graph::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

void Graph::ToParserString(ParserString& str)
{
    str.Add(id, U"graph_line(");
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
