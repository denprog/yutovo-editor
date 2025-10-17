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
#include "formulas/power.h"
#include "formulas/code_string.h"
#include "formulas/multiply.h"
#include "formulas/minus.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//Graph

Graph::Graph(Element* _parent, bool with_init) :
    Formula(_parent)
{
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
}

Graph::Graph(Document* _document, bool with_init) :
    Formula(_document)
{
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
}

Graph::Graph(const Graph& source) :
    Formula(source),
    last_expression(source.last_expression),
    dependencies(source.dependencies),
    format(source.format),
    x_left(source.x_left), 
    x_right(source.x_right), 
    y_bottom(source.y_bottom), 
    y_top(source.y_top)
{
}

void Graph::Init()
{
    if (elements->Count() == 0)
    {
        elements->Add(ElementPtr(new CodeRow(this))); //y up
        elements->Add(ElementPtr(new CodeRow(this))); //expression
        elements->Add(ElementPtr(new CodeRow(this))); //y down
        elements->Add(ElementPtr(new CodeRow(this))); //x left
        elements->Add(ElementPtr(new CodeRow(this))); //variable
        elements->Add(ElementPtr(new CodeRow(this))); //x right
        elements->Add(ElementPtr(new Shape(this))); //graph
    }
    GetShape()->can_resize = true;
    GetShape()->can_move_picture = true;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    UpdateLevel(level);
    GetShape()->editable = false;
    editable = false;
}

void Graph::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Graph::Remake(bool with_elements)
{
    if (document->editing)
        moving = false;

    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    int x_left = std::max({GetExpression()->rect.width, GetYTop()->rect.width, GetYBottom()->rect.width});
    GetShape()->rect.SetRect(0, 0, format.size.width, format.size.height);
    GetYTop()->rect.Move(x_left - GetYTop()->rect.width, GetShape()->rect.top);
    GetExpression()->rect.Move(x_left - GetExpression()->rect.width, GetShape()->rect.height / 2 - GetExpression()->rect.height / 2);
    GetYBottom()->rect.Move(x_left - GetYBottom()->rect.width, GetShape()->rect.GetBottom() - GetYBottom()->rect.height);
    GetShape()->rect.Move(x_left + 2, 0);
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

void Graph::Resize(const int dx, const int dy)
{
    if (format.size.width + dx >= 100)
        format.size.width += dx;
    if (format.size.height + dy >= 100)
        format.size.height += dy;
}

void Graph::MovePicture(const int dx, const int dy)
{
    moving = true;
    double tx = (x_right - x_left) / (GetShape()->rect.width - 40);
    double ty = (y_bottom - y_top) / (GetShape()->rect.height - 40);
    double _dx = tx * dx;
    double _dy = ty * dy;
    SetNumber(x_left - _dx, GetXLeft());
    SetNumber(x_right - _dx, GetXRight());
    SetNumber(y_bottom - _dy, GetYBottom());
    SetNumber(y_top - _dy, GetYTop());
    document->AddResolveElement(logical_id);
}

void Graph::ZoomPicture(const int pixels)
{
    moving = true;
    double k = pixels > 0 ? (1 + double(pixels) / 15) : (-(1 / (double(pixels) / 15 - 1)));
    double w = (x_right - x_left) / k;
    double c = (x_right + x_left) / 2;
    SetNumber(c - w / 2, GetXLeft());
    SetNumber(c + w / 2, GetXRight());
    w = (y_top - y_bottom) / k;
    c = (y_top + y_bottom) / 2;
    SetNumber(c - w / 2, GetYBottom());
    SetNumber(c + w / 2, GetYTop());
    document->AddResolveElement(logical_id);
}

bool Graph::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    return false;
}

void Graph::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetXLeft())
        GetXLeft()->UpdateLevel(_level + 2);
    if (GetXRight())
        GetXRight()->UpdateLevel(_level + 2);
    if (GetYBottom())
        GetYBottom()->UpdateLevel(_level + 2);
    if (GetYTop())
        GetYTop()->UpdateLevel(_level + 2);
}

bool Graph::AfterInsert(bool with_undo)
{
    CaretState c;
    if (elements->Get(0)->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

void Graph::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);
    if (solving)
        document->BreakSolving(logical_id, guid, ((CodeBlock*)code.get())->code_id, false);

    solving = true;
    document->Solve(logical_id, guid, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (!moving && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    document->AddChangedElement(id);
}

void Graph::LogicalIdChanged(const LogicalId& last_id)
{
    solving = false;
}

bool Graph::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

void Graph::PutError(const Error& error)
{
    last_parser_error_code = error.parser_error_code;
    ElementId err_id = last_expression.GetElement(error.pos, error.size);
    if (!err_id.empty())
    {
        auto el = document->GetElement(err_id);
        if (el)
        {
            document->RemoveErrorMarks(parent->parent->id);
            document->AddErrorMark(err_id, 0, el->elements->Count());
            document->Redraw(err_id, false);
        }
    }
}

void Graph::SetNumber(const double num, CodeRow* el)
{
    el->elements->Clear();
    std::ostringstream oss;
    oss.precision(3);
    oss << std::scientific << num;
    std::string s = oss.str();
    std::string m = s.substr(0, s.find('e'));
    std::string e = s.substr(s.find('e') + 1);
    int exp = std::stoi(e);
    if (std::abs(exp) < 3)
    {
        std::ostringstream os;
        os.precision(3);
        os << std::fixed << num;
        m = os.str();
        e = "";
    }

    if (m.find('.') != std::string::npos)
        m = m.substr(0, m.find_last_not_of('0') + 1);

    if (m.empty())
    {
        el->AddElement(CodeStringPtr(new CodeString(el, "0")));
        return;
    }
    if (m[0] == '-')
    {
        el->AddElement(ElementPtr(new Minus(el)));
        el->AddElement(CodeStringPtr(new CodeString(el, m.substr(1, m.size() - 1))));
    }
    else
        el->AddElement(CodeStringPtr(new CodeString(el, m)));
    if (!e.empty())
    {
        el->AddElement(ElementPtr(new Multiply(el)));
        PowerPtr p(new Power(el));
        el->AddElement(p);
        p->AddBase(CodeStringPtr(new CodeString(p.get(), "10")));
        if (e[0] == '-')
        {
            e = e.substr(1, e.size() - 1);
            e.erase(0, e.find_first_not_of('0'));
            if (e.empty())
                e = "0";
            p->AddExponent(ElementPtr(new Minus(p.get())));
            p->AddExponent(CodeStringPtr(new CodeString(p.get(), e)));
        }
        else
        {
            if (e[0] == '+')
                e = e.substr(1, e.size() - 1);
            e.erase(0, e.find_first_not_of('0'));
            if (e.empty())
                e = "0";
            p->AddExponent(CodeStringPtr(new CodeString(p.get(), e)));
        }
    }
}

CodeRow* Graph::GetYTop() const
{
    return (CodeRow*)elements->Get(0).get();
}

CodeRow* Graph::GetExpression() const
{
    return (CodeRow*)elements->Get(1).get();
}

CodeRow* Graph::GetYBottom() const
{
    return (CodeRow*)elements->Get(2).get();
}

CodeRow* Graph::GetXLeft() const
{
    return (CodeRow*)elements->Get(3).get();
}

CodeRow* Graph::GetVariable() const
{
    return (CodeRow*)elements->Get(4).get();
}

CodeRow* Graph::GetXRight() const
{
    return (CodeRow*)elements->Get(5).get();
}

Shape* Graph::GetShape() const
{
    return (Shape*)elements->Get(6).get();
}

//GraphLine

GraphLine::GraphLine(Element* _parent, bool with_init) : 
    Graph(_parent, with_init)
{
    type = ElementType::GRAPH_LINE;
    if (with_init)
        Init();
}

GraphLine::GraphLine(Document* _document, bool with_init) : 
    Graph(_document, with_init)
{
    type = ElementType::GRAPH_LINE;
    if (with_init)
        Init();
}

GraphLine::GraphLine(const GraphLine& source) : 
    Graph(source),
    x(source.x), 
    y(source.y)
{
    Init();
}

void GraphLine::Init()
{
    Graph::Init();

    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            graph.NewFrame();
            graph.SetFlagAdv(1, MGL_NO_SCALE_REL);
            graph.SetScaleText(false);
            graph.SetSize(r.width, r.height, false);
            graph.SubPlot(1, 1, 0, "#");
            graph.InPlot(0.05, 0.95, 0.05, 0.95);

            if (solving && !moving)
            {
                graph.SetRanges(-1, 1, -1, 1);
                graph.SetFontSize(level + 1);
                graph.Puts(mglPoint(0, 0), "~");
            }
            else if (last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR && last_error_code != yutovo_solver::ErrorCode::OK && !moving)
            {
                graph.SetRanges(-1, 1, -1, 1);
                graph.SetFontSize(level + 1);
                if (last_parser_error_code != yutovo_calculator::ParserExceptionCode::None)
                    graph.Puts(mglPoint(0, 0), ErrorCodeToString(last_parser_error_code).c_str());
                else
                    graph.Puts(mglPoint(0, 0), ErrorCodeToString(last_error_code).c_str());
            }
            else
            {
                graph.SetRanges(x_left, x_right, y_bottom, y_top);
                graph.SetFontSize(level);
                graph.Axis("xyz", "r-1", "h-1");
                graph.Grid("xyz", "h");
                graph.SetQuality(MGL_DRAW_NORM);

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
    #ifdef EMSCRIPTEN
                    std::string f = "{" + format.plot_color.ToRGB() + "}-" + std::to_string(format.plot_width);
    #else
                    std::string f = "{" + format.plot_color.ToBGR() + "}-" + std::to_string(format.plot_width);
    #endif
                    graph.Plot(x_data, y_data, f.c_str());
                }
            }

            const unsigned char* picture = graph.GetRGBA();
            std::vector<unsigned char> arr(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
            window->DrawImage(r.left + 1, r.top + 1, r.width, r.height, arr);
        };
}

bool GraphLine::AfterFromJson()
{
    Init();
    last_expression.Reset();
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

void GraphLine::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Formula::ToJson(value, alloc);
    format.ToJson(value, alloc);
}

Element* GraphLine::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    GraphFormat f;
    f.FromJson(value, alloc);
    GraphLine* el = parent ? new GraphLine(parent, false) : new GraphLine(document, false);
    el->format = f;
    return el;
}

void GraphLine::Solve()
{
    Formula::Solve();

    ParserString str;
    str.Add(id, U"graph_line(");
    GetExpression()->ToParserString(str);
    str.Add(id, U",");
    GetVariable()->ToParserString(str);
    str.Add(id, U",");
    GetXLeft()->ToParserString(str);
    str.Add(id, U",");
    GetXRight()->ToParserString(str);
    str.Add(id, U",");
    GetYBottom()->ToParserString(str);
    str.Add(id, U",");
    GetYTop()->ToParserString(str);
    str.Add(id, U",");
    str.Add(id, ToUtfString(std::to_string(graph.GetWidth())));
    str.Add(id, U")");

    if (last_expression != str)
    {
        last_expression = str;
        document->AddResolveElement(logical_id);
    }
}

void GraphLine::PutResult(Result& result)
{
    solving = false;

    last_error_code = result.error.error_code;
    dependencies = result.dependencies;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expression.Reset();
        return;
    }

    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
        PutError(result.error); //put error message
    else
        document->RemoveErrorMarks(id);

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

std::u32string GraphLine::ToText() const
{
    std::u32string s = U"graph_line(";
    for (int i = 0; i < elements->Count(); ++i)
    {
        s += elements->Get(i)->ToText();
        if (i < elements->Count() - 2)
            s += U",";
    }
    s += U")";
    return s;
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

}
