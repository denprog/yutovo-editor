/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//The classic limit: lim_{x→0} sin(x)/x = 1
TEST_F(SolverAutoTest, limit1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false); //cross the arrow into the value row
    document.InsertString(U"0", true);
    document.MoveCaretRight(false); //leave the element into the containing row
    InsertExpression(U"sin(x)/x");
    document.MoveCaretRight(false); //leave the denominator into the containing row

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]sin((x))/(x)=1.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//The approach direction as a power of the value: lim_{x→0+} 1/x = ∞, lim_{x→0-} 1/x = -∞
TEST_F(SolverAutoTest, limit2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertPlus(true));
    document.MoveCaretRight(false); //leave the exponent
    document.MoveCaretRight(false); //leave the element into the containing row
    InsertExpression(U"1/x");
    document.MoveCaretRight(false); //leave the denominator into the containing row

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,+)](1)/(x)=∞") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,+)](1)/(x)") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,+)](1)/(x)=∞") << ToBasicString(document.ToText());
}

//The direction from below: lim_{x→0-} 1/x = -∞
TEST_F(SolverAutoTest, limit3)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertMinus(true));
    document.MoveCaretRight(false); //leave the exponent
    document.MoveCaretRight(false); //leave the element into the containing row
    InsertExpression(U"1/x");
    document.MoveCaretRight(false); //leave the denominator into the containing row

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,-)](1)/(x)=-∞") << ToBasicString(document.ToText());
}

//The limit at infinity: lim_{x→∞} 1/x = 0
TEST_F(SolverAutoTest, limit4)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"∞", true));
    document.MoveCaretRight(false);
    InsertExpression(U"1/x");
    document.MoveCaretRight(false); //leave the denominator into the containing row

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→∞](1)/(x)=0.") << ToBasicString(document.ToText());
}

//A symbolic parameter stays in the result: lim_{x→0} a*sin(x)/x = a
TEST_F(SolverAutoTest, limit5)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    InsertExpression(U"a*sin(x)/x");
    document.MoveCaretRight(false); //leave the denominator into the containing row

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]a*sin((x))/(x)=1.a") << ToBasicString(document.ToText());
}

//The full html of a solved limit
TEST_F(SolverAutoTest, limit6)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    InsertExpression(U"sin(x)/x");
    document.MoveCaretRight(false); //leave the denominator into the containing row

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]sin((x))/(x)=1.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<munder>"\
                                "<mi>lim</mi>"\
                                "<mrow>"\
                                    "<mrow>"\
                                        "<mi>x</mi>"\
                                    "</mrow>"\
                                    "<mo>→</mo>"\
                                    "<mrow>"\
                                        "<mi>0</mi>"\
                                    "</mrow>"\
                                "</mrow>"\
                            "</munder>"\
                            "<mi>sin</mi>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mo>(</mo>"\
                                    "<mi>x</mi>"\
                                    "<mo>)</mo>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//An incomplete limit changes nothing while editing: the function solves on its own
TEST_F(SolverAutoTest, limit7)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false); //cross the arrow into the value row
    document.MoveCaretRight(false); //leave the element into the containing row
    InsertExpression(U"x+1");

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"lim[x→]x+1=1+x") << ToBasicString(document.ToText());
}

}
