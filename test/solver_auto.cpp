#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

TEST_F(SolverAutoTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
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

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
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

TEST_F(SolverAutoTest, solver2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>2</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, solver3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.Save("solver3_1.yut");
    document.WaitTask(document.New());
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Load("solver3_1.yut"));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//log
TEST_F(SolverAutoTest, solver4)
{
    Start(600);
    
    document.WaitTask(document.InsertSubscriptFunction("log", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi>log</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertCodeString("2", true));
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertCodeString("8", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<msub>"\
                                "<mrow>"\
                                    "<mi>log</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</msub>"\
                            "<mi>8</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Solve after changing element
TEST_F(SolverAutoTest, solver5)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("3345", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("2", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3345</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>22</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>152.045</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Solve with errors
TEST_F(SolverAutoTest, solver6)
{
    Start(600);
    
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>Null</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>Null</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.InsertString("2", true);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>Null</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Remove the equation sign
TEST_F(SolverAutoTest, solver7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.InsertString("15", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12</mi>"\
                            "<mo>+</mo>"\
                            "<mi>15</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>27.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12</mi>"\
                            "<mo>+</mo>"\
                            "<mi>15</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>27.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                        "<mo>+</mo>"\
                        "<mi>15</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>12</mi>"\
                            "<mo>+</mo>"\
                            "<mi>15</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>27.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Replace the row with equation below and change it
TEST_F(SolverAutoTest, solver8)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>32</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>35.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2</mi>"\
                            "<mo>+</mo>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a paragraph in the row with equation
TEST_F(SolverAutoTest, solver9)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("235", true);
    document.InsertPlus(true);
    document.InsertString("345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235+3=238.\n" \
        U"45"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235+345=580."
        ) << ToBasicString(document.ToText());
}

//Solve a big number, result must be uneditable
TEST_F(SolverAutoTest, solver10)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("235235435345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"235235435345=2.352*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"235235435345"
        ) << ToBasicString(document.ToText());
}

//Change the order of results
TEST_F(SolverAutoTest, solver11)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5\n"
        U"(1)/(2)=(1)/(2)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    ResultType order2[4] = {ResultType::INTEGER, ResultType::RATIONAL, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order2, order2 + 4, config.auto_result.results_order);
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5\n"
        U"(1)/(2)=(1)/(2)\n"
        U"(3)/(2)=1(dec)"
        ) << ToBasicString(document.ToText());
}

//Change a sub-element of an equation
TEST_F(SolverAutoTest, solver12)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("1234", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(2)=617."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(22)=56.091"
        ) << ToBasicString(document.ToText());
}

//Solve after deleting a division and undo
TEST_F(SolverAutoTest, solver13)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("1234", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(2)=617."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"12342=12342."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1234)/(2)=617."
        ) << ToBasicString(document.ToText());
}

//Solve after deleting a division and undo
TEST_F(SolverAutoTest, solver14)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("12", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(12)/(2)=6."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretLeft(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"=Expression expected"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(12)/(2)=6."
        ) << ToBasicString(document.ToText());
}

//Change precision
TEST_F(SolverAutoTest, solver16)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("12.3456789012345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789012345=12.346"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 7, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"12.3456789012345=12.3456789"
        ) << ToBasicString(document.ToText());
}

//Change exponential threshold
TEST_F(SolverAutoTest, solver17)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("123456789012", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"123456789012=1.235*pow(10,11)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetExp({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 12, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"123456789012=123456789012."
        ) << ToBasicString(document.ToText());
}

//Change angle measure
TEST_F(SolverAutoTest, solver18)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenFence(true);
    document.InsertString("1", true);
    document.InsertCloseFence(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetResultAngleMeasure({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, AngleMeasure::DEGREE, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=90.(deg)"
        ) << ToBasicString(document.ToText());
}

//Change fraction form on auto result
TEST_F(SolverAutoTest, solver19)
{
    Start(600);

    document.GetConfig(config);
    ResultType order[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order, order + 4, config.auto_result.results_order);
    config.auto_result.rational_result.fraction_form = FractionForm::IMPROPER;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("11", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=(11)/(5)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetFractionForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, FractionForm::PROPER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=2(1)/(5)"
        ) << ToBasicString(document.ToText());
}

//Change the order of results
TEST_F(SolverAutoTest, solver20)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("1.2", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)+1.2=1(7)/(10)"
        ) << ToBasicString(document.ToText());
}

//Save and load with changing a result parameter
TEST_F(SolverAutoTest, solver21)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234.12345678", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitTask(document.SetPrecision({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, 7, true));
    document.WaitSolver();
    document.Save("solver21_1.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.WaitTask(document.Load("solver21_1.yut"));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"234.12345678=234.1234568") << ToBasicString(document.ToText());
}

//Change the order of results
TEST_F(SolverAutoTest, solver22)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("cos", true);
    document.InsertOpenFence(true);
    document.InsertString("0", true);
    document.InsertCloseFence(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"cos(0)=1."
        ) << ToBasicString(document.ToText());
}

//Solve with errors
TEST_F(SolverAutoTest, errors1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>3.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.InsertDivision(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>Null</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.InsertString("4", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>4</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>0.75</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>4</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>4</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>0.75</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>Null</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Syntax error</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Solve with errors
TEST_F(SolverAutoTest, errors2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.InsertString("2", true);
    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt((2)/())=Syntax error") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 3);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt((2)/(3))=0.816") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.error_marks.empty());

    document.MoveCaretUp(false);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt(23)=4.796") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.error_marks.empty());
}

//Solve with errors
TEST_F(SolverAutoTest, errors3)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"sqrt()=Wrong arguments count in 'sqrt'") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 2);
}

TEST_F(SolverAutoTest, units1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1m</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                                "<mi>m</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, units2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2cm", true);
    document.InsertMultiply(true);
    document.InsertString("3cm", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2cm</mi>"\
                            "<mo>×</mo>"\
                            "<mi>3cm</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>6.</mi>"\
                                "<msup>"\
                                    "<mrow>"\
                                        "<mi>cm</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                "</msup>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverAutoTest, units3)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4s", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(2m)/(4s)=0.5(m)/(s)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, units4)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("6kg", true);
    document.InsertMultiply(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4s", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(6kg*2m)/(4s)=3.(kg*m)/(s)") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, units5)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("6kg", true);
    document.InsertMultiply(true);
    document.InsertString("2m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("4", true);
    document.InsertMultiply(true);
    document.InsertString("s", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(6kg*2m)/(4*pow(s,2))=3.N") << ToBasicString(document.ToText());
}

TEST_F(SolverAutoTest, units6)
{
    Start(600);

    document.InsertCode(1, true);
    document.InsertString("fut", true);
    document.WaitTask(document.InsertSubscript(true));
    document.InsertString("rus", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<msub>"\
                                "<mrow>"\
                                    "<mi>fut</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>rus</mi>"\
                                "</mrow>"\
                            "</msub>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.</mi>"\
                                "<msub>"\
                                    "<mrow>"\
                                        "<mi>fut</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>rus</mi>"\
                                    "</mrow>"\
                                "</msub>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

}
