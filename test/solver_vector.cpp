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
using namespace yutovo_solver;
using namespace std::chrono_literals;

//a vector literal renders as a column: each element inside its own brackets
TEST_F(SolverVectorTest, solver1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[1,2,3]=[[1.],[2.],[3.]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mo>[</mo>"
                            "<mi>1</mi>"
                            "<mo>,</mo>"
                            "<mi>2</mi>"
                            "<mo>,</mo>"
                            "<mi>3</mi>"
                            "<mo>]</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mover accent='true'>"
                                    "<mrow>"
                                        "<mo>(</mo>"
                                        "<mtable>"
                                            "<mtr>"
                                                "<mtd><mrow><mi>1.</mi></mrow></mtd>"
                                            "</mtr>"
                                            "<mtr>"
                                                "<mtd><mrow><mi>2.</mi></mrow></mtd>"
                                            "</mtr>"
                                            "<mtr>"
                                                "<mtd><mrow><mi>3.</mi></mrow></mtd>"
                                            "</mtr>"
                                        "</mtable>"
                                        "<mo>)</mo>"
                                        "</mrow>"
                                        "<mo>⃗</mo>"
                                "</mover>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>")
        << document.ToHtml();
}

//a one-element vector is a scalar and renders without brackets
TEST_F(SolverVectorTest, solver2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("norm", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"norm([3,4])=5.") << ToBasicString(document.ToText());
}

//the dot product of two vectors is a scalar
TEST_F(SolverVectorTest, solver3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("4", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertComma(true);
    document.InsertString("6", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"dot([1,2,3],[4,5,6])=32.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>dot</mi>"
                            "<mo>(</mo>"
                            "<mo>[</mo>"
                            "<mi>1</mi>"
                            "<mo>,</mo>"
                            "<mi>2</mi>"
                            "<mo>,</mo>"
                            "<mi>3</mi>"
                            "<mo>]</mo>"
                            "<mo>,</mo>"
                            "<mo>[</mo>"
                            "<mi>4</mi>"
                            "<mo>,</mo>"
                            "<mi>5</mi>"
                            "<mo>,</mo>"
                            "<mi>6</mi>"
                            "<mo>]</mo>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>32.</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") 
        << document.ToHtml();
}

//rational vector elements render as fractions
TEST_F(SolverVectorTest, solver4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[(1)/(2),4]=[[(1)/(2)],[4]]") << ToBasicString(document.ToText());
}

//complex vector elements
TEST_F(SolverVectorTest, solver5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_COMPLEX, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[1+i,2]=[[1.+1.i],[2.]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mo>[</mo>"
                            "<mi>1</mi>"
                            "<mo>+</mo>"
                            "<mi>i</mi>"
                            "<mo>,</mo>"
                            "<mi>2</mi>"
                            "<mo>]</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mover accent='true'>"
                                    "<mrow>"
                                        "<mo>(</mo>"
                                        "<mtable>"
                                            "<mtr>"
                                                "<mtd><mrow><mi>1.</mi><mo>+</mo><mi>1.</mi><mi>i</mi></mrow></mtd>"
                                            "</mtr>"
                                            "<mtr>"
                                                "<mtd><mrow><mi>2.</mi></mrow></mtd>"
                                            "</mtr>"
                                        "</mtable>"
                                        "<mo>)</mo>"
                                        "</mrow>"
                                        "<mo>⃗</mo>"
                                "</mover>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>")
        << document.ToHtml();
}

//a shape error in the vector parser shows the error mark
TEST_F(SolverVectorTest, solver6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(ToBasicString(document.ToText()).find("=Incorrect operation") != std::string::npos) << ToBasicString(document.ToText());
}

//Editing the input after the result recomputes the vector
TEST_F(SolverVectorTest, vector_edit1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[1,2]=[[1.],[2.]]") << ToBasicString(document.ToText());

    //delete the last element of the input and type a new one: two moves left put the caret after the "2"
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("9", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[1,9]=[[1.],[9.]]") << ToBasicString(document.ToText());
}

//A vector variable defined by an assignment is visible to the vector equations
TEST_F(SolverVectorTest, vector_variables1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("u", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("u", true);
    document.InsertComma(true);
    document.InsertString("u", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"u=[1,2]\ndot(u,u)=5.") << ToBasicString(document.ToText());
}

//A user function is applied elementwise inside a vector literal
TEST_F(SolverVectorTest, vector_user_functions1)
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
    document.InsertString("2", true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.InsertComma(true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"f(x)=x*2\n[f(1),f(2)]=[[2.],[4.]]") << ToBasicString(document.ToText());
}

//The cross product typed as text
TEST_F(SolverVectorTest, vector_cross1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("cross", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"cross([1,0,0],[0,1,0])=[[0.],[0.],[1.]]") << ToBasicString(document.ToText());
}

//A vector variable changes and the dependent equation recomputes
TEST_F(SolverVectorTest, vector_variables2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("u", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);

    document.InsertCode(false, true);
    document.InsertString("norm", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("u", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"u=[3,4]\nnorm(u)=5.") << ToBasicString(document.ToText());

    //change the vector variable: the norm equation re-solves
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("0", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"u=[3,0]\nnorm(u)=3.") << ToBasicString(document.ToText());
}

//The dot product of two vectors through the symbolic parser
TEST_F(SolverSymbolicTest, vector_symbolic1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"dot([1,2],[3,4])=11") << ToBasicString(document.ToText());
}

//The norm of a vector
TEST_F(SolverSymbolicTest, vector_symbolic2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("norm", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"norm([3,4])=5") << ToBasicString(document.ToText());
}

//The cross product renders as a single-row matrix
TEST_F(SolverSymbolicTest, vector_symbolic3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("cross", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"cross([1,0,0],[0,1,0])=[[0,0,1]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>cross</mi>"
                            "<mo>(</mo>"
                            "<mo>[</mo>"
                            "<mi>1</mi>"
                            "<mo>,</mo>"
                            "<mi>0</mi>"
                            "<mo>,</mo>"
                            "<mi>0</mi>"
                            "<mo>]</mo>"
                            "<mo>,</mo>"
                            "<mo>[</mo>"
                            "<mi>0</mi>"
                            "<mo>,</mo>"
                            "<mi>1</mi>"
                            "<mo>,</mo>"
                            "<mi>0</mi>"
                            "<mo>]</mo>"
                            "<mo>)</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mo>(</mo>"
                            "<mtable>"
                                "<mtr>"
                                    "<mtd>"
                                        "<mrow>"
                                            "<mi>0</mi>"
                                        "</mrow>"
                                    "</mtd>"
                                    "<mtd>"
                                        "<mrow>"
                                            "<mi>0</mi>"
                                        "</mrow>"
                                    "</mtd>"
                                    "<mtd>"
                                        "<mrow>"
                                            "<mi>1</mi>"
                                        "</mrow>"
                                    "</mtd>"
                                "</mtr>"
                            "</mtable>"
                            "<mo>)</mo>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") 
        << document.ToHtml();
}

//The unit vector keeps exact fractions
TEST_F(SolverSymbolicTest, vector_symbolic4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("normalize", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"normalize([3,4])=[[0.6,0.8]]") << ToBasicString(document.ToText());
}

//A vector variable with symbols
TEST_F(SolverSymbolicTest, vector_symbolic5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("u", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("x", true);
    document.InsertComma(true);
    document.InsertString("y", true);
    document.InsertCloseSquareBracket(true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("u", true);
    document.InsertComma(true);
    document.InsertString("u", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"u=[x,y]\ndot(u,u)=pow(x,2)+pow(y,2)") << ToBasicString(document.ToText());
}

//A user function returning a vector
TEST_F(SolverSymbolicTest, vector_symbolic6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("t", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.InsertString("t", true);
    document.InsertMultiply(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("f", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("2", true);
    document.InsertCloseRoundBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"f(t)=t*[1,2]\ndot(f(2),[1,1])=6") << ToBasicString(document.ToText());
}

//Vector operations inside a bigger expression
TEST_F(SolverSymbolicTest, vector_symbolic7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertMultiply(true);
    document.InsertString("norm", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.InsertPlus(true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"2*norm([3,4])+dot([1,2],[3,4])=21") << ToBasicString(document.ToText());
}

//The angle between vectors
TEST_F(SolverSymbolicTest, vector_symbolic8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("angle", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"angle([1,0],[0,1])=1.571") << ToBasicString(document.ToText());
}

//The matrix-vector product through the symbolic parser
TEST_F(SolverSymbolicTest, vector_symbolic9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("det", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"det([[1,2],[3,4]])=-2") << ToBasicString(document.ToText());
}

//A symbolic rational dot product of fractions stays exact
TEST_F(SolverSymbolicTest, vector_symbolic10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"dot([(1)/(2),(1)/(3)],[(1)/(4),(1)/(5)])=(23)/(120)") << ToBasicString(document.ToText());
}

//A complex vector dot product
TEST_F(SolverSymbolicTest, vector_symbolic11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertMinus(true);
    document.InsertString("i", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertMinus(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_COMPLEX, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"dot([1+i,1-i],[1-i,1+i])=4") << ToBasicString(document.ToText());
}

//Editing the vector variable re-solves the dependent symbolic equation
TEST_F(SolverSymbolicTest, vector_symbolic12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("u", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);

    document.InsertCode(false, true);
    document.InsertString("norm", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("u", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitTask(document.InsertEquation(ResultType::SYMBOLIC_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"u=[3,4]\nnorm(u)=5") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.InsertString("0", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"u=[3,0]\nnorm(u)=3") << ToBasicString(document.ToText());
}

//Vector rational results: the elements render as fractions inside the column

TEST_F(SolverVectorTest, vector_rational1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[(1)/(2),(3)/(4)]=[[(1)/(2)],[(3)/(4)]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mo>[</mo>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>1</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>2</mi>"
                                "</mrow>"
                            "</mfrac>"
                            "<mo>,</mo>"
                            "<mfrac>"
                                "<mrow>"
                                    "<mi>3</mi>"
                                "</mrow>"
                                "<mrow>"
                                    "<mi>4</mi>"
                                "</mrow>"
                            "</mfrac>"
                            "<mo>]</mo>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mover accent='true'>"
                                    "<mrow>"
                                        "<mo>(</mo>"
                                        "<mtable>"
                                            "<mtr>"
                                                "<mtd>"
                                                    "<mrow>"
                                                        "<mfrac>"
                                                            "<mrow>"
                                                                "<mi>1</mi>"
                                                            "</mrow>"
                                                            "<mrow>"
                                                                "<mi>2</mi>"
                                                            "</mrow>"
                                                        "</mfrac>"
                                                    "</mrow>"
                                                "</mtd>"
                                            "</mtr>"
                                            "<mtr>"
                                                "<mtd>"
                                                    "<mrow>"
                                                        "<mfrac>"
                                                            "<mrow>"
                                                                "<mi>3</mi>"
                                                            "</mrow>"
                                                            "<mrow>"
                                                                "<mi>4</mi>"
                                                            "</mrow>"
                                                        "</mfrac>"
                                                    "</mrow>"
                                                "</mtd>"
                                            "</mtr>"
                                        "</mtable>"
                                        "<mo>)</mo>"
                                        "</mrow>"
                                        "<mo>⃗</mo>"
                                "</mover>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>")
        << document.ToHtml();
}

TEST_F(SolverVectorTest, vector_rational2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("normalize", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"normalize([3,4])=[[(3)/(5)],[(4)/(5)]]") << ToBasicString(document.ToText());
}

TEST_F(SolverVectorTest, vector_rational3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("cross", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("4", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertComma(true);
    document.InsertString("6", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"cross([1,2,3],[4,5,6])=[[-3],[6],[-3]]") << ToBasicString(document.ToText());
}

//Vector complex results

TEST_F(SolverVectorTest, vector_complex1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("dot", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertMinus(true);
    document.InsertString("i", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertMinus(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("i", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_COMPLEX, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"dot([1+i,1-i],[1-i,1+i])=4.") << ToBasicString(document.ToText());
}

TEST_F(SolverVectorTest, vector_complex2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("norm", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertMultiply(true);
    document.InsertString("i", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_COMPLEX, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"norm([3,4*i])=5.") << ToBasicString(document.ToText());
}

TEST_F(SolverVectorTest, vector_complex3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("cross", true);
    document.InsertOpenRoundBracket(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertComma(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("0", true);
    document.InsertComma(true);
    document.InsertString("i", true);
    document.InsertComma(true);
    document.InsertString("0", true);
    document.InsertCloseSquareBracket(true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::VECTOR_COMPLEX, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"cross([i,0,0],[0,i,0])=[[0.],[0.],[-1.]]") << ToBasicString(document.ToText());
}

}
