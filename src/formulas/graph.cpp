/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "graph.h"
#include "document.h"
#include "formulas/code_block.h"
#include "formulas/code_paragraph.h"
#include "formulas/code_paragraphs_block.h"
#include "code_row.h"
#include "formulas/power.h"
#include "formulas/code_string.h"
#include "formulas/multiply.h"
#include "formulas/minus.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <algorithm>
#include <cmath>

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo
{

typedef unsigned char uchar;

std::recursive_mutex Graph::mathgl_mutex;

//Graph

Graph::Graph(Element* _parent, bool with_init) :
    Formula(_parent)
{
}

Graph::Graph(Document* _document, bool with_init) :
    Formula(_document)
{
}

Graph::Graph(const Graph& source) :
    Formula(source),
    last_expressions(source.last_expressions),
    dependencies(source.dependencies),
    format(source.format),
    x_left(source.x_left), 
    x_right(source.x_right), 
    y_bottom(source.y_bottom), 
    y_top(source.y_top),
    config(source.config)
{
}

void Graph::Init()
{
    if (elements->Count() == 0)
    {
        elements->Add(ElementPtr(new CodeRow<>(this))); //y up
        elements->Add(ElementPtr(new CodeParagraphsBlock<>(this, true))); //expression
        elements->Add(ElementPtr(new CodeRow<>(this))); //y down
        elements->Add(ElementPtr(new CodeRow<>(this))); //x left
        elements->Add(ElementPtr(new CodeRow<>(this))); //variable
        elements->Add(ElementPtr(new CodeRow<>(this))); //x right
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
    if (GetXLeft()->rect.width + GetVariable()->rect.width / 2 > GetShape()->rect.width / 2)
        GetShape()->rect.Move(x_left + 2 + ((GetXLeft()->rect.width + GetVariable()->rect.width / 2) - GetShape()->rect.width / 2), 0);
    else
        GetShape()->rect.Move(x_left + 2, 0);
    GetXLeft()->rect.Move(x_left, GetShape()->rect.GetBottom() + 2);
    GetVariable()->rect.Move(GetShape()->rect.left + 2 + GetShape()->rect.width / 2 - GetVariable()->rect.width / 2, GetShape()->rect.GetBottom() + 2);
    if (GetVariable()->rect.width / 2 + GetXRight()->rect.width > GetShape()->rect.width / 2 - 2)
        GetXRight()->rect.Move(GetVariable()->rect.GetRight() + 2, GetShape()->rect.GetBottom() + 2);
    else
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

void Graph::MovePicture(const int dx, const int dy, bool shift)
{
}

void Graph::ZoomPicture(const int pixels)
{
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
        GetXLeft()->UpdateLevel(_level + 1);
    if (GetXRight())
        GetXRight()->UpdateLevel(_level + 1);
    if (GetYBottom())
        GetYBottom()->UpdateLevel(_level + 1);
    if (GetYTop())
        GetYTop()->UpdateLevel(_level + 1);
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

void Graph::GetImage(std::string& image_base64) const
{
    std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

    Draw();

    const unsigned char* picture = graph.GetRGBA();
    std::vector<unsigned char> rgba(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
    std::vector<unsigned char> png;

    RgbaToPng(rgba, graph.GetWidth(), graph.GetHeight(), png);
    image_base64 = yutovo::Base64Encode(png);
}

void Graph::SetNumber(const double num, CodeRow<>* el)
{
    bool b = document->caret->IsInsideElement(el->id);
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
    if (b)
    {
        CaretState c;
        if (el->GetFirstCaretState(c, nullptr))
        {
            document->caret->SetState(c);
            document->selection.Clear();
        }
    }
}

CodeRow<>* Graph::GetYTop() const
{
    return (CodeRow<>*)elements->Get(0).get();
}

CodeParagraphsBlock<>* Graph::GetExpression() const
{
    return (CodeParagraphsBlock<>*)elements->Get(1).get();
}

CodeRow<>* Graph::GetYBottom() const
{
    return (CodeRow<>*)elements->Get(2).get();
}

CodeRow<>* Graph::GetXLeft() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

CodeRow<>* Graph::GetVariable() const
{
    return (CodeRow<>*)elements->Get(4).get();
}

CodeRow<>* Graph::GetXRight() const
{
    return (CodeRow<>*)elements->Get(5).get();
}

Shape* Graph::GetShape() const
{
    return (Shape*)elements->Get(6).get();
}

//GraphLine

const std::vector<Color> GraphLine::default_colors = {Color::Red(), Color::Blue(), Color::Green(), Color::Magenta(), Color::Cian(), Color::Black()};

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
    plots(source.plots)
{
    Init();
}

void GraphLine::Init()
{
    Graph::Init();

    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

            graph.NewFrame();
            graph.SetFlagAdv(1, MGL_NO_SCALE_REL);
            graph.SetScaleText(false);
            graph.SetSize(r.width, r.height, false);
            graph.SubPlot(1, 1, 0, "#");
            graph.InPlot(0.05, 0.95, 0.05, 0.95);
            if (r.width < 200 || r.height < 200)
                graph.SetPenDelta(0.5);
            else if (r.width < 300 || r.height < 300)
                graph.SetPenDelta(0.6);
            else if (r.width < 400 || r.height < 400)
                graph.SetPenDelta(0.7);
            else
                graph.SetPenDelta(1.);

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
                std::string f = "{" + format.color.ToRGB() + "}";
                graph.Axis("xy", std::string(f + "-1").c_str(), "h-1");
                if (format.grid_width > 0)
                    graph.Grid("xy", std::string("h" + std::to_string(format.grid_width) + f).c_str());
                graph.SetQuality(MGL_DRAW_NORM);
                //graph.SetPenDelta(0.5);
                for (const Plot& p : plots)
                {
                    auto& x = p.x;
                    auto& y = p.y;
                    if (!x.empty() && !y.empty())
                    {
                        mglData x_data(x.size());
                        mglData y_data(y.size());
                        x_data.Set(x);
                        y_data.Set(y);
                        f = "{" + p.format.color.ToRGB() + "}-" + std::to_string(p.format.width);
                        graph.Plot(x_data, y_data, f.c_str());
                    }
                }
            }

            const unsigned char* picture = graph.GetRGBA();
            std::vector<unsigned char> arr(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
            window->DrawImage(r.left + 1, r.top + 1, r.width, r.height, arr);
        };
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
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& p : plots)
    {
        rapidjson::Value v;
        v.SetObject();
        p.format.ToJson(v, alloc);
        arr.PushBack(v, alloc);
    }
    value.AddMember("plots", arr, alloc);
}

Element* GraphLine::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    GraphFormat f;
    f.FromJson(value, alloc);
    GraphLine* el = parent ? new GraphLine(parent, false) : new GraphLine(document, false);
    el->format = f;

    if (value.HasMember("plots") && value["plots"].IsArray())
    {
        rapidjson::Value::ConstArray arr = value["plots"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
            Plot p{guid};
            if (arr[i].IsObject())
            {
                rapidjson::Value::ConstObject v = arr[i].GetObject();
                p.format.FromJson(v, alloc);
            }
            el->plots.push_back(p);
        }
    }

    return el;
}

bool GraphLine::AfterFromJson()
{
    if (elements->Count() < 7)
        elements->Clear();
    else if (elements->Get(1)->type != ElementType::CODE_PARAGRAPHS_BLOCK)
    {
        //this is old type of graph, transform it
        ElementPtr el = elements->Get(1);
        elements->RemoveAt(1, 1);
        ElementPtr b(new CodeParagraphsBlock<>(this, false));
        elements->Insert(b, 1);
        b->elements->Clear();

        CodeParagraph<>* p = new CodeParagraph<>(b.get(), true);
        p->elements->Add(el);
        StringFormatPtr f = GetStringFormat();
        Color color;
        uint width = 1;
        GetPlotFormat(0, color, width);
        p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough, 
            f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
        b->elements->Add(ElementPtr(p));
    }
    else
    {
        StringFormatPtr f = GetStringFormat();
        Color color;
        uint width = 1;
        for (int i = 0; i < elements->Get(1)->elements->Count(); ++i)
        {
            CodeParagraph<>* p = (CodeParagraph<>*)elements->Get(1)->elements->Get(i).get();
            GetPlotFormat(i, color, width);
            p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough, 
                f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
        }
    }

    Init();
    last_expressions.clear();
    return true;
}

void GraphLine::MovePicture(const int dx, const int dy, bool shift)
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

void GraphLine::ZoomPicture(const int pixels)
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

void GraphLine::Solve()
{
    if (rect.width == 0)
        return;
        
    std::vector<ParserString> expressions;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        ParserString str;
        ElementPtr el = GetExpression()->elements->Get(i);
        ElementId& _id = el->id;
        str.Add(_id, U"graph_line(");
        el->ToParserString(str);
        str.Add(_id, U",");
        GetVariable()->ToParserString(str);
        str.Add(_id, U",");
        GetXLeft()->ToParserString(str);
        str.Add(_id, U",");
        GetXRight()->ToParserString(str);
        str.Add(_id, U",");
        GetYBottom()->ToParserString(str);
        str.Add(_id, U",");
        GetYTop()->ToParserString(str);
        str.Add(_id, U",");
        str.Add(_id, ToUtfString(std::to_string(rect.width)));
        str.Add(_id, U")");
        expressions.push_back(str);
    }

    if (last_expressions != expressions)
    {
        last_expressions = expressions;
        document->AddResolveElement(logical_id);
    }
}

void GraphLine::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);

    //plots count must be count of the expressions
    while (plots.size() < GetExpression()->elements->Count())
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % 4]});
    }
    if (plots.size() > GetExpression()->elements->Count())
        plots.resize(GetExpression()->elements->Count());

    if (solving)
    {
        for (size_t i = 0; i < plots.size(); ++i)
            document->BreakSolving(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, false);
    }

    solving = true;
    for (const Plot& p : plots)
    {
        for (size_t i = 0; i < plots.size() && i < last_expressions.size(); ++i)
        {
            document->Solve(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, config, 
                !GetParent(1)->visible, last_expressions[i].Text(), 
                (!moving && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
        }
    }
    document->AddChangedElement(id);
}

void GraphLine::PutResult(Result& result)
{
    solving = false;

    last_error_code = result.error.error_code;
    dependencies = result.dependencies;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expressions.clear();
        return;
    }

    auto it = std::find_if(plots.begin(), plots.end(), 
        [result](const Plot& p)
        {
            return p.guid == result.guid;
        });
    if (it == plots.end())
        return;

    Plot& plot = *it;

    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        //put error message
        size_t i = std::distance(plots.begin(), it);
        last_parser_error_code = result.error.parser_error_code;
        if (i < last_expressions.size())
        {
            ElementId err_id = last_expressions[i].GetElement(result.error.pos, result.error.size);
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
    }
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

    plot.x.clear();
    plot.y.clear();

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
        plot.x.push_back(to_double(result.values[i]));
        plot.y.push_back(to_double(result.values[j]));
    }

    document->Redraw(id, false);
}

bool GraphLine::MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    for (int i = 0; i < elements->Get(1)->elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(1)->elements->Get(i);
        if (el->elements->Count() == 0)
            return false;
        Rect r1 = el->GetAbsoluteRect();
        Rect r2 = el->elements->Get(0)->GetAbsoluteRect();
        Rect r{r1.left, r1.top, r2.left - r1.left, r2.GetBottom() - r1.top};
        if (r.IsPointInside(x, y))
        {
            mouse_l_button_pos = i;
            hold_type = MouseHoldType::PLOT_FORMAT_DIALOG;
            hold_id = id;
            return true;
        }
    }
    return false;
}

std::string GraphLine::ToHtml() const
{
    std::string expr;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        const auto& el = GetExpression()->elements->Get(i);
        if (!el->IsVisible())
            continue;
        //the template is not solved yet - plots may be empty or shorter than the paragraph list
        std::string color = i < (int)plots.size() ? plots[i].format.color.ToHex() : default_colors[i % default_colors.size()].ToHex();
        expr += "<p><span style=\"color: " + color + ";\">█&nbsp;</span>" + el->ToHtml() + "</p>";
    }

    std::string image_base64;
    GetImage(image_base64);
    std::string s =
        "<table>"
            "<tr>"
                "<td style=\"height:100%; vertical-align:top;\">"
                    "<table style=\"height:100%;\">"
                        "<tr>"
                            "<td style=\"vertical-align:top;text-align:right;\">" + GetYTop()->ToHtml() + "</td>"
                        "</tr>"
                        "<tr>"
                            "<td style=\"vertical-align:middle;\">" + expr + "</td>"
                        "</tr>"
                        "<tr>"
                            "<td style=\"vertical-align:bottom;text-align:right\">" + GetYBottom()->ToHtml() + "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
                "<td>"
                    "<img src=\"data:image/png;base64," + image_base64 + "\">"
                "</td>"
            "</tr>"
            "<tr>"
                "<td>"
                "</td>"
                "<td style=\"vertical-align:top;\">"
                    "<table style=\"width:100%;\">"
                        "<tr>"
                            "<td style=\"vertical-align:top;text-align:left;\">" + GetXLeft()->ToHtml() + "</td>"
                            "<td style=\"vertical-align:top;text-align:center\">" + GetVariable()->ToHtml() + "</td>"
                            "<td style=\"vertical-align:top;text-align:right;\">" + GetXRight()->ToHtml() + "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
            "</tr>"
       "</table>";
    return s;
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

void GraphLine::GetPlotFormat(const int pos, Color& color, uint& width)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    color = plots[pos].format.color;
    width = plots[pos].format.width;
}

void GraphLine::GetPlotFormat(const int pos, PlotFormat& format)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    format = plots[pos].format;
}

void GraphLine::GetPlotFormat(PlotFormat& format)
{
    GetPlotFormat(mouse_l_button_pos, format);
}

void GraphLine::SetPlotFormat(const PlotFormat& format)
{
    if (mouse_l_button_pos >= plots.size())
        return;
    plots[mouse_l_button_pos].format.color = format.color;
    plots[mouse_l_button_pos].format.width = format.width;

    ElementPtr p = elements->Get(1)->elements->Get(mouse_l_button_pos);
    if (!p)
        return;
    StringFormatPtr f = GetStringFormat();
    ((CodeParagraph<>*)p.get())->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
        f->subscript, f->superscript, format.color, f->text_bg_color, f->text_bg_selection_color));
}

//GraphSurface

const std::vector<Color> GraphSurface::default_colors = {Color::Red(), Color::Blue(), Color::Green(), Color::Magenta(), Color::Cian(), Color::Black()};

GraphSurface::GraphSurface(Element* _parent, bool with_init) :
    Graph(_parent, with_init)
{
    type = ElementType::GRAPH_SURFACE;
    if (with_init)
        Init();
}

GraphSurface::GraphSurface(Document* _document, bool with_init) :
    Graph(_document, with_init)
{
    type = ElementType::GRAPH_SURFACE;
    if (with_init)
        Init();
}

GraphSurface::GraphSurface(const GraphSurface& source) :
    Graph(source),
    plots(source.plots),
    rot_x(source.rot_x),
    rot_z(source.rot_z)
{
    Init();
}

void GraphSurface::Init()
{
    if (elements->Count() == 0)
    {
        elements->Add(ElementPtr(new CodeRow<>(this))); //y top
        elements->Add(ElementPtr(new CodeParagraphsBlock<>(this, true))); //expressions
        elements->Add(ElementPtr(new CodeRow<>(this))); //y variable
        elements->Add(ElementPtr(new CodeRow<>(this))); //y bottom
        elements->Add(ElementPtr(new CodeRow<>(this))); //x left
        elements->Add(ElementPtr(new CodeRow<>(this))); //x variable
        elements->Add(ElementPtr(new CodeRow<>(this))); //x right
        elements->Add(ElementPtr(new Shape(this))); //graph
    }
    GetShape()->can_resize = true;
    GetShape()->can_move_picture = true;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    UpdateLevel(level);
    GetShape()->editable = false;
    editable = false;

    GetShape()->draw_func =
        [&](const Rect& r)
        {
            std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

            graph.NewFrame();
            graph.SetFlagAdv(1, MGL_NO_SCALE_REL);
            graph.SetScaleText(false);
            graph.SetSize(r.width, r.height, false);
            graph.SubPlot(1, 1, 0, "#");
            graph.InPlot(0.02, 0.98, 0.02, 0.98);
            if (r.width < 200 || r.height < 200)
                graph.SetPenDelta(0.5);
            else if (r.width < 300 || r.height < 300)
                graph.SetPenDelta(0.6);
            else if (r.width < 400 || r.height < 400)
                graph.SetPenDelta(0.7);
            else
                graph.SetPenDelta(1.);

            if (solving && !moving)
            {
                graph.Rotate(0, 0);
                graph.SetRanges(-1, 1, -1, 1);
                graph.SetFontSize(level + 1);
                graph.Puts(mglPoint(0, 0), "~");
            }
            else if (last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR && last_error_code != yutovo_solver::ErrorCode::OK && !moving)
            {
                graph.Rotate(0, 0);
                graph.SetRanges(-1, 1, -1, 1);
                graph.SetFontSize(level + 1);
                if (last_parser_error_code != yutovo_calculator::ParserExceptionCode::None)
                    graph.Puts(mglPoint(0, 0), ErrorCodeToString(last_parser_error_code).c_str());
                else
                    graph.Puts(mglPoint(0, 0), ErrorCodeToString(last_error_code).c_str());
            }
            else
            {
                graph.Rotate(rot_x, rot_z);

                double z_min = 0, z_max = 1;
                bool has_z = false;
                for (const Plot& p : plots)
                {
                    for (double v : p.z)
                    {
                        if (std::isnan(v) || std::isinf(v))
                            continue;
                        if (!has_z)
                        {
                            z_min = z_max = v;
                            has_z = true;
                        }
                        else
                        {
                            z_min = std::min(z_min, v);
                            z_max = std::max(z_max, v);
                        }
                    }
                }
                if (!has_z || z_min == z_max)
                {
                    z_min -= 1;
                    z_max += 1;
                }

                graph.SetRanges(x_left, x_right, y_bottom, y_top, z_min, z_max);
                graph.SetFontSize(level);
                std::string f = "{" + format.color.ToRGB() + "}";
                graph.Box(std::string(f + "-2").c_str());
                graph.Axis("xyz", std::string(f + "-1").c_str(), "h-1");
                if (format.grid_width > 0)
                    graph.Grid("xyz", std::string("h" + std::to_string(format.grid_width) + f).c_str());
                graph.SetQuality(MGL_DRAW_NORM);
                for (const Plot& p : plots)
                {
                    if (p.nx > 1 && p.ny > 1 && p.z.size() == (size_t)(p.nx * p.ny))
                    {
                        mglData z_data;
                        z_data.Set(p.z.data(), (long)p.nx, (long)p.ny);
                        std::string sch;
                        switch (p.format.style)
                        {
                        case SurfaceStyle::UNIFORM:
                            sch = "{" + p.format.color.ToRGB() + "}";
                            break;
                        case SurfaceStyle::HEIGHT_MESH:
                            sch = "#";
                            break;
                        case SurfaceStyle::WIREFRAME:
                            sch = "{" + p.format.color.ToRGB() + "}-" + std::to_string(p.format.width);
                            break;
                        case SurfaceStyle::POINTS:
                            //the point size comes from the pen width digit in the scheme
                            sch = "{" + p.format.color.ToRGB() + "}." + std::to_string(std::min(9u, std::max(1u, p.format.width)));
                            break;
                        default:
                            sch = "";
                            break;
                        }
                        if (p.format.style == SurfaceStyle::WIREFRAME)
                            graph.Mesh(z_data, sch.c_str());
                        else
                            graph.Surf(z_data, sch.c_str());
                    }
                }
            }

            const unsigned char* picture = graph.GetRGBA();
            std::vector<unsigned char> arr(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
            window->DrawImage(r.left + 1, r.top + 1, r.width, r.height, arr);
        };
}

Element* GraphSurface::Clone()
{
    return new GraphSurface(*this);
}

Element* GraphSurface::Create(Element* _parent)
{
    return new GraphSurface(_parent);
}

bool GraphSurface::Remake(bool with_elements)
{
    if (document->editing)
        moving = false;

    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    int left = std::max({GetExpression()->rect.width + GetYVariable()->rect.width + 2, GetYTop()->rect.width, GetYBottom()->rect.width});
    GetShape()->rect.SetRect(0, 0, format.size.width, format.size.height);
    GetYTop()->rect.Move(left - GetYTop()->rect.width, GetShape()->rect.top);
    GetYVariable()->rect.Move(left - GetYVariable()->rect.width, GetShape()->rect.height / 2 - GetYVariable()->rect.height / 2);
    GetExpression()->rect.Move(left - GetYVariable()->rect.width - 2 - GetExpression()->rect.width, GetShape()->rect.height / 2 - GetExpression()->rect.height / 2);
    GetYBottom()->rect.Move(left - GetYBottom()->rect.width, GetShape()->rect.GetBottom() - GetYBottom()->rect.height);
    if (GetXLeft()->rect.width + GetVariable()->rect.width / 2 > GetShape()->rect.width / 2)
        GetShape()->rect.Move(left + 2 + ((GetXLeft()->rect.width + GetVariable()->rect.width / 2) - GetShape()->rect.width / 2), 0);
    else
        GetShape()->rect.Move(left + 2, 0);
    GetXLeft()->rect.Move(left, GetShape()->rect.GetBottom() + 2);
    GetVariable()->rect.Move(GetShape()->rect.left + 2 + GetShape()->rect.width / 2 - GetVariable()->rect.width / 2, GetShape()->rect.GetBottom() + 2);
    if (GetVariable()->rect.width / 2 + GetXRight()->rect.width > GetShape()->rect.width / 2 - 2)
        GetXRight()->rect.Move(GetVariable()->rect.GetRight() + 2, GetShape()->rect.GetBottom() + 2);
    else
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

void GraphSurface::MovePicture(const int dx, const int dy, bool shift)
{
    moving = true;
    if (!shift)
    {
        //drag rotates the view, the sampled data stays the same;
        rot_z -= dx * 0.5;
        rot_x -= dy * 0.5;
        return;
    }

    //pan exactly under the mouse: convert the pixel delta into data units through the current projection,
    //so the surface follows the cursor by the mouse distance in any rotation (the axes are foreshortened in 3D)
    double kx = (x_right - x_left) * dx / (GetShape()->rect.width - 40);
    double ky = (y_top - y_bottom) * dy / (GetShape()->rect.height - 40);
    {
        std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

        int w = GetShape()->rect.width;
        int h = GetShape()->rect.height;
        graph.NewFrame();
        graph.SetSize(w, h, false);
        graph.SubPlot(1, 1, 0, "#");
        graph.InPlot(0.02, 0.98, 0.02, 0.98);
        graph.Rotate(rot_x, rot_z);
        graph.SetRanges(x_left, x_right, y_bottom, y_top, -1, 1);

        //CalcXYZ unprojects through the z buffer - draw the surfaces so the buffer is filled
        for (const Plot& p : plots)
        {
            if (p.nx > 1 && p.ny > 1 && p.z.size() == (size_t)(p.nx * p.ny))
            {
                mglData z_data;
                z_data.Set(p.z.data(), (long)p.nx, (long)p.ny);
                graph.Surf(z_data, "");
            }
        }
        graph.GetRGBA(); //finalize the frame - the z buffer is filled during rasterization

        mglPoint p0 = graph.CalcXYZ(w / 2, h / 2);
        mglPoint ex = graph.CalcXYZ(w / 2 + 10, h / 2);
        mglPoint ey = graph.CalcXYZ(w / 2, h / 2 + 10);
        if (!std::isnan(p0.x) && !std::isnan(p0.y) && !std::isnan(ex.x) && !std::isnan(ex.y) && !std::isnan(ey.x) && !std::isnan(ey.y))
        {
            double sx = ((ex.x - p0.x) * dx + (ey.x - p0.x) * dy) / 10;
            double sy = ((ex.y - p0.y) * dx + (ey.y - p0.y) * dy) / 10;
            //guard against samples that missed the surface (the z buffer is empty there)
            if (std::fabs(sx) + std::fabs(sy) > 1e-9 && std::fabs(sx) < (x_right - x_left) && std::fabs(sy) < (y_top - y_bottom))
            {
                kx = sx;
                ky = sy;
            }
        }
    }

    SetNumber(x_left - kx, GetXLeft());
    SetNumber(x_right - kx, GetXRight());
    SetNumber(y_bottom - ky, GetYBottom());
    SetNumber(y_top - ky, GetYTop());
    document->AddResolveElement(logical_id);
}

void GraphSurface::ZoomPicture(const int pixels)
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

void GraphSurface::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetXLeft())
        GetXLeft()->UpdateLevel(_level + 1);
    if (GetXRight())
        GetXRight()->UpdateLevel(_level + 1);
    if (GetYBottom())
        GetYBottom()->UpdateLevel(_level + 1);
    if (GetYTop())
        GetYTop()->UpdateLevel(_level + 1);
    if (GetYVariable())
        GetYVariable()->UpdateLevel(_level + 1);
}

void GraphSurface::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Formula::ToJson(value, alloc);
    format.ToJson(value, alloc);
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& p : plots)
    {
        rapidjson::Value v;
        v.SetObject();
        p.format.ToJson(v, alloc);
        arr.PushBack(v, alloc);
    }
    value.AddMember("plots", arr, alloc);
    value.AddMember("rot_x", rot_x, alloc);
    value.AddMember("rot_z", rot_z, alloc);
}

Element* GraphSurface::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    GraphFormat f;
    f.FromJson(value, alloc);
    GraphSurface* el = parent ? new GraphSurface(parent, false) : new GraphSurface(document, false);
    el->format = f;

    if (value.HasMember("plots") && value["plots"].IsArray())
    {
        rapidjson::Value::ConstArray arr = value["plots"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
            Plot p{guid};
            if (arr[i].IsObject())
            {
                rapidjson::Value::ConstObject v = arr[i].GetObject();
                p.format.FromJson(v, alloc);
            }
            el->plots.push_back(p);
        }
    }
    if (value.HasMember("rot_x") && value["rot_x"].IsDouble())
        el->rot_x = value["rot_x"].GetDouble();
    if (value.HasMember("rot_z") && value["rot_z"].IsDouble())
        el->rot_z = value["rot_z"].GetDouble();

    return el;
}

bool GraphSurface::AfterFromJson()
{
    bool valid = elements->Count() == 8 && elements->Get(0)->type == ElementType::CODE_ROW && 
        elements->Get(1)->type == ElementType::CODE_PARAGRAPHS_BLOCK && elements->Get(2)->type == ElementType::CODE_ROW &&
        elements->Get(3)->type == ElementType::CODE_ROW && elements->Get(4)->type == ElementType::CODE_ROW &&
        elements->Get(5)->type == ElementType::CODE_ROW && elements->Get(6)->type == ElementType::CODE_ROW &&
        elements->Get(7)->type == ElementType::SHAPE;
    for (int i = 0; valid && i < elements->Get(1)->elements->Count(); ++i)
        valid = elements->Get(1)->elements->Get(i)->type == ElementType::CODE_PARAGRAPH;
    if (!valid)
        return false;

    StringFormatPtr f = GetStringFormat();
    Color color;
    uint width = 1;
    SurfaceStyle style = SurfaceStyle::HEIGHT;
    for (int i = 0; i < elements->Get(1)->elements->Count(); ++i)
    {
        CodeParagraph<>* p = (CodeParagraph<>*)elements->Get(1)->elements->Get(i).get();
        GetPlotFormat(i, color, width, style);
        p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
            f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
    }

    Init();
    last_expressions.clear();
    return true;
}

void GraphSurface::Solve()
{
    if (rect.width == 0)
        return;

    //while some of the required rows are still empty the parser string is incomplete - show the empty frame instead of a parser error
    bool ready = GetExpression()->elements->Count() > 0 && !GetVariable()->ToText().empty() && !GetYVariable()->ToText().empty() &&
        !GetXLeft()->ToText().empty() && !GetXRight()->ToText().empty() && !GetYBottom()->ToText().empty() && !GetYTop()->ToText().empty();
    for (int i = 0; ready && i < GetExpression()->elements->Count(); ++i)
        ready = !GetExpression()->elements->Get(i)->ToText().empty();
    if (!ready)
    {
        if (!last_expressions.empty() || last_error_code != yutovo_solver::ErrorCode::OK)
        {
            last_expressions.clear();
            for (Plot& p : plots)
            {
                p.z.clear();
                p.nx = p.ny = 0;
            }
            last_error_code = yutovo_solver::ErrorCode::OK;
            last_parser_error_code = yutovo_calculator::ParserExceptionCode::None;
            solving = false;
            document->RemoveErrorMarks(id);
            document->AddChangedElement(id);
        }
        return;
    }

    long nx = std::max(10L, std::min(80L, (long)(GetShape()->rect.width / 8)));
    long ny = std::max(10L, std::min(80L, (long)(GetShape()->rect.height / 8)));

    std::vector<ParserString> expressions;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        ParserString str;
        ElementPtr el = GetExpression()->elements->Get(i);
        ElementId& _id = el->id;
        str.Add(_id, U"graph_surface(");
        el->ToParserString(str);
        str.Add(_id, U",");
        GetVariable()->ToParserString(str);
        str.Add(_id, U",");
        GetYVariable()->ToParserString(str);
        str.Add(_id, U",");
        GetXLeft()->ToParserString(str);
        str.Add(_id, U",");
        GetXRight()->ToParserString(str);
        str.Add(_id, U",");
        GetYBottom()->ToParserString(str);
        str.Add(_id, U",");
        GetYTop()->ToParserString(str);
        str.Add(_id, U",");
        str.Add(_id, ToUtfString(std::to_string(nx)));
        str.Add(_id, U",");
        str.Add(_id, ToUtfString(std::to_string(ny)));
        str.Add(_id, U")");
        expressions.push_back(str);
    }

    if (last_expressions != expressions)
    {
        last_expressions = expressions;
        document->AddResolveElement(logical_id);
    }
}

void GraphSurface::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);

    //plots count must be count of the expressions
    while (plots.size() < GetExpression()->elements->Count())
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    if (plots.size() > GetExpression()->elements->Count())
        plots.resize(GetExpression()->elements->Count());

    if (solving)
    {
        for (size_t i = 0; i < plots.size(); ++i)
            document->BreakSolving(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, false);
    }

    //the template is not fully filled yet - nothing to send, the empty frame is drawn instead of the waiting symbol
    if (last_expressions.empty())
    {
        document->AddChangedElement(id);
        return;
    }

    solving = true;
    for (size_t i = 0; i < plots.size() && i < last_expressions.size(); ++i)
    {
        document->Solve(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, config,
            !GetParent(1)->visible, last_expressions[i].Text(),
            (!moving && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    }
    document->AddChangedElement(id);
}

void GraphSurface::PutResult(Result& result)
{
    solving = false;

    last_error_code = result.error.error_code;
    dependencies = result.dependencies;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expressions.clear();
        return;
    }

    auto it = std::find_if(plots.begin(), plots.end(),
        [result](const Plot& p)
        {
            return p.guid == result.guid;
        });
    if (it == plots.end())
        return;

    Plot& plot = *it;

    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        //put error message
        size_t i = std::distance(plots.begin(), it);
        last_parser_error_code = result.error.parser_error_code;
        if (i < last_expressions.size())
        {
            ElementId err_id = last_expressions[i].GetElement(result.error.pos, result.error.size);
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
    }
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

    plot.z.clear();
    plot.nx = plot.ny = 0;

    if (result.values.size() < 6)
    {
        document->Redraw(id, false);
        return;
    }

    //the first items are the bounds of the graph and the sizes of the grid
    x_left = to_double(result.values[0]);
    x_right = to_double(result.values[1]);
    y_bottom = to_double(result.values[2]);
    y_top = to_double(result.values[3]);
    plot.nx = (int)to_double(result.values[4]);
    plot.ny = (int)to_double(result.values[5]);

    //next items are z values of the grid, the x index changes faster
    for (size_t i = 6; i < result.values.size(); ++i)
        plot.z.push_back(to_double(result.values[i]));

    document->Redraw(id, false);
}

bool GraphSurface::MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    for (int i = 0; i < elements->Get(1)->elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(1)->elements->Get(i);
        if (el->elements->Count() == 0)
            return false;
        Rect r1 = el->GetAbsoluteRect();
        Rect r2 = el->elements->Get(0)->GetAbsoluteRect();
        Rect r{r1.left, r1.top, r2.left - r1.left, r2.GetBottom() - r1.top};
        if (r.IsPointInside(x, y))
        {
            mouse_l_button_pos = i;
            hold_type = MouseHoldType::PLOT_FORMAT_DIALOG;
            hold_id = id;
            return true;
        }
    }
    return false;
}

std::string GraphSurface::ToHtml() const
{
    std::string expr;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        const auto& el = GetExpression()->elements->Get(i);
        if (!el->IsVisible())
            continue;
        //the template is not solved yet - plots may be empty or shorter than the paragraph list
        std::string color = i < (int)plots.size() ? plots[i].format.color.ToHex() : default_colors[i % default_colors.size()].ToHex();
        expr += "<p><span style=\"color: " + color + ";\">█&nbsp;</span>" + el->ToHtml() + "</p>";
    }

    std::string image_base64;
    GetImage(image_base64);
    std::string s =
        "<table>"
            "<tr>"
                "<td style=\"height:100%; vertical-align:top;\">"
                    "<table style=\"height:100%;\">"
                        "<tr>"
                            "<td style=\"vertical-align:top;text-align:right;\">" + GetYTop()->ToHtml() + "</td>"
                        "</tr>"
                        "<tr>"
                            "<td style=\"vertical-align:middle;\">"
                                "<table style=\"width:100%;\">"
                                    "<tr>"
                                        "<td style=\"vertical-align:top;text-align:right;\">" + expr + "</td>"
                                        "<td style=\"vertical-align:top;text-align:left;\">" + GetYVariable()->ToHtml() + "</td>"
                                    "</tr>"
                                "</table>"
                            "</td>"
                        "</tr>"
                        "<tr>"
                            "<td style=\"vertical-align:bottom;text-align:right\">" + GetYBottom()->ToHtml() + "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
                "<td>"
                    "<img src=\"data:image/png;base64," + image_base64 + "\">"
                "</td>"
            "</tr>"
            "<tr>"
                "<td>"
                "</td>"
                "<td style=\"vertical-align:top;\">"
                    "<table style=\"width:100%;\">"
                        "<tr>"
                            "<td style=\"vertical-align:top;text-align:left;\">" + GetXLeft()->ToHtml() + "</td>"
                            "<td style=\"vertical-align:top;text-align:center\">" + GetVariable()->ToHtml() + "</td>"
                            "<td style=\"vertical-align:top;text-align:right;\">" + GetXRight()->ToHtml() + "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
            "</tr>"
       "</table>";
    return s;
}

std::u32string GraphSurface::ToText() const
{
    std::u32string s = U"graph_surface(";
    if (elements->Count() >= 8)
    {
        //the public text order stays y top, expression, y bottom, x left, x variable, x right, y variable
        s += elements->Get(0)->ToText();
        s += U",";
        s += elements->Get(1)->ToText();
        s += U",";
        s += elements->Get(3)->ToText();
        s += U",";
        s += elements->Get(4)->ToText();
        s += U",";
        s += elements->Get(5)->ToText();
        s += U",";
        s += elements->Get(6)->ToText();
        s += U",";
        s += elements->Get(2)->ToText();
    }
    else
    {
        //the element is still being built
        for (int i = 0; i < elements->Count(); ++i)
        {
            s += elements->Get(i)->ToText();
            if (i < elements->Count() - 2)
                s += U",";
        }
    }
    s += U")";
    return s;
}

void GraphSurface::ToParserString(ParserString& str)
{
    str.Add(id, U"graph_surface(");
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, U",");
    if (elements->Count() < 8)
    {
        str.Add(id, U")");
        return;
    }
    elements->Get(3)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(4)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(5)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(6)->ToParserString(str);
    str.Add(id, U",");
    elements->Get(2)->ToParserString(str);
    str.Add(id, U")");

    if (elements->Count() > 0)
    {
        int start = str.Length();
        elements->Get(0)->ToParserString(str);
        str.Annotate(id, start, str.Length());
    }
}

void GraphSurface::GetPlotFormat(const int pos, Color& color, uint& width, SurfaceStyle& style)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    color = plots[pos].format.color;
    width = plots[pos].format.width;
    style = plots[pos].format.style;
}

void GraphSurface::GetPlotFormat(const int pos, PlotFormat& format)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    format = plots[pos].format;
}

void GraphSurface::GetPlotFormat(PlotFormat& format)
{
    GetPlotFormat(mouse_l_button_pos, format);
}

void GraphSurface::SetPlotFormat(const PlotFormat& format)
{
    if (mouse_l_button_pos >= plots.size())
        return;
    plots[mouse_l_button_pos].format = format;

    ElementPtr p = elements->Get(1)->elements->Get(mouse_l_button_pos);
    if (!p)
        return;
    StringFormatPtr f = GetStringFormat();
    ((CodeParagraph<>*)p.get())->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
        f->subscript, f->superscript, format.color, f->text_bg_color, f->text_bg_selection_color));
}

CodeRow<>* GraphSurface::GetYVariable() const
{
    return (CodeRow<>*)elements->Get(2).get();
}

CodeRow<>* GraphSurface::GetYBottom() const
{
    return (CodeRow<>*)elements->Get(3).get();
}

CodeRow<>* GraphSurface::GetXLeft() const
{
    return (CodeRow<>*)elements->Get(4).get();
}

CodeRow<>* GraphSurface::GetVariable() const
{
    return (CodeRow<>*)elements->Get(5).get();
}

CodeRow<>* GraphSurface::GetXRight() const
{
    return (CodeRow<>*)elements->Get(6).get();
}

Shape* GraphSurface::GetShape() const
{
    return (Shape*)elements->Get(7).get();
}

//GraphHistogram

const std::vector<Color> GraphHistogram::default_colors = {Color::Red(), Color::Blue(), Color::Green(), Color::Magenta(), Color::Cian(), Color::Black()};

GraphHistogram::GraphHistogram(Element* _parent, bool with_init) :
    Graph(_parent, with_init)
{
    type = ElementType::GRAPH_HISTOGRAM;
    if (with_init)
        Init();
}

GraphHistogram::GraphHistogram(Document* _document, bool with_init) :
    Graph(_document, with_init)
{
    type = ElementType::GRAPH_HISTOGRAM;
    if (with_init)
        Init();
}

GraphHistogram::GraphHistogram(const GraphHistogram& source) :
    Graph(source),
    plots(source.plots)
{
    Init();
}

void GraphHistogram::Init()
{
    if (elements->Count() == 0)
    {
        elements->Add(ElementPtr(new CodeParagraphsBlock<>(this, true))); //expressions, one paragraph is one array of bars
        elements->Add(ElementPtr(new Shape(this))); //graph
    }
    GetShape()->can_resize = true;
    //the bars are placed by their indices - dragging the picture does not pan the data
    GetShape()->can_move_picture = false;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->can_merge = false;
    UpdateLevel(level);
    GetShape()->editable = false;
    editable = false;

    GetShape()->draw_func =
        [&](const Rect& r)
        {
            std::lock_guard<std::recursive_mutex> lock(mathgl_mutex);

            graph.NewFrame();
            graph.SetFlagAdv(1, MGL_NO_SCALE_REL);
            graph.SetScaleText(false);
            graph.SetSize(r.width, r.height, false);
            graph.SubPlot(1, 1, 0, "#");
            graph.InPlot(0.05, 0.95, 0.05, 0.95);
            if (r.width < 200 || r.height < 200)
                graph.SetPenDelta(0.5);
            else if (r.width < 300 || r.height < 300)
                graph.SetPenDelta(0.6);
            else if (r.width < 400 || r.height < 400)
                graph.SetPenDelta(0.7);
            else
                graph.SetPenDelta(1.);

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
                //the y axis sits at the left edge and the x axis at the zero line; the bars, stems and areas
                //grow from the zero line in both directions - the default origin is the bottom of the range
                graph.SetOrigin(x_left, 0);
                graph.SetFontSize(level);
                std::string f = "{" + format.color.ToRGB() + "}";
                graph.Axis("xy", std::string(f + "-1").c_str(), "h-1");
                if (format.grid_width > 0)
                    graph.Grid("xy", std::string("h" + std::to_string(format.grid_width) + f).c_str());
                graph.SetQuality(MGL_DRAW_NORM);
                for (const Plot& p : plots)
                {
                    auto& y = p.y;
                    if (y.empty())
                        continue;
                    //the drawn x positions must sit exactly on the integer ticks - the automatic x of MathGL spreads the points over the whole range
                    std::vector<double> x(y.size());
                    for (size_t i = 0; i < x.size(); ++i)
                        x[i] = (double)(i + 1);
                    mglData x_data(x.size());
                    x_data.Set(x);
                    mglData y_data(y.size());
                    y_data.Set(y);
                    std::string fill = "{" + p.format.color.ToRGB() + "}";
                    std::string line = fill + "-" + std::to_string(p.format.width);
                    switch (p.format.histogram_style)
                    {
                    case HistogramStyle::BARS_LINE:
                        //the line connects the bar tops
                        graph.Bars(x_data, y_data, fill.c_str());
                        graph.Plot(x_data, y_data, line.c_str());
                        break;
                    case HistogramStyle::BARS_SOLID:
                        //full bar width leaves no gaps between the bars
                        graph.SetBarWidth(1.);
                        graph.Bars(x_data, y_data, fill.c_str());
                        graph.SetBarWidth(0.7);
                        break;
                    case HistogramStyle::STEM:
                        graph.Stem(x_data, y_data, line.c_str());
                        break;
                    case HistogramStyle::AREA:
                        graph.Area(x_data, y_data, fill.c_str());
                        break;
                    case HistogramStyle::STEP:
                        graph.Step(x_data, y_data, line.c_str());
                        break;
                    case HistogramStyle::MARKS:
                        //the point size comes from the pen width digit in the scheme, no "-" so no line is drawn
                        graph.Plot(x_data, y_data, std::string(fill + "." + std::to_string(std::min(9u, std::max(1u, p.format.width)))).c_str());
                        break;
                    default:
                        graph.Bars(x_data, y_data, fill.c_str());
                        break;
                    }
                }
            }

            const unsigned char* picture = graph.GetRGBA();
            std::vector<unsigned char> arr(picture, picture + 4 * (graph.GetWidth() * graph.GetHeight()));
            window->DrawImage(r.left + 1, r.top + 1, r.width, r.height, arr);
        };
}

Element* GraphHistogram::Clone()
{
    return new GraphHistogram(*this);
}

Element* GraphHistogram::Create(Element* _parent)
{
    return new GraphHistogram(_parent);
}

void GraphHistogram::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Formula::ToJson(value, alloc);
    format.ToJson(value, alloc);
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& p : plots)
    {
        rapidjson::Value v;
        v.SetObject();
        p.format.ToJson(v, alloc);
        arr.PushBack(v, alloc);
    }
    value.AddMember("plots", arr, alloc);
}

Element* GraphHistogram::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    GraphFormat f;
    f.FromJson(value, alloc);
    GraphHistogram* el = parent ? new GraphHistogram(parent, false) : new GraphHistogram(document, false);
    el->format = f;

    if (value.HasMember("plots") && value["plots"].IsArray())
    {
        rapidjson::Value::ConstArray arr = value["plots"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
            Plot p{guid};
            if (arr[i].IsObject())
            {
                rapidjson::Value::ConstObject v = arr[i].GetObject();
                p.format.FromJson(v, alloc);
            }
            el->plots.push_back(p);
        }
    }

    return el;
}

bool GraphHistogram::AfterFromJson()
{
    bool valid = elements->Count() == 2 && elements->Get(0)->type == ElementType::CODE_PARAGRAPHS_BLOCK && elements->Get(1)->type == ElementType::SHAPE;
    for (int i = 0; valid && i < elements->Get(0)->elements->Count(); ++i)
        valid = elements->Get(0)->elements->Get(i)->type == ElementType::CODE_PARAGRAPH;
    if (!valid)
        return false;

    StringFormatPtr f = GetStringFormat();
    Color color;
    uint width = 1;
    for (int i = 0; i < elements->Get(0)->elements->Count(); ++i)
    {
        CodeParagraph<>* p = (CodeParagraph<>*)elements->Get(0)->elements->Get(i).get();
        GetPlotFormat(i, color, width);
        p->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
            f->subscript, f->superscript, color, f->text_bg_color, f->text_bg_selection_color));
    }

    Init();
    last_expressions.clear();
    return true;
}

void GraphHistogram::Solve()
{
    if (rect.width == 0)
        return;

    //while some paragraph is still empty the parser string is incomplete - show the empty frame instead of a parser error
    bool ready = GetExpression()->elements->Count() > 0;
    for (int i = 0; ready && i < GetExpression()->elements->Count(); ++i)
        ready = !GetExpression()->elements->Get(i)->ToText().empty();
    if (!ready)
    {
        if (!last_expressions.empty() || last_error_code != yutovo_solver::ErrorCode::OK)
        {
            last_expressions.clear();
            for (Plot& p : plots)
                p.y.clear();
            last_error_code = yutovo_solver::ErrorCode::OK;
            last_parser_error_code = yutovo_calculator::ParserExceptionCode::None;
            solving = false;
            document->RemoveErrorMarks(id);
            document->AddChangedElement(id);
        }
        return;
    }

    std::vector<ParserString> expressions;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        ParserString str;
        ElementPtr el = GetExpression()->elements->Get(i);
        ElementId& _id = el->id;
        str.Add(_id, U"graph_bar(");
        el->ToParserString(str);
        str.Add(_id, U")");
        expressions.push_back(str);
    }

    if (last_expressions != expressions)
    {
        last_expressions = expressions;
        document->AddResolveElement(logical_id);
    }
}

void GraphHistogram::ReSolve(bool if_error, bool force)
{
    if (if_error)
        return;

    document->RemoveErrorMarks(id);
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    assert(code);

    //plots count must be count of the expressions
    while (plots.size() < GetExpression()->elements->Count())
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    if (plots.size() > GetExpression()->elements->Count())
        plots.resize(GetExpression()->elements->Count());

    if (solving)
    {
        for (size_t i = 0; i < plots.size(); ++i)
            document->BreakSolving(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, false);
    }

    //the template is not fully filled yet - nothing to send, the empty frame is drawn instead of the waiting symbol
    if (last_expressions.empty())
    {
        document->AddChangedElement(id);
        return;
    }

    solving = true;
    for (size_t i = 0; i < plots.size() && i < last_expressions.size(); ++i)
    {
        document->Solve(GetExpression()->elements->Get(i)->logical_id, plots[i].guid, ((CodeBlock*)code.get())->code_id, config,
            !GetParent(1)->visible, last_expressions[i].Text(),
            (!moving && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    }
    document->AddChangedElement(id);
}

void GraphHistogram::PutResult(Result& result)
{
    solving = false;

    last_error_code = result.error.error_code;
    dependencies = result.dependencies;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        last_expressions.clear();
        return;
    }

    auto it = std::find_if(plots.begin(), plots.end(),
        [result](const Plot& p)
        {
            return p.guid == result.guid;
        });
    if (it == plots.end())
        return;

    Plot& plot = *it;

    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        //put error message
        size_t i = std::distance(plots.begin(), it);
        last_parser_error_code = result.error.parser_error_code;
        if (i < last_expressions.size())
        {
            ElementId err_id = last_expressions[i].GetElement(result.error.pos, result.error.size);
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
    }
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

    plot.y.clear();

    if (result.values.size() < 5)
    {
        document->Redraw(id, false);
        return;
    }

    //the values after the bounds header are the bar heights, a NaN value leaves a hole in the bars
    for (size_t i = 5; i < result.values.size(); ++i)
        plot.y.push_back(to_double(result.values[i]));

    //all series of the graph share one scale - recompute the bounds as the union of the series
    //(the per-result header bounds of the last arriving result would clip the others)
    y_bottom = 0;
    y_top = 0;
    bool found = false;
    size_t max_n = 0;
    for (const Plot& p : plots)
    {
        max_n = std::max(max_n, p.y.size());
        for (double v : p.y)
        {
            if (std::isnan(v))
                continue;
            found = true;
            y_bottom = std::min(y_bottom, v);
            y_top = std::max(y_top, v);
        }
    }
    if (!found || y_bottom == y_top)
        y_top = y_bottom + 1;
    x_left = 0.5;
    x_right = (double)max_n + 0.5;

    document->Redraw(id, false);
}

bool GraphHistogram::MouseLButtonHold(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        ElementPtr el = GetExpression()->elements->Get(i);
        if (el->elements->Count() == 0)
            return false;
        Rect r1 = el->GetAbsoluteRect();
        Rect r2 = el->elements->Get(0)->GetAbsoluteRect();
        Rect r{r1.left, r1.top, r2.left - r1.left, r2.GetBottom() - r1.top};
        if (r.IsPointInside(x, y))
        {
            mouse_l_button_pos = i;
            hold_type = MouseHoldType::PLOT_FORMAT_DIALOG;
            hold_id = id;
            return true;
        }
    }
    return false;
}

std::string GraphHistogram::ToHtml() const
{
    std::string expr;
    for (int i = 0; i < GetExpression()->elements->Count(); ++i)
    {
        const auto& el = GetExpression()->elements->Get(i);
        if (!el->IsVisible())
            continue;
        //the template is not solved yet - plots may be empty or shorter than the paragraph list
        std::string color = i < (int)plots.size() ? plots[i].format.color.ToHex() : default_colors[i % default_colors.size()].ToHex();
        expr += "<p><span style=\"color: " + color + ";\">█&nbsp;</span>" + el->ToHtml() + "</p>";
    }

    std::string image_base64;
    GetImage(image_base64);
    std::string s =
        "<table>"
            "<tr>"
                "<td style=\"height:100%; vertical-align:middle;\">" + expr + "</td>"
                "<td>"
                    "<img src=\"data:image/png;base64," + image_base64 + "\">"
                "</td>"
            "</tr>"
       "</table>";
    return s;
}

std::u32string GraphHistogram::ToText() const
{
    std::u32string s = U"graph_bar(";
    if (elements->Count() >= 2)
        s += elements->Get(0)->ToText();
    else
    {
        //the element is still being built
        for (int i = 0; i < elements->Count(); ++i)
            s += elements->Get(i)->ToText();
    }
    s += U")";
    return s;
}

void GraphHistogram::ToParserString(ParserString& str)
{
    str.Add(id, U"graph_bar(");
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, U")");

    if (elements->Count() > 0)
    {
        int start = str.Length();
        elements->Get(0)->ToParserString(str);
        str.Annotate(id, start, str.Length());
    }
}

bool GraphHistogram::Remake(bool with_elements)
{
    if (document->editing)
        moving = false;

    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    int left = GetExpression()->rect.width;
    GetShape()->rect.SetRect(0, 0, format.size.width, format.size.height);
    GetExpression()->rect.Move(left - GetExpression()->rect.width, GetShape()->rect.height / 2 - GetExpression()->rect.height / 2);
    GetShape()->rect.Move(left + 2, 0);
    baseline = GetShape()->rect.GetBottom() - GetShape()->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void GraphHistogram::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
}

void GraphHistogram::GetPlotFormat(const int pos, Color& color, uint& width)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    color = plots[pos].format.color;
    width = plots[pos].format.width;
}

void GraphHistogram::GetPlotFormat(const int pos, PlotFormat& format)
{
    while (plots.size() < pos + 1)
    {
        std::string guid = boost::uuids::to_string(boost::uuids::random_generator()());
        plots.push_back(Plot{guid, default_colors[plots.size() % default_colors.size()]});
    }
    format = plots[pos].format;
}

void GraphHistogram::GetPlotFormat(PlotFormat& format)
{
    GetPlotFormat(mouse_l_button_pos, format);
}

void GraphHistogram::SetPlotFormat(const PlotFormat& format)
{
    if (mouse_l_button_pos >= plots.size())
        return;
    plots[mouse_l_button_pos].format = format;

    ElementPtr p = GetExpression()->elements->Get(mouse_l_button_pos);
    if (!p)
        return;
    StringFormatPtr f = GetStringFormat();
    ((CodeParagraph<>*)p.get())->SetMarker(U"█", document->GetStringFormat(f->family, f->size, f->bold, f->italic, f->underline, f->strikethrough,
        f->subscript, f->superscript, format.color, f->text_bg_color, f->text_bg_selection_color));
}

CodeParagraphsBlock<>* GraphHistogram::GetExpression() const
{
    return (CodeParagraphsBlock<>*)elements->Get(0).get();
}

Shape* GraphHistogram::GetShape() const
{
    return (Shape*)elements->Get(1).get();
}

}
