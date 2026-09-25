/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <fstream>
#include <iterator>
#include <cstring>
#include "mock.h"
#include "style.h"
#include "paragraph.h"
#include "formulas/graph.h"
#include <atomic>
#include <thread>

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Histogram graph over an array
TEST_F(FormulaTest, histogram1)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    std::this_thread::sleep_for(100ms);
    std::string image_base64;
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ((GraphHistogram*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:middle;\">"
                                    "<p>"
                                        "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                        "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                            "<mrow>"
                                                "<mi></mi>"
                                            "</mrow>"
                                        "</math>"
                                    "</p>"
                                "</td>"
                                "<td>"
                                    "<img src=\"data:image/png;base64," + image_base64 + "\">"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ((GraphHistogram*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:middle;\">"
                                    "<p>"
                                        "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                        "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                            "<mrow>"
                                                "<mi></mi>"
                                            "</mrow>"
                                        "</math>"
                                    "</p>"
                                "</td>"
                                "<td>"
                                    "<img src=\"data:image/png;base64," + image_base64 + "\">"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, histogram2)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == U"graph_bar([1,5,3,2])") << ToBasicString(document.ToText());
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    const GraphHistogram::Plot& plot = graph->plots[0];
    ASSERT_TRUE(plot.y.size() == 4) << plot.y.size();
    ASSERT_TRUE(std::fabs(plot.y[0] - 1.) < 0.01) << plot.y[0];
    ASSERT_TRUE(std::fabs(plot.y[1] - 5.) < 0.01) << plot.y[1];
    ASSERT_TRUE(std::fabs(plot.y[2] - 3.) < 0.01) << plot.y[2];
    ASSERT_TRUE(std::fabs(plot.y[3] - 2.) < 0.01) << plot.y[3];
    ASSERT_TRUE(std::fabs(graph->x_left - 0.5) < 0.01) << graph->x_left;
    ASSERT_TRUE(std::fabs(graph->x_right - 4.5) < 0.01) << graph->x_right;
    ASSERT_TRUE(std::fabs(graph->y_bottom - 0.) < 0.01) << graph->y_bottom;
    ASSERT_TRUE(std::fabs(graph->y_top - 5.) < 0.01) << graph->y_top;
}

TEST_F(FormulaTest, histogram3)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertMinus(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("3", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == U"graph_bar([-2,3])") << ToBasicString(document.ToText());
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    const GraphHistogram::Plot& plot = graph->plots[0];
    ASSERT_TRUE(plot.y.size() == 2) << plot.y.size();
    ASSERT_TRUE(std::fabs(plot.y[0] + 2.) < 0.01) << plot.y[0];
    ASSERT_TRUE(std::fabs(plot.y[1] - 3.) < 0.01) << plot.y[1];
    ASSERT_TRUE(std::fabs(graph->y_bottom + 2.) < 0.01) << graph->y_bottom;
    ASSERT_TRUE(std::fabs(graph->y_top - 3.) < 0.01) << graph->y_top;
}

//Two bar series in one graph
TEST_F(FormulaTest, histogram4)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    ASSERT_TRUE(document.ToText() == U"graph_bar([1,2]\n[3,4])") << ToBasicString(document.ToText());
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(graph->plots.size() == 2) << graph->plots.size();
    ASSERT_TRUE(graph->plots[0].y.size() == 2) << graph->plots[0].y.size();
    ASSERT_TRUE(graph->plots[1].y.size() == 2) << graph->plots[1].y.size();
    ASSERT_TRUE(std::fabs(graph->plots[0].y[1] - 2.) < 0.01) << graph->plots[0].y[1];
    ASSERT_TRUE(std::fabs(graph->plots[1].y[0] - 3.) < 0.01) << graph->plots[1].y[0];
    ASSERT_TRUE(std::fabs(graph->plots[1].y[1] - 4.) < 0.01) << graph->plots[1].y[1];
}

//A NaN element leaves a hole in the bars
TEST_F(FormulaTest, histogram5)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("sqrt", true);
    document.InsertOpenRoundBracket(true);
    document.InsertMinus(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    const GraphHistogram::Plot& plot = graph->plots[0];
    ASSERT_TRUE(plot.y.size() == 2) << plot.y.size();
    ASSERT_TRUE(std::isnan(plot.y[0])) << plot.y[0];
    ASSERT_TRUE(std::fabs(plot.y[1] - 1.) < 0.01) << plot.y[1];
    ASSERT_TRUE(std::fabs(graph->y_bottom - 0.) < 0.01) << graph->y_bottom;
    ASSERT_TRUE(std::fabs(graph->y_top - 1.) < 0.01) << graph->y_top;
}

//An unknown identifier shows the error instead of the bars
TEST_F(FormulaTest, histogram6)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.WaitTask(document.InsertString("y", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(graph->last_error_code != yutovo_solver::ErrorCode::OK) << (int)graph->last_error_code;
    ASSERT_TRUE(graph->plots[0].y.empty()) << graph->plots[0].y.size();
}

//Editing the array assignment re-solves the graph
TEST_F(FormulaTest, histogram7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("3", true));
    document.InsertCloseSquareBracket(true);
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertGraphHistogram(true));
    document.WaitTask(document.InsertString("a", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(graph->plots.size() == 1) << graph->plots.size();
    ASSERT_TRUE(graph->plots[0].y.size() == 3) << graph->plots[0].y.size();
    ASSERT_TRUE(std::fabs(graph->plots[0].y[2] - 3.) < 0.01) << graph->plots[0].y[2];

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("0", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == U"a=[1,2,30]\ngraph_bar(a)") << ToBasicString(document.ToText());
    ASSERT_TRUE(graph->plots[0].y.size() == 3) << graph->plots[0].y.size();
    ASSERT_TRUE(std::fabs(graph->plots[0].y[2] - 30.) < 0.01) << graph->plots[0].y[2];
}

//The marker opens the plot format dialog
TEST_F(FormulaTest, histogram8)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    auto p1 = el->elements->Get(0)->elements->Get(0);
    Rect r = p1->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + 5, r.top + 5, hold_type, id));
    ASSERT_TRUE(hold_type == MouseHoldType::PLOT_FORMAT_DIALOG && id == el->id);

    PlotFormat format;
    ASSERT_TRUE(document.GetPlotFormat(id, format));
    format.width = 2;
    format.color = Color::Blue();
    document.WaitTask(document.SetPlotFormat(id, format, true));

    format = PlotFormat{};
    ASSERT_TRUE(document.GetPlotFormat(id, format));
    ASSERT_TRUE(format.width == 2 && format.color == Color::Blue());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.GetPlotFormat(id, format));
    ASSERT_TRUE(format.width == 1 && format.color == Color::Red());
}

//Graph format and the image export
TEST_F(FormulaTest, histogram9)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.size.width == 400 && format.size.height == 400);
    format.size.width = 500;
    format.grid_width = 2;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.size.width == 500 && format.grid_width == 2);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.size.width == 400 && format.grid_width == 1);

    std::vector<unsigned char> png;
    ASSERT_TRUE(document.GetGraphImage(el->id, png));
    ASSERT_TRUE(png.size() > 8 && png[0] == 0x89 && png[1] == 'P' && png[2] == 'N' && png[3] == 'G') << png.size();
}

//Save and load
TEST_F(FormulaTest, histogram10)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    PlotFormat format;
    document.GetPlotFormat(el->id, format);
    format.color = Color::Blue();
    document.WaitTask(document.SetPlotFormat(el->id, format, true));
    std::this_thread::sleep_for(500ms);

    document.WaitTask(document.Save("graph_histogram10.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    document.Load("graph_histogram10.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(3s);

    ASSERT_TRUE(document.ToText() == U"graph_bar([1,2])") << ToBasicString(document.ToText());
    el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ASSERT_TRUE(el != nullptr);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(graph->plots.size() == 1) << graph->plots.size();
    document.GetPlotFormat(el->id, format);
    ASSERT_TRUE(format.color == Color::Blue());
}

//The empty template does not solve and shows no error
TEST_F(FormulaTest, histogram11)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    std::this_thread::sleep_for(1s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(!graph->solving) << graph->solving;
    ASSERT_TRUE(graph->last_error_code == yutovo_solver::ErrorCode::OK) << (int)graph->last_error_code;
    //one plot is lazily created for the paragraph marker color, it stays without data
    ASSERT_TRUE(graph->plots.size() == 1 && graph->plots[0].y.empty()) << graph->plots.size();
    ASSERT_TRUE(document.ToText() == U"graph_bar()") << ToBasicString(document.ToText());
}

//Dragging or zooming the discrete bars does nothing
TEST_F(FormulaTest, histogram12)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    std::vector<double> y_before = graph->plots[0].y;

    document.WaitTask(document.Save("graph_histogram12.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(!document.MouseMove(r.left + r.width / 2 + 20, r.top + r.height / 2 + 20));
    document.MouseLButtonUp(r.left + r.width / 2 + 20, r.top + r.height / 2 + 20);
    ASSERT_TRUE(!document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point(0, 120), Point()));
    std::this_thread::sleep_for(1s);

    ASSERT_TRUE(graph->plots[0].y == y_before);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();
}

//Negative values draw bars below the zero line
TEST_F(FormulaTest, histogram13)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.InsertComma(true);
    document.WaitTask(document.InsertString("3", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    std::string image_base64;
    graph->GetImage(image_base64); //draws the frame into graph->graph

    const int w = graph->graph.GetWidth();
    const int h = graph->graph.GetHeight();
    const unsigned char* pic = graph->graph.GetRGBA();
    ASSERT_TRUE(pic != nullptr && w == 400 && h == 400) << w << "x" << h;

    //the plot area is InPlot(0.05, 0.95, 0.05, 0.95), the y range is [-2, 3]:
    //the zero line sits at row 0.95*h - (0 - y_bottom)/(y_top - y_bottom) * 0.9*h
    const double zero_row = 0.95 * h - 2. / 5. * 0.9 * h;
    int red_above = 0, red_below = 0;
    for (int y = (int)(0.05 * h) + 2; y < (int)(0.95 * h) - 2; ++y)
    {
        for (int x = (int)(0.05 * w) + 2; x < (int)(0.95 * w) - 2; ++x)
        {
            const unsigned char* p = pic + 4 * (y * w + x);
            if (p[0] > 200 && p[1] < 100 && p[2] < 100)
            {
                if (y < zero_row)
                    ++red_above;
                else
                    ++red_below;
            }
        }
    }
    //the bar of 3 grows above the zero line, the bar of -2 must grow below it
    ASSERT_TRUE(red_above > 500) << red_above;
    ASSERT_TRUE(red_below > 500) << red_below;

    //the positive bar must not extend below the zero line: its column x=2 is at pixel
    //0.05*w + (2 - 0.5)/2 * 0.9*w, and the strip right below the zero line must stay empty there
    int red_under_positive = 0;
    const int positive_x = (int)(0.05 * w + 1.5 / 2. * 0.9 * w);
    for (int y = (int)zero_row + 8; y < (int)zero_row + 48; ++y)
    {
        for (int x = positive_x - 20; x < positive_x + 20; ++x)
        {
            const unsigned char* p = pic + 4 * (y * w + x);
            if (p[0] > 200 && p[1] < 100 && p[2] < 100)
                ++red_under_positive;
        }
    }
    ASSERT_TRUE(red_under_positive == 0) << red_under_positive;
}

//An all-negative array draws bars from the values up to the zero line at the top of the range
TEST_F(FormulaTest, histogram14)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.InsertComma(true);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("3", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(std::fabs(graph->y_bottom + 3.) < 0.01) << graph->y_bottom;
    ASSERT_TRUE(std::fabs(graph->y_top - 0.) < 0.01) << graph->y_top;

    std::string image_base64;
    graph->GetImage(image_base64);

    const int w = graph->graph.GetWidth();
    const int h = graph->graph.GetHeight();
    const unsigned char* pic = graph->graph.GetRGBA();
    ASSERT_TRUE(pic != nullptr && w == 400 && h == 400) << w << "x" << h;

    int red = 0;
    for (int y = (int)(0.05 * h) + 2; y < (int)(0.95 * h) - 2; ++y)
    {
        for (int x = (int)(0.05 * w) + 2; x < (int)(0.95 * w) - 2; ++x)
        {
            const unsigned char* p = pic + 4 * (y * w + x);
            if (p[0] > 200 && p[1] < 100 && p[2] < 100)
                ++red;
        }
    }
    //both bars grow from the zero line at the top of the range down to their values
    ASSERT_TRUE(red > 50000) << red;
}

//The plot styles are applied and restored on load
TEST_F(FormulaTest, histogram15)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    PlotFormat format;
    ASSERT_TRUE(document.GetPlotFormat(el->id, format));
    format.histogram_style = HistogramStyle::STEM;
    format.color = Color::Blue();
    format.width = 2;
    document.WaitTask(document.SetPlotFormat(el->id, format, true));
    std::this_thread::sleep_for(500ms);

    format = PlotFormat{};
    ASSERT_TRUE(document.GetPlotFormat(el->id, format));
    ASSERT_TRUE(format.histogram_style == HistogramStyle::STEM) << (int)format.histogram_style;
    ASSERT_TRUE(format.color == Color::Blue());
    ASSERT_TRUE(format.width == 2);

    //the styled graph draws without errors and survives a save/load roundtrip
    std::string image_base64;
    ((GraphHistogram*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(!image_base64.empty());

    document.WaitTask(document.Save("graph_histogram15.yut"));
    std::this_thread::sleep_for(500ms);
    document.Load("graph_histogram15.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ASSERT_TRUE(el != nullptr);
    ASSERT_TRUE(document.GetPlotFormat(el->id, format));
    ASSERT_TRUE(format.histogram_style == HistogramStyle::STEM) << (int)format.histogram_style;
    ASSERT_TRUE(format.color == Color::Blue());
    ASSERT_TRUE(format.width == 2);
}

//The graph format color paints only the grid and the axes - the bar series keep their own colors
TEST_F(FormulaTest, histogram16)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(graph->plots[0].format.color == Color::Red());

    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.color = Color::Green();
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    std::this_thread::sleep_for(500ms);

    //the bar series and its marker keep their own plot format color
    ASSERT_TRUE(graph->plots[0].format.color == Color::Red());
    PlotFormat plot_format;
    ASSERT_TRUE(document.GetPlotFormat(el->id, plot_format));
    ASSERT_TRUE(plot_format.color == Color::Red());
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.color == Color::Green());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    //undo replaces the element - resolve it again before checking
    el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.color == Color::Black());
}

//All series of one graph share a single scale - the union of their bounds
TEST_F(FormulaTest, histogram17)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.WaitTask(document.InsertString("8", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertOpenSquareBracket(true);
    document.InsertString("4.4", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    GraphHistogram* graph = (GraphHistogram*)el.get();
    ASSERT_TRUE(graph->plots.size() == 2) << graph->plots.size();
    ASSERT_TRUE(graph->plots[0].y.size() == 1 && std::fabs(graph->plots[0].y[0] - 8.) < 0.01) << graph->plots[0].y.size();
    ASSERT_TRUE(graph->plots[1].y.size() == 1 && std::fabs(graph->plots[1].y[0] - 4.4) < 0.01) << graph->plots[1].y.size();
    //the smaller series must not stretch to the full height - the shared scale covers 0..8
    ASSERT_TRUE(std::fabs(graph->y_top - 8.) < 0.01) << graph->y_top;
    ASSERT_TRUE(std::fabs(graph->y_bottom - 0.) < 0.01) << graph->y_bottom;
    //both series have one element, so they share the single bar position x=1
    ASSERT_TRUE(std::fabs(graph->x_right - 1.5) < 0.01) << graph->x_right;
}

//The stems sit exactly on the integer ticks and the y axis stays at the left edge
TEST_F(FormulaTest, histogram18)
{
    Start(600);

    document.WaitTask(document.InsertGraphHistogram(true));
    document.InsertOpenSquareBracket(true);
    document.WaitTask(document.InsertString("1", true));
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    PlotFormat plot_format;
    ASSERT_TRUE(document.GetPlotFormat(el->id, plot_format));
    plot_format.histogram_style = HistogramStyle::STEM;
    document.WaitTask(document.SetPlotFormat(el->id, plot_format, true));
    GraphFormat graph_format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, graph_format));
    graph_format.grid_width = 0;
    document.WaitTask(document.SetGraphFormat(el->id, graph_format, true));
    std::this_thread::sleep_for(500ms);

    GraphHistogram* graph = (GraphHistogram*)el.get();
    std::string image_base64;
    graph->GetImage(image_base64);
    const int w = graph->graph.GetWidth();
    const int h = graph->graph.GetHeight();
    const unsigned char* pic = graph->graph.GetRGBA();
    ASSERT_TRUE(pic != nullptr && w == 400 && h == 400) << w << "x" << h;

    auto red_in =
        [&](int x_from, int x_to)
        {
            int count = 0;
            for (int y = (int)(0.05 * h) + 2; y < (int)(0.95 * h) - 2; ++y)
                for (int x = x_from; x < x_to; ++x)
                {
                    const unsigned char* p = pic + 4 * (y * w + x);
                    if (p[0] > 200 && p[1] < 100 && p[2] < 100)
                        ++count;
                }
            return count;
        };
    //the ticks 1 and 2 sit at 0.05*w + 0.5/2*0.9*w and 0.05*w + 1.5/2*0.9*w - the stems must grow there
    const int tick1 = (int)(0.05 * w + 0.5 / 2. * 0.9 * w);
    const int tick2 = (int)(0.05 * w + 1.5 / 2. * 0.9 * w);
    //the stem of the value 1 spans half of the plot height (~180 rows), the one of 2 the full height
    ASSERT_TRUE(red_in(tick1 - 10, tick1 + 10) > 100) << red_in(tick1 - 10, tick1 + 10);
    ASSERT_TRUE(red_in(tick2 - 10, tick2 + 10) > 200) << red_in(tick2 - 10, tick2 + 10);
    //the automatic x of MathGL would spread the points over the whole range - no stems at the edges
    ASSERT_TRUE(red_in((int)(0.05 * w), (int)(0.05 * w) + 40) == 0) << red_in((int)(0.05 * w), (int)(0.05 * w) + 40);
    ASSERT_TRUE(red_in((int)(0.95 * w) - 40, (int)(0.95 * w)) == 0) << red_in((int)(0.95 * w) - 40, (int)(0.95 * w));

    //the y axis is a vertical black line at the left edge, the middle of the plot must stay empty
    //(the bottom rows are excluded - the x axis tick marks stick up from the axis line)
    auto dark_in =
        [&](int x_from, int x_to, int y_to)
        {
            int count = 0;
            for (int y = (int)(0.05 * h) + 2; y < y_to; ++y)
                for (int x = x_from; x < x_to; ++x)
                {
                    const unsigned char* p = pic + 4 * (y * w + x);
                    if (p[0] < 80 && p[1] < 80 && p[2] < 80)
                        ++count;
                }
            return count;
        };
    ASSERT_TRUE(dark_in((int)(0.05 * w) - 2, (int)(0.05 * w) + 4, (int)(0.95 * h) - 2) > 100) <<
        dark_in((int)(0.05 * w) - 2, (int)(0.05 * w) + 4, (int)(0.95 * h) - 2);
    ASSERT_TRUE(dark_in(w / 2 - 15, w / 2 + 15, (int)(0.95 * h) - 40) == 0) <<
        dark_in(w / 2 - 15, w / 2 + 15, (int)(0.95 * h) - 40);
}

//The interim "style" values 8..14 of the interim builds migrate into histogram_style on load
TEST_F(FormulaTest, histogram19)
{
    Start(600);

    auto json =         "{\"string_formats\":[{\"id\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\",\"family\":\"Arial\",\"size\":14,\"bold\":false,\"itali" \
        "c\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"89c69148-d1bc-4384-b009-7b7" \
        "52eab898c\",\"family\":\"Courier New\",\"size\":14,\"bold\":false,\"italic\":false,\"underline\":false,\"color\":4278190080,\"" \
        "selection_color\":4294967295}],\"paragraph_formats\":[{\"name\":\"Text body\",\"alignment\":0,\"word_wrap\":1,\"line_spaci" \
        "ng\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"defau" \
        "lt_string_format\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\"},{\"name\":\"Code\",\"alignment\":0,\"word_wrap\":0,\"line_spa" \
        "cing\":2,\"indent_before\":2,\"indent_after\":2,\"indent_first_line\":0,\"spacing_before\":2,\"spacing_after\":2,\"default" \
        "_string_format\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}],\"text\":{\"id\":\"0\",\"type\":1,\"elements\":[{\"id\":\"0,0\",\"ty" \
        "pe\":2,\"level\":1,\"format_name\":\"Text body\",\"format_alignment\":0,\"elements\":[{\"id\":\"0,0,0\",\"type\":3,\"level\":1,\"f" \
        "ormat_name\":\"Text body\",\"elements\":[{\"id\":\"0,0,0,0\",\"type\":5,\"level\":1,\"code_id\":1,\"elements\":[{\"id\":\"0,0,0,0," \
        "0\",\"type\":6,\"level\":1,\"format_name\":\"Code\",\"format_alignment\":0,\"elements\":[{\"id\":\"0,0,0,0,0,0\",\"type\":7,\"leve" \
        "l\":1,\"elements\":[{\"id\":\"0,0,0,0,0,0,0\",\"type\":59,\"level\":1,\"graph_format\":{\"width\":400,\"height\":400,\"color\":42" \
        "78190080,\"grid_width\":1},\"plots\":[{\"plot_format\":{\"width\":1,\"color\":4278190335,\"style\":11}}],\"elements\":[{\"id\"" \
        ":\"0,0,0,0,0,0,0,0\",\"type\":44,\"level\":1,\"elements\":[{\"id\":\"0,0,0,0,0,0,0,0,0\",\"type\":6,\"level\":1,\"format_name\":" \
        "\"Code\",\"format_alignment\":0,\"marker\":\"\u2588\",\"marker_format_id\":\"89c69148-d1bc-4384-b009-7b752eab898c\",\"eleme" \
        "nts\":[{\"id\":\"0,0,0,0,0,0,0,0,0,0\",\"type\":7,\"level\":1,\"elements\":[{\"id\":\"0,0,0,0,0,0,0,0,0,0,0\",\"type\":8,\"level" \
        "\":1,\"elements\":\"1\",\"format_id\":\"89c69148-d1bc-4384-b009-7b752eab898c\",\"can_merge\":true}]}]}]},{\"id\":\"0,0,0,0,0" \
        ",0,0,1\",\"type\":10,\"level\":1}]}]}]}]}]}]}]}}";

    document.WaitTask(document.LoadJson(json, 0));
    std::this_thread::sleep_for(500ms);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ASSERT_TRUE(el != nullptr);
    PlotFormat format;
    ASSERT_TRUE(document.GetPlotFormat(el->id, format));
    //style 11 of the interim encoding is STEM (11 - 8)
    ASSERT_TRUE(format.histogram_style == HistogramStyle::STEM) << (int)format.histogram_style;
    ASSERT_TRUE(format.color == Color::Blue()) << format.color.ToInt();

    //a save/load roundtrip writes the dedicated key and keeps the style
    document.WaitTask(document.Save("graph_histogram19.yut"));
    std::this_thread::sleep_for(500ms);
    document.Load("graph_histogram19.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);
    el = document.FindByType(ElementId{0}, ElementType::GRAPH_HISTOGRAM);
    ASSERT_TRUE(el != nullptr);
    ASSERT_TRUE(document.GetPlotFormat(el->id, format));
    ASSERT_TRUE(format.histogram_style == HistogramStyle::STEM) << (int)format.histogram_style;
}

}
