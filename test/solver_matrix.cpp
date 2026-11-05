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

static const ElementId matrix_id{0, 0, 0, 0, 0, 0, 0};

//A matrix literal solves through the symbolic parser and the result is rendered as a matrix
TEST_F(SolverAutoTest, matrix1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]=[[1,2],[3,4]]") << ToBasicString(document.ToText());

    //the result is a matrix element rendered as a grid
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mo>(</mo>"
                            "<mtable>"
                                "<mtr>"
                                    "<mtd><mrow><mi>1</mi></mrow></mtd>"
                                    "<mtd><mrow><mi>2</mi></mrow></mtd>"
                                "</mtr>"
                                "<mtr>"
                                    "<mtd><mrow><mi>3</mi></mrow></mtd>"
                                    "<mtd><mrow><mi>4</mi></mrow></mtd>"
                                "</mtr>"
                            "</mtable>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mo>(</mo>"
                            "<mtable>"
                                "<mtr>"
                                    "<mtd><mrow><mi>1</mi></mrow></mtd>"
                                    "<mtd><mrow><mi>2</mi></mrow></mtd>"
                                "</mtr>"
                                "<mtr>"
                                    "<mtd><mrow><mi>3</mi></mrow></mtd>"
                                    "<mtd><mrow><mi>4</mi></mrow></mtd>"
                                "</mtr>"
                            "</mtable>"
                            "<mo>)</mo>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();
}

//A determinant solves to its scalar value
TEST_F(SolverAutoTest, matrix2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertDeterminant(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"det([[1,2],[3,4]])=-2") << ToBasicString(document.ToText());
}

//A symbolic determinant with variables
TEST_F(SolverAutoTest, matrix3)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertDeterminant(true, 2, 2));
    document.InsertString("x", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"det([[x,2],[3,4]])=-6+4*x") << ToBasicString(document.ToText());
}

//Matrix multiplication
TEST_F(SolverAutoTest, matrix4)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    //multiply by the same matrix: the caret sits after the first matrix, insert the product sign and the second one
    document.WaitTask(document.InsertMultiply(true));
    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.InsertString("6", true);
    document.MoveCaretRight(false);
    document.InsertString("7", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("8", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]*[[5,6],[7,8]]=[[19,22],[43,50]]") << ToBasicString(document.ToText());
}

//Resizing a solved matrix re-solves the equation
TEST_F(SolverAutoTest, matrix5)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]=[[1,2],[3,4]]") << ToBasicString(document.ToText());

    //shrink the solved matrix - the equation is re-solved
    std::vector<ElementId> els;
    document.GetElement(ElementId{0})->GetElements(ElementType::MATRIX, els);
    ASSERT_TRUE(els.size() == 2) << els.size(); //the left part and the rendered result
    document.WaitTask(document.SetMatrixSize(els[0], 1, 2, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[[1,2]]=[[1,2]]") << ToBasicString(document.ToText());
}

}
