#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

TEST_F(SolverRationalTest, rational1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>1</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>1</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
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
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Present Auto result as Rational result
TEST_F(SolverRationalTest, rational2)
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
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>1</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>0.5</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetResult({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::RATIONAL, true));
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
                                    "<mi>1</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>1</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
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
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(1)/(2)=0.5"
        ) << ToBasicString(document.ToText());
}

TEST_F(SolverRationalTest, rational3)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertMinus(true);
    document.InsertString("2", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
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
                                    "<mo>-</mo>"\
                                    "<mi>2</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mo>-</mo>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>2</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>3</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(SolverRationalTest, rational4)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
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
                                    "<mi>6</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>3</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Check proper and improper fractions
TEST_F(SolverRationalTest, rational5)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::PROPER;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("9", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("7", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(9)/(7)=1(2)/(7)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::IMPROPER;
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("9", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("7", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(9)/(7)=1(2)/(7)\n"
        U"(9)/(7)=(9)/(7)"
        ) << ToBasicString(document.ToText());
}

//Check proper and improper fractions
TEST_F(SolverRationalTest, rational6)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::PROPER;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("42", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(42)/(3)=14"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::IMPROPER;
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("42", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(42)/(3)=14\n"
        U"(42)/(3)=14"
        ) << ToBasicString(document.ToText());
}

//Set proper and improper fractions
TEST_F(SolverRationalTest, rational7)
{
    Start(600);
    
    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::IMPROPER;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("11", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
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

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=(11)/(5)"
        ) << ToBasicString(document.ToText());
}

TEST_F(SolverRationalTest, units1)
{
    Start(600);
    
    document.InsertDivision(true);
    document.InsertString("1m", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("3s", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"(1m)/(3s)=(1)/(3)(m)/(s)") << ToBasicString(document.ToText());
}

TEST_F(SolverRationalTest, units2)
{
    Start(600);

    document.InsertCode(1, true);
    document.InsertString("fut", true);
    document.WaitTask(document.InsertSubscript(true));
    document.InsertString("rus", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"fut{rus}=1fut{rus}") << ToBasicString(document.ToText());
}

}
