#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

TEST_F(SolverIntegerTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.InsertPlus(true);
    document.InsertString("35", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                            "<mo>+</mo>"\
                            "<mi>35</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2380</mi>"\
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
                        "<mi>2345</mi>"\
                        "<mo>+</mo>"\
                        "<mi>35</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();
}

TEST_F(SolverIntegerTest, solver2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("23", true);
    document.InsertMultiply(true);
    document.InsertString("355", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>-</mo>"\
                            "<mi>23</mi>"\
                            "<mo>×</mo>"\
                            "<mi>355</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mo>-</mo>"\
                                "<mi>8165</mi>"\
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
                        "<mo>-</mo>"\
                        "<mi>23</mi>"\
                        "<mo>×</mo>"\
                        "<mi>355</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3, 3})) << document.GetEditorState().ToString();
}

//Present Auto result as Integer result
TEST_F(SolverIntegerTest, solver3)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2345.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetResult({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::INTEGER));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2345</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

}
