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

//Evaluate at a point: pow(x,2) |_{x=3} = 9
TEST_F(SolverAutoTest, evaluate1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false); //move out of the exponent

    document.InsertEvaluationBarSubscript(true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"pow(x,2)[x=3]=9.") << ToBasicString(document.ToText());
}

//Evaluate at a point with several variables: x+y |_{x=1,y=2} = 3
TEST_F(SolverAutoTest, evaluate2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.InsertString("y", true);

    document.InsertEvaluationBarSubscript(true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("1", true);
    document.InsertParagraph(true);
    document.MoveCaretRight(false);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"x+y[x=1,y=2]=3.") << ToBasicString(document.ToText());
}

//Evaluate a user function at a point: f(x)=x*x, f(x) |_{x=3} = 9
TEST_F(SolverAutoTest, evaluate3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("x", true);
    document.InsertMultiply(true);
    document.InsertString("x", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);

    document.InsertEvaluationBarSubscript(true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() ==
        U"f(x)=x*x\n"
        U"f(x)[x=3]=9."
        ) << ToBasicString(document.ToText());
}

//Evaluate at a point after a plain fraction: (x)/(y) |_{y=2,x=6} = 3
TEST_F(SolverAutoTest, evaluate4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertDivision(true);
    document.InsertString("y", true);
    document.MoveCaretRight(false); //move out of the denominator

    document.InsertEvaluationBarSubscript(true);
    document.MoveCaretRight(false);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.InsertParagraph(true);
    document.MoveCaretRight(false);
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("6", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"(x)/(y)[y=2,x=6]=3.") << ToBasicString(document.ToText());
}

//Insert the function at point template and solve: (pow(x,2)) |_{x=3} = 9
TEST_F(SolverAutoTest, function_at_point1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("func", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("y", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("y", true);
    document.InsertPower(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));

    document.WaitTask(document.InsertFunctionAtPoint(true));
    document.InsertString("func", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("y", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"func(y)=pow(y,2)\n"
        U"func(y)[y=3]=9."
        ) << ToBasicString(document.ToText());
}

}
