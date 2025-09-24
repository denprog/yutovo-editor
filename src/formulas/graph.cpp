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

//GraphLine

GraphLine::GraphLine(Element* _parent, bool with_init) : 
    Formula(_parent)
{
    type = ElementType::GRAPH_LINE;
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
    if (with_init)
        Init();
}

GraphLine::GraphLine(Document* _document, bool with_init) : 
    Formula(_document)
{
    type = ElementType::GRAPH_LINE;
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
    if (with_init)
        Init();
}

GraphLine::GraphLine(const GraphLine& source) : 
    Formula(source),
    last_expression(source.last_expression),
    dependencies(source.dependencies)
{
}

void GraphLine::Init()
{
    elements->Add(ElementPtr(new CodeRow(this))); //y up
    elements->Add(ElementPtr(new CodeRow(this))); //expression
    elements->Add(ElementPtr(new CodeRow(this))); //y down
    elements->Add(ElementPtr(new CodeRow(this))); //x left
    elements->Add(ElementPtr(new CodeRow(this))); //variable
    elements->Add(ElementPtr(new CodeRow(this))); //x right
    elements->Add(ElementPtr(new Shape(this))); //graph
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    UpdateLevel(level);
}

bool GraphLine::AfterFromJson()
{
    if (elements->Count() != 7)
        return false;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    return true;
}

Element* GraphLine::Clone()
{
    return new GraphLine(*this);
}

Element* GraphLine::Create(Element* _parent)
{
    return new GraphLine(_parent);
}

Element* GraphLine::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new GraphLine(parent, false);
    return new GraphLine(document, false);
}

void GraphLine::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            graph.SetSize(r.width, r.height);
            graph.NewFrame();
            graph.SetRanges(x_left, x_right, y_bottom, y_top);
            graph.SubPlot(1, 1, 0, "#");
            double x1, x2, y1, y2;
            x1 = double(20) / r.width;
            x2 = double(20 + 2) / r.width;
            y1 = double(r.height - 2 - 20) / r.height;
            y2 = double(r.height - 20) / r.height;
            graph.InPlot(x1, y1, x2, y2);
            graph.SetFontSize(2);
            graph.Axis("xyz", "r-1", "h-1");
            graph.Grid("xyz", "h");
            graph.SetQuality(MGL_DRAW_WIRE);

            if (!x.empty() && !y.empty())
            {
                mglData x_data(x.size());
                mglData y_data(y.size());
                for (size_t i = 0; i < x.size() && i < y.size(); ++i)
                {
                    if (x[i] >= x_left && x[i] <= x_right)
                    {
                        x_data.a[i] = x[i];
                        y_data.a[i] = y[i];
                    }
                }
                graph.Plot(x_data, y_data, "g-1");
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

    Formula::Draw();
}

bool GraphLine::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    int x_left = std::max({GetExpression()->rect.width, GetYUp()->rect.width, GetYDown()->rect.width});
    GetShape()->rect.SetRect(0, 0, 400, 400);
    GetYUp()->rect.Move(x_left - GetYUp()->rect.width, GetShape()->rect.top);
    GetExpression()->rect.Move(x_left - GetExpression()->rect.width, GetShape()->rect.height / 2 - GetExpression()->rect.height / 2);
    GetYDown()->rect.Move(x_left - GetYDown()->rect.width, GetShape()->rect.GetBottom() - GetYDown()->rect.height);
    GetShape()->rect.Move(x_left, 0);
    GetXLeft()->rect.Move(GetShape()->rect.left, GetShape()->rect.GetBottom() + 2);
    GetVariable()->rect.Move(x_left + 2 + GetShape()->rect.width / 2 - GetVariable()->rect.width / 2, GetShape()->rect.GetBottom() + 2);
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

bool GraphLine::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return false;
}

void GraphLine::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
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

void GraphLine::Solve()
{
    Formula::Solve();

    ParserString func_str, arg_str, x_left_str, x_right_str, y_down_str, y_up_str;
    GetExpression()->ToParserString(func_str);
    GetVariable()->ToParserString(arg_str);
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

void GraphLine::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);
    if (solving)
        document->BreakSolving(logical_id, guid, ((CodeBlock*)code.get())->code_id);

    solving = true;
    document->Solve(logical_id, guid, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    document->AddChangedElement(id);
}

void GraphLine::PutResult(Result& result)
{
    solving = false;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expression.Reset();
        return;
    }

    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        //put error message
    }
    else
    {
        document->RemoveErrorMarks(id);
    }

    std::locale::global(std::locale::classic());

    auto to_double = 
        [](Value& v)
        {
            double r;
            try
            {
                r = std::stod(v.value["mantissa"] + "E" + v.value["exponent"]);
            }
            catch (const std::exception& ex)
            {
                r = std::numeric_limits<double>::quiet_NaN();
            }
            return r;
        };

    x.clear();
    y.clear();

    if (result.values.size() < 4)
    {
        document->Redraw(id, false);
        return;
    }

    //the first items are the bounds of the graph
    x_left = to_double(result.values[0]);
    x_right = to_double(result.values[1]);
    y_bottom = to_double(result.values[2]);
    y_top = to_double(result.values[3]);

    //next items are the coords of the points
    for (size_t i = 4, j = 5; i < result.values.size() && j < result.values.size(); i += 2, j += 2)
    {
        x.push_back(to_double(result.values[i]));
        y.push_back(to_double(result.values[j]));
    }

    document->Redraw(id, false);
}

bool GraphLine::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

void GraphLine::ToParserString(ParserString& str)
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

CodeRow* GraphLine::GetYUp() const
{
    return (CodeRow*)elements->Get(0).get();
}

CodeRow* GraphLine::GetExpression() const
{
    return (CodeRow*)elements->Get(1).get();
}

CodeRow* GraphLine::GetYDown() const
{
    return (CodeRow*)elements->Get(2).get();
}

CodeRow* GraphLine::GetXLeft() const
{
    return (CodeRow*)elements->Get(3).get();
}

CodeRow* GraphLine::GetVariable() const
{
    return (CodeRow*)elements->Get(4).get();
}

CodeRow* GraphLine::GetXRight() const
{
    return (CodeRow*)elements->Get(5).get();
}

Shape* GraphLine::GetShape() const
{
    return (Shape*)elements->Get(6).get();
}

}
