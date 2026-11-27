/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

TEST_F(AssignmentTest, assignment1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//User variable
TEST_F(AssignmentTest, assignment2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("67", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>67</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                                "<mo>+</mo>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>72.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//User function
TEST_F(AssignmentTest, assignment3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>f</mi>"\
                            "<mo>(</mo>"\
                            "<mi>x</mi>"\
                            "<mo>)</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("4", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>f</mi>"\
                                "<mo>(</mo>"\
                                "<mi>x</mi>"\
                                "<mo>)</mo>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                                "<mo>+</mo>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>f</mi>"\
                                "<mo>(</mo>"\
                                "<mi>4</mi>"\
                                "<mo>)</mo>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>9.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Edit an assignment
TEST_F(AssignmentTest, assignment4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("6", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>6</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>6</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 1})) << document.GetEditorState().ToString();
}

//Insert a paragraph after assignment
TEST_F(AssignmentTest, assignment5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste
TEST_F(AssignmentTest, assignment6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == 
        U"x=5x=5"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert in a division
TEST_F(AssignmentTest, assignment7)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("m", true);
    //an assignment inside a formula is not a declaration, nothing is dispatched to the solver
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() ==
        U"(4)/(m=)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"(4)/(m)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//The expression is typed first, the assignment divides the row at the caret
TEST_F(AssignmentTest, assignment8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"=5+3") << ToBasicString(document.ToText());
    //the variable is not typed yet - the caret is in the left part
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"5+3") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"=5+3") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    //the registered variable solves below
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                                "<mo>+</mo>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>8.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//The assignment divides the row in the middle: the left part takes the elements before the caret, the right part - the elements after it
TEST_F(AssignmentTest, assignment9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+=1") << ToBasicString(document.ToText());
    //the left part is filled - the caret is in the right part
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();
}

//A comma of the enclosing argument list ends the wrapped right part
TEST_F(AssignmentTest, assignment9a)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("a", true);
    document.InsertComma(true);
    document.InsertString("b", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"f(=a,b)") << ToBasicString(document.ToText());
    //the variable is not typed yet - the caret is in the left part
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//A closing bracket of the enclosing call ends the wrapped right part
TEST_F(AssignmentTest, assignment9b)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    //an assignment inside a formula is not a declaration, nothing is dispatched to the solver
    ASSERT_TRUE(document.ToText() == U"sin(=x)") << ToBasicString(document.ToText());
    //the variable is not typed yet - the caret is in the left part
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//A statement element to the right is not wrapped into the right part
TEST_F(AssignmentTest, assignment9c)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"x=5") << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"=x=5") << ToBasicString(document.ToText());
    //the variable is not typed yet - the caret is in the left part
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//The caret goes into the left part when the assignment is inserted into an empty row
TEST_F(AssignmentTest, assignment10)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"=") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//The reverse entry inside a for head
TEST_F(AssignmentTest, assignment11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("for", true);
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertAssignment(true));
    std::this_thread::sleep_for(200ms);
    //an assignment inside a formula is not a declaration, nothing is dispatched to the solver
    ASSERT_TRUE(document.ToText() == U"for(=") << ToBasicString(document.ToText());
    //the variable is not typed yet - the caret is in the left part
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertString("i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    //the comma leaves the right part of the assignment into the containing row
    document.WaitTask(document.InsertComma(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"for(i=1,") << ToBasicString(document.ToText());
}

//Deletion of assignment
TEST_F(AssignmentTest, delete1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x5</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of assignment
TEST_F(AssignmentTest, delete2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();
}

//Delete inside empty right element
TEST_F(AssignmentTest, delete3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x12", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"x12="
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.DeleteElements(true, true));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"x12"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Delete an element on the right
TEST_F(AssignmentTest, delete4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("34", true);
    document.WaitSolver();
#ifdef _WIN32
    std::this_thread::sleep_for(2s);
#else
    std::this_thread::sleep_for(600ms);
#endif
    ASSERT_TRUE(document.ToText() == 
        U"x=2+34"
        ) << ToBasicString(document.ToText());

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
#ifdef _WIN32
    std::this_thread::sleep_for(2s);
#else
    std::this_thread::sleep_for(600ms);
#endif
    ASSERT_TRUE(document.ToText() == 
        U"x=2+"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
#ifdef _WIN32
    std::this_thread::sleep_for(2s);
#else
    std::this_thread::sleep_for(600ms);
#endif
    ASSERT_TRUE(document.ToText() == 
        U"x=2+34"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 2}, 2, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
#ifdef _WIN32
    std::this_thread::sleep_for(2s);
#else
    std::this_thread::sleep_for(600ms);
#endif
    ASSERT_TRUE(document.ToText() == 
        U"x=2+"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 2);
    ASSERT_TRUE(!document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 1}, start, size)) << ErrorMarks();
}

//Assign to a number is a error
TEST_F(AssignmentTest, error1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertAssignment(true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"123="
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0}, start, size)) << ErrorMarks();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Check error marks
TEST_F(AssignmentTest, error2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("r", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(!document.HasErrorMarks(ElementId{0, 0, 0, 0, 0})) << ErrorMarks();

    document.InsertPlus(true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.HasErrorMarks(ElementId{0, 0, 0, 0, 0})) << ErrorMarks();

    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"r5+"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks(ElementId{0, 0, 0, 0, 0})) << ErrorMarks();
}

//Save/load a document with assignment
TEST_F(AssignmentTest, files1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });
    
    document.WaitTask(document.Save("files9.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Load("files9.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

}
