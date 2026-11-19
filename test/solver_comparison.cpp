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

//2 < 3 solves to 1
TEST_F(SolverAutoTest, comparison1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"2<3=1.") << ToBasicString(document.ToText());
}

//The equal comparison is == for the calculator: 5 == 5 solves to 1
TEST_F(SolverAutoTest, comparison2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"5", true));
    document.WaitTask(document.InsertEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"5=5=1.") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"5=5") << ToBasicString(document.ToText());
}

//2 > 3 solves to 0
TEST_F(SolverAutoTest, comparison3)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertGreater(true));
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"2>3=0.") << ToBasicString(document.ToText());
}

//2 >= 3 solves to 0
TEST_F(SolverAutoTest, comparison4)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertGreaterEqual(true));
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"2⩾3=0.") << ToBasicString(document.ToText());
}

//2 <= 3 solves to 1
TEST_F(SolverAutoTest, comparison5)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLessEqual(true));
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"2⩽3=1.") << ToBasicString(document.ToText());
}

//1 <> 2 solves to 1
TEST_F(SolverAutoTest, comparison6)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.InsertNotEqual(true));
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"1≠2=1.") << ToBasicString(document.ToText());
}

//A comparison is a parenthesized operand: 2*(x<5) with x:=3 solves to 2
TEST_F(SolverAutoTest, comparison7)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the assignment
    document.WaitTask(document.InsertString(U"2", true));
    document.InsertMultiply(true);
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"5", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison
    document.InsertCloseRoundBracket(true);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"x=32*(x<5)=2.") << ToBasicString(document.ToText());
}

//An incomplete comparison changes nothing while editing: no solve, no error
TEST_F(SolverAutoTest, comparison8)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    //an equation with nothing to solve shows the same error as an empty equation
    ASSERT_TRUE(document.ToText() == U"2<=Expression expected") << ToBasicString(document.ToText());
}

//A comparison inside the left part of an equation: 2 < 3+1 solves to 1
TEST_F(SolverAutoTest, comparison9)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"3", true));
    document.InsertPlus(true);
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"2<3+1=1.") << ToBasicString(document.ToText());
}

}
