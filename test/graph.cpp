/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/graph.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//2D graph
TEST_F(FormulaTest, graphs1)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
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
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, graphs2)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>Null</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mo>-</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mo>-</mo>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mrow>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    auto el = document.FindByType({0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    ASSERT_TRUE(graph->y_bottom == -2) << graph->y_bottom;
    ASSERT_TRUE(el->elements->Get(1)->ToText() == U"x+5");
    ASSERT_TRUE(graph->y_top == 2) << graph->y_top;
    ASSERT_TRUE(graph->x_left == -4);
    ASSERT_TRUE(el->elements->Get(4)->ToText() == U"x");
    ASSERT_TRUE(graph->x_right == 4);
    const std::vector<double> _x{-4., -3.98, -3.96};
    std::vector<double> x(graph->x.begin(), std::next(graph->x.begin(), 3));
    ASSERT_TRUE(std::equal(x.begin(), x.end(), _x.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << x[0] << x[1] << x[2];
    const std::vector<double> _y{1., 1.02, 1.04};
    std::vector<double> y(graph->y.begin(), std::next(graph->y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

TEST_F(FormulaTest, graphs3)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"graph_line(2,sin(x),-2,-2,x,2)") << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(1s);
    auto el = document.FindByType({0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    ASSERT_TRUE(!graph->x.empty());
    ASSERT_TRUE(!graph->y.empty());
}

TEST_F(FormulaTest, graphs4)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("x", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,(x)/(x),,,,)") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"graph_line(,xx,,,,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,(x)/(x),,,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,xx,,,,)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 1, 0})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, graphs5)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(100ms);
    
    document.MoveCaretRight(false);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToText() == U"graph_line(,sin((x)/()),,,,)") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"graph_line(,sin(x),,,,)") << ToBasicString(document.ToText());
}

//Set graph format
TEST_F(FormulaTest, graphs6)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
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

//Change user function which is used in the graph
TEST_F(FormulaTest, graphs7)
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
    document.WaitTask(document.InsertGraph(true));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    auto el = document.FindByType({0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    const std::vector<double> _y1{-2., -1.99, -1.98};
    std::vector<double> y(graph->y.begin(), std::next(graph->y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y1.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("0", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    const std::vector<double> _y2{-20., -19.9, -19.8};
    y = std::vector<double>(graph->y.begin(), std::next(graph->y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y2.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

//Copy-paste a graph after changing its format
TEST_F(FormulaTest, graphs8)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();

    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 500;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    el = document.FindByType(ElementId{0, 0, 1, 0}, ElementType::GRAPH_LINE);
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    ASSERT_TRUE(format.size.width == 500);
    const std::vector<double> _y{-1., -0.996, -0.992};
    GraphLine* graph = (GraphLine*)el.get();
    std::vector<double> y(graph->y.begin(), std::next(graph->y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

//Check error marks
TEST_F(FormulaTest, graphs9)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(el->id, start, size)) << ErrorMarks();

    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertDivision(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.HasErrorMark(el->elements->Get(0)->id, start, size)) << ErrorMarks();

    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.HasErrorMark(el->elements->Get(1)->id, start, size)) << ErrorMarks();
}

//Check redrawing graph after undo
TEST_F(FormulaTest, graphs10)
{
    Start(600);

    document.InsertString("Graph:", true);
    document.WaitTask(document.InsertParagraph(true));

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("1", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertParagraph(true));

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();

    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    const std::vector<double> _y{-1., -0.996, -0.992};
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    std::vector<double> y(graph->y.begin(), std::next(graph->y.begin(), 3));
    ASSERT_TRUE(std::equal(y.begin(), y.end(), _y.begin(), 
        [](double x, double y)
        {
            return std::fabs(x - y) < 0.01;
        })) << y[0] << y[1] << y[2];
}

//Zooming a graph
TEST_F(FormulaTest, graphs11)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    auto r = el->GetAbsoluteRect();
    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"1.") << ToBasicString(el->elements->Get(0)->ToText());
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-1.") << ToBasicString(el->elements->Get(2)->ToText());
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"1.");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.5");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"0.5");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, 15}, Point{0, 15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.25");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-0.25");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.25");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"0.25");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, -15}, Point{0, -15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"0.5");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-0.5");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"0.5");

    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, -15}, Point{0, -15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"1.");
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-1.");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"1.");
}

//Zooming a graph
TEST_F(FormulaTest, graphs12)
{
    Start(600);

    document.WaitTask(document.InsertGraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphFormat format;
    ASSERT_TRUE(document.GetGraphFormat(el->id, format));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));
    format.size.width = 400;
    document.WaitTask(document.SetGraphFormat(el->id, format, true));

    document.InsertString("2000", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2000", true);
    document.MoveCaretRight(false);
    document.InsertMinus(false);
    document.InsertString("2000", true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("2000", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    auto r = el->GetAbsoluteRect();
    ASSERT_TRUE(document.MouseWheel(r.left + r.width / 2, r.top + r.height / 2, Point{0, -15}, Point{0, -15}));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(el->elements->Get(0)->ToText() == U"4.*pow(10,3)") << ToBasicString(el->elements->Get(0)->ToText());
    ASSERT_TRUE(el->elements->Get(2)->ToText() == U"-4.*pow(10,3)") << ToBasicString(el->elements->Get(2)->ToText());
    ASSERT_TRUE(el->elements->Get(3)->ToText() == U"-4.*pow(10,3)");
    ASSERT_TRUE(el->elements->Get(5)->ToText() == U"4.*pow(10,3)");
}

}
