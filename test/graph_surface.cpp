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

//3D surface graph
TEST_F(FormulaTest, graph_surface1)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    std::this_thread::sleep_for(100ms);
    std::string image_base64;
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    ((GraphSurface*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:top;\">"
                                    "<table style=\"height:100%;\">"
                                        "<tr>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:middle;\">"
                                                "<table style=\"width:100%;\">"
                                                    "<tr>"
                                                        "<td style=\"vertical-align:top;text-align:right;\">"
                                                            "<p>"
                                                                "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                                                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                                                    "<mrow>"
                                                                        "<mi></mi>"
                                                                    "</mrow>"
                                                                "</math>"
                                                            "</p>"
                                                        "</td>"
                                                        "<td style=\"vertical-align:top;text-align:left;\">"
                                                            "<mrow>"
                                                                "<mi>y</mi>"
                                                            "</mrow>"
                                                        "</td>"
                                                    "</tr>"
                                                "</table>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:bottom;text-align:right\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
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
                                            "<td style=\"vertical-align:top;text-align:left;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:center\">"
                                                "<mrow>"
                                                    "<mi>x</mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                    "</table>"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    ((GraphSurface*)el.get())->GetImage(image_base64);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<table>"
                            "<tr>"
                                "<td style=\"height:100%; vertical-align:top;\">"
                                    "<table style=\"height:100%;\">"
                                        "<tr>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:middle;\">"
                                                "<table style=\"width:100%;\">"
                                                    "<tr>"
                                                        "<td style=\"vertical-align:top;text-align:right;\">"
                                                            "<p>"
                                                                "<span style=\"color: #ff0000;\">█&nbsp;</span>"
                                                                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                                                                    "<mrow>"
                                                                        "<mi></mi>"
                                                                    "</mrow>"
                                                                "</math>"
                                                            "</p>"
                                                        "</td>"
                                                        "<td style=\"vertical-align:top;text-align:left;\">"
                                                            "<mrow>"
                                                                "<mi>y</mi>"
                                                            "</mrow>"
                                                        "</td>"
                                                    "</tr>"
                                                "</table>"
                                            "</td>"
                                        "</tr>"
                                        "<tr>"
                                            "<td style=\"vertical-align:bottom;text-align:right\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
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
                                            "<td style=\"vertical-align:top;text-align:left;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:center\">"
                                                "<mrow>"
                                                    "<mi>x</mi>"
                                                "</mrow>"
                                            "</td>"
                                            "<td style=\"vertical-align:top;text-align:right;\">"
                                                "<mrow>"
                                                    "<mi></mi>"
                                                "</mrow>"
                                            "</td>"
                                        "</tr>"
                                    "</table>"
                                "</td>"
                            "</tr>"
                        "</table>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, graph_surface2)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    const GraphSurface::Plot& plot = graph->plots[0];
    ASSERT_TRUE(graph->y_top == 2) << graph->y_top;
    ASSERT_TRUE(graph->y_bottom == -2) << graph->y_bottom;
    ASSERT_TRUE(graph->x_left == -4) << graph->x_left;
    ASSERT_TRUE(graph->x_right == 4) << graph->x_right;
    ASSERT_TRUE(el->elements->Get(1)->ToText() == U"x+y") << ToBasicString(el->elements->Get(1)->ToText());
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"x");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"y");
    ASSERT_TRUE(plot.nx == 50) << plot.nx;
    ASSERT_TRUE(plot.ny == 50) << plot.ny;
    ASSERT_TRUE(plot.z.size() == 2500) << plot.z.size();
    const double dx = 8. / 49;
    const double dy = 4. / 49;
    auto z_at =
        [&](int i, int j)
        {
            return (-4 + i * dx) + (-2 + j * dy);
        };
    ASSERT_TRUE(std::fabs(plot.z[0] - z_at(0, 0)) < 0.01) << plot.z[0];
    ASSERT_TRUE(std::fabs(plot.z[49] - z_at(49, 0)) < 0.01) << plot.z[49];
    ASSERT_TRUE(std::fabs(plot.z[2499] - z_at(49, 49)) < 0.01) << plot.z[2499];
    ASSERT_TRUE(std::fabs(plot.z[25 * 50 + 25] - z_at(25, 25)) < 0.01) << plot.z[25 * 50 + 25];
}

TEST_F(FormulaTest, graph_surface3)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));

    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertPlus(true);
    document.InsertString("cos", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("y", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"graph_surface(2,sin(x)+cos(y),-2,-2,x,2,y)") << ToBasicString(document.ToText());
}

//Two surfaces in one graph
TEST_F(FormulaTest, graph_surface4)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));

    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("y", true);
    document.InsertParagraph(true);
    document.InsertMinus(true);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(4s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    ASSERT_TRUE(graph->plots.size() == 2) << graph->plots.size();
    const GraphSurface::Plot& plot1 = graph->plots[0];
    const GraphSurface::Plot& plot2 = graph->plots[1];
    ASSERT_TRUE(plot1.z.size() == 2500) << plot1.z.size();
    ASSERT_TRUE(plot2.z.size() == 2500) << plot2.z.size();
    ASSERT_TRUE(std::fabs(plot1.z[0] + 6.) < 0.01) << plot1.z[0];
    ASSERT_TRUE(std::fabs(plot2.z[0] - 4.) < 0.01) << plot2.z[0];
    ASSERT_TRUE(std::fabs(plot2.z[2499] + 4.) < 0.01) << plot2.z[2499];
}

//Dragging rotates the view without recalculating the samples
TEST_F(FormulaTest, graph_surface5)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    ASSERT_TRUE(graph->plots[0].z.size() == 2500);
    std::vector<double> z_before = graph->plots[0].z;

    document.WaitTask(document.Save("graph_surface5.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(document.MouseMove(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10));
    document.MouseLButtonUp(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10);
    std::this_thread::sleep_for(1s);

    ASSERT_TRUE(std::fabs(graph->rot_x - 45.) < 0.01) << graph->rot_x;
    ASSERT_TRUE(std::fabs(graph->rot_z - 55.) < 0.01) << graph->rot_z;
    ASSERT_TRUE(graph->plots[0].z.size() == 2500);
    ASSERT_TRUE(graph->plots[0].z == z_before);
    ASSERT_TRUE(document.IsChanged()) << document.IsChanged();
}

//Shift-drag pans the ranges like in the 2D graph
TEST_F(FormulaTest, graph_surface6)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    document.WaitTask(document.Save("graph_surface7.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(document.MouseMove(r.left + r.width / 2 + 10, r.top + r.height / 2, true));
    document.MouseLButtonUp(r.left + r.width / 2 + 10, r.top + r.height / 2);
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    //the pan goes through the current 3D projection (the axes are foreshortened at the default 50/60 rotation)
    ASSERT_TRUE(el->elements->Get(4)->ToText() == U"-3.618") << ToBasicString(el->elements->Get(4)->ToText());
    ASSERT_TRUE(el->elements->Get(6)->ToText() == U"4.382") << ToBasicString(el->elements->Get(6)->ToText());
    ASSERT_TRUE(document.IsChanged()) << document.IsChanged();
}

//Zooming a graph with the mouse wheel
TEST_F(FormulaTest, graph_surface7)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.InsertString("2", true);
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    document.WaitTask(document.Save("graph_surface8.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    auto r = el->GetAbsoluteRect();
    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"1.") << ToBasicString(el->elements->Get(0)->ToText());
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-1.") << ToBasicString(el->elements->Get(3)->ToText());
    ASSERT_TRUE(el->elements->Get(4)->ToText() == U"-2.") << ToBasicString(el->elements->Get(4)->ToText());
    ASSERT_TRUE(el->elements->Get(6)->ToText() == U"2.") << ToBasicString(el->elements->Get(6)->ToText());

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.5");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(4)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(6)->ToText() == U"1.");
    ASSERT_TRUE(document.IsChanged()) << document.IsChanged();
}

//Graph format dialog: resize the graph, undo and redo
TEST_F(FormulaTest, graph_surface8)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 500;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 600;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    ASSERT_TRUE(el->rect.width == 600);

    ASSERT_TRUE(document.CanUndo());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    ASSERT_TRUE(el->rect.width == 500) << el->rect.width;

    ASSERT_TRUE(document.CanRedo());
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    el = document.FindByType(ElementId{0}, ElementType::SHAPE);
    ASSERT_TRUE(el->rect.width == 600);
}

//Surface format: color, line width and draw style
TEST_F(FormulaTest, graph_surface9)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);

    PlotFormat format;
    ASSERT_TRUE(document.GetPlotFormat(el->id, format));
    ASSERT_TRUE(format.color == Color::Red()) << format.color.ToHex();
    ASSERT_TRUE(format.width == 1);
    ASSERT_TRUE(format.style == SurfaceStyle::HEIGHT);

    format.color = Color::Blue();
    format.width = 3;
    format.style = SurfaceStyle::WIREFRAME;
    document.WaitTask(document.SetPlotFormat(el->id, format, true));
    std::this_thread::sleep_for(200ms);

    PlotFormat format2;
    ASSERT_TRUE(document.GetPlotFormat(el->id, format2));
    ASSERT_TRUE(format2.color == Color::Blue()) << format2.color.ToHex();
    ASSERT_TRUE(format2.width == 3);
    ASSERT_TRUE(format2.style == SurfaceStyle::WIREFRAME);

    GraphSurface* graph = (GraphSurface*)el.get();
    ASSERT_TRUE(graph->plots[0].format.style == SurfaceStyle::WIREFRAME);

    //the paragraph marker must switch to the new color (it is drawn through the cached marker draw format)
    ElementPtr paragraph = el->elements->Get(1)->elements->Get(0);
    ASSERT_TRUE(((Paragraph*)paragraph.get())->marker_format->text_color == Color::Blue()) <<
        ((Paragraph*)paragraph.get())->marker_format->text_color.ToHex();
    auto marker_draw_format = ((Paragraph*)paragraph.get())->marker_draw_format;
    ASSERT_TRUE(!marker_draw_format || marker_draw_format->text_color == Color::Blue()) << "marker draw format cache is stale";

    ASSERT_TRUE(document.ToHtml().find("color: #0000ff") != std::string::npos) << document.ToHtml();
}

//Error in the surface expression
TEST_F(FormulaTest, graph_surface10)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertString("zzz", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertComma(true);
    document.InsertString("y", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    ASSERT_TRUE(!graph->solving);
    ASSERT_TRUE(graph->last_error_code != yutovo_solver::ErrorCode::OK) << (int)graph->last_error_code;
    ASSERT_TRUE(graph->plots[0].z.empty());
}

//Save and load with the rotation angles
TEST_F(FormulaTest, graph_surface11)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    graph->rot_x = 30;
    graph->rot_z = 45;

    document.WaitTask(document.Save("graph_surface1.yut"));
    document.Load("graph_surface1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    graph = (GraphSurface*)el.get();
    ASSERT_TRUE(graph != nullptr);
    ASSERT_TRUE(std::fabs(graph->rot_x - 30.) < 0.01) << graph->rot_x;
    ASSERT_TRUE(std::fabs(graph->rot_z - 45.) < 0.01) << graph->rot_z;
    ASSERT_TRUE(document.ToText() == U"graph_surface(2,x+y,-2,-4,x,4,y)") << ToBasicString(document.ToText());
}

//NaN samples do not break the grid
TEST_F(FormulaTest, graph_surface12)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("sqrt", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    const GraphSurface::Plot& plot = graph->plots[0];
    ASSERT_TRUE(plot.z.size() == 2500) << plot.z.size();
    ASSERT_TRUE(std::isnan(plot.z[0])) << plot.z[0];
    ASSERT_TRUE(std::isnan(plot.z[24])) << plot.z[24];
    ASSERT_TRUE(std::fabs(plot.z[49] - 2.) < 0.01) << plot.z[49];
    ASSERT_TRUE(std::fabs(plot.z[2499] - 2.) < 0.01) << plot.z[2499];
}

//Change a user function which is used in the surface
TEST_F(FormulaTest, graph_surface13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertString("x", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertGraphSurface(true));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertPlus(true);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto el = document.FindByType({0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    const GraphSurface::Plot& plot = graph->plots[0];
    const std::vector<double> _z1{-3., -2.92, -2.84};
    ASSERT_TRUE(plot.z.size() > 3);
    ASSERT_TRUE(std::equal(plot.z.begin(), std::next(plot.z.begin(), 3), _z1.begin(),
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << plot.z[0] << plot.z[1] << plot.z[2];

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("0", true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    const std::vector<double> _z2{-21., -20.18, -19.37};
    ASSERT_TRUE(std::equal(plot.z.begin(), std::next(plot.z.begin(), 3), _z2.begin(),
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.1;
        })) << plot.z[0] << plot.z[1] << plot.z[2];
}

//A partially filled template does not solve and does not show an error
TEST_F(FormulaTest, graph_surface14)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    //only the expression is filled, the bounds are still empty
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    GraphSurface* graph = (GraphSurface*)el.get();
    ASSERT_TRUE(!graph->solving) << graph->solving;
    ASSERT_TRUE(graph->last_error_code == yutovo_solver::ErrorCode::OK) << (int)graph->last_error_code;
    ASSERT_TRUE(graph->last_expressions.empty());
    ASSERT_TRUE(graph->plots.empty() || graph->plots[0].z.empty());
    ASSERT_TRUE(document.ToText() == U"graph_surface(1,x+y,,,x,,y)") << ToBasicString(document.ToText());

    //filling the rest of the rows solves the graph
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    graph = (GraphSurface*)document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE).get();
    ASSERT_TRUE(graph->plots[0].z.size() == 2500) << graph->plots[0].z.size();
    ASSERT_TRUE(graph->last_error_code == yutovo_solver::ErrorCode::OK) << (int)graph->last_error_code;
}

//Caret traversal: right walks the fields in their visual order, left walks back
TEST_F(FormulaTest, graph_surface15)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);

    //the caret starts in the y top row; every field is identified by typing a marker character in it
    auto row_text =
        [](ElementPtr e) -> std::u32string
        {
            return e->ToText();
        };

    document.WaitTask(document.InsertString("1", true)); //y top
    ASSERT_TRUE(row_text(el->elements->Get(0)) == U"1") << ToBasicString(row_text(el->elements->Get(0)));

    document.WaitTask(document.MoveCaretRight(false)); //into the expressions block
    document.WaitTask(document.InsertString("a", true));
    ASSERT_TRUE(row_text(el->elements->Get(1)) == U"a") << ToBasicString(row_text(el->elements->Get(1)));

    document.WaitTask(document.MoveCaretRight(false)); //into the y variable row, before the prefilled "y"
    document.WaitTask(document.MoveCaretRight(false)); //after the "y"
    document.WaitTask(document.InsertString("u", true)); //append to the prefilled variable: "yu"
    ASSERT_TRUE(row_text(el->elements->Get(2)) == U"yu") << ToBasicString(row_text(el->elements->Get(2)));

    document.WaitTask(document.MoveCaretRight(false)); //into the y bottom row
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(row_text(el->elements->Get(3)) == U"2") << ToBasicString(row_text(el->elements->Get(3)));

    document.WaitTask(document.MoveCaretRight(false)); //into the x left row
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(row_text(el->elements->Get(4)) == U"3") << ToBasicString(row_text(el->elements->Get(4)));

    document.WaitTask(document.MoveCaretRight(false)); //into the x variable row, before the prefilled "x"
    document.WaitTask(document.MoveCaretRight(false)); //after the "x"
    document.WaitTask(document.InsertString("v", true)); //append to the prefilled variable: "xv"
    ASSERT_TRUE(row_text(el->elements->Get(5)) == U"xv") << ToBasicString(row_text(el->elements->Get(5)));

    document.WaitTask(document.MoveCaretRight(false)); //into the x right row
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(row_text(el->elements->Get(6)) == U"4") << ToBasicString(row_text(el->elements->Get(6)));

    //walk back with the left arrow: the fields are visited in the reverse order
    auto state =
        [&]() -> std::string
        {
            return document.GetEditorState().ToString();
        };
    auto left =
        [&](int count)
        {
            for (int i = 0; i < count; ++i)
                document.WaitTask(document.MoveCaretLeft(false));
        };

    left(5); //through the x variable row into the x left row, at its end
    ASSERT_TRUE(state() == "0,0,0,0,0,0,0,4,0,1 []") << state();
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(row_text(el->elements->Get(4)) == U"35") << ToBasicString(document.ToText());

    left(3); //into the y bottom row, at its end
    ASSERT_TRUE(state() == "0,0,0,0,0,0,0,3,0,1 []") << state();
    document.WaitTask(document.InsertString("6", true));
    ASSERT_TRUE(row_text(el->elements->Get(3)) == U"26") << ToBasicString(row_text(el->elements->Get(3)));

    left(6); //through the y variable row into the expressions block, at its end
    ASSERT_TRUE(state() == "0,0,0,0,0,0,0,1,0,0,0,1 []") << state();
    document.WaitTask(document.InsertString("b", true));
    ASSERT_TRUE(row_text(el->elements->Get(1)) == U"ab") << ToBasicString(row_text(el->elements->Get(1)));

    left(3); //into the y top row, at its end
    ASSERT_TRUE(state() == "0,0,0,0,0,0,0,0,0,1 []") << state();
    document.WaitTask(document.InsertString("7", true));
    ASSERT_TRUE(row_text(el->elements->Get(0)) == U"17") << ToBasicString(row_text(el->elements->Get(0)));

    ASSERT_TRUE(document.ToText() == U"graph_surface(17,ab,26,35,xv,4,yu)") << ToBasicString(document.ToText());
}

//Dragging and zooming the graph with the mouse marks the document changed
TEST_F(FormulaTest, graph_surface16)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    document.WaitTask(document.Save("graph_surface2.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    //rotate the view with a drag
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(document.MouseMove(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10));
    document.MouseLButtonUp(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10);
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.IsChanged());

    document.WaitTask(document.Save("graph_surface2.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    //zoom with the wheel
    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.IsChanged());
}

//A loaded document (no undoable edits) becomes changed after a mouse drag or wheel zoom
TEST_F(FormulaTest, graph_surface17)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    document.WaitTask(document.Save("graph_surface6.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged());
    document.Load("graph_surface6.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    //rotate the view with a drag - the loaded document must become changed
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(document.MouseMove(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10));
    document.MouseLButtonUp(r.left + r.width / 2 + 10, r.top + r.height / 2 + 10);
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.IsChanged()) << document.IsChanged();

    document.WaitTask(document.Save("graph_surface6.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged());

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.IsChanged());
}

//Shift-drag pans the surface under the mouse through the current projection
TEST_F(FormulaTest, graph_surface18)
{
    Start(600);

    document.WaitTask(document.InsertGraphSurface(true));
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);

    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("y", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.InsertMinus(true);
    document.WaitTask(document.InsertString("1", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    auto row_value =
        [](ElementPtr e)
        {
            return std::stod(ToBasicString(e->ToText()));
        };
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_SURFACE);
    double x_left0 = row_value(el->elements->Get(4));
    double x_right0 = row_value(el->elements->Get(6));
    double y_bottom0 = row_value(el->elements->Get(3));
    double y_top0 = row_value(el->elements->Get(0));

    document.WaitTask(document.Save("graph_surface9.yut"));
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(!document.IsChanged()) << document.IsChanged();

    //drag right: the surface must follow the mouse, the ranges shift by equal offsets
    auto r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(document.MouseMove(r.left + r.width / 2 + 40, r.top + r.height / 2, true));
    document.MouseLButtonUp(r.left + r.width / 2 + 40, r.top + r.height / 2);
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    double kx = row_value(el->elements->Get(4)) - x_left0;
    double kx2 = row_value(el->elements->Get(6)) - x_right0;
    double ky = row_value(el->elements->Get(3)) - y_bottom0;
    double ky2 = row_value(el->elements->Get(0)) - y_top0;
    ASSERT_TRUE(std::fabs(kx - kx2) < 0.01) << kx << " " << kx2;
    ASSERT_TRUE(std::fabs(ky - ky2) < 0.01) << ky << " " << ky2;
    //the offset must be noticeable (the drag is 40 pixels over a range of 2);
    //its sign follows the current projection of the x axis on the screen
    ASSERT_TRUE(std::fabs(kx) > 0.1 && std::fabs(kx) < 1.9) << kx;
    ASSERT_TRUE(std::fabs(ky) > 0.1 && std::fabs(ky) < 1.9) << ky;
    ASSERT_TRUE(document.IsChanged()) << document.IsChanged();
}

}
