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
    config.rational_result.fraction_form = FractionForm::Proper;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("9", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("7", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(9)/(7)=1(2)/(7)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::Improper;
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
    config.rational_result.fraction_form = FractionForm::Proper;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("42", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(42)/(3)=14"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::Improper;
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
    config.rational_result.fraction_form = FractionForm::Improper;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("11", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=(11)/(5)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetFractionForm({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, FractionForm::Proper, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=2(1)/(5)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"(11)/(5)=(11)/(5)"
        ) << ToBasicString(document.ToText());
}

//Save and load
TEST_F(SolverRationalTest, rational8)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("234", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    document.Save("solver5_1.yut");

    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.WaitTask(document.Load("solver5_1.yut"));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"(234)/(5)=46(4)/(5)") << ToBasicString(document.ToText());
}

//Solve an expression
TEST_F(SolverRationalTest, rational9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("0", true);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"0=0") << ToBasicString(document.ToText());
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
    ASSERT_TRUE(document.ToText() == U"(1m)/(3s)=20(m)/(min)") << ToBasicString(document.ToText());
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
    ASSERT_TRUE(document.ToText() == U"fut{rus}=1fut{rus}") << ToBasicString(document.ToText());
}

//Changing unit of result
TEST_F(SolverRationalTest, units3)
{
    Start(600);
    
    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::Improper;
    document.SetConfig(config);

    document.InsertDivision(true);
    document.InsertString("50", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.InsertString("3s", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"(50)/(3s)=(50)/(3)Hz") << ToBasicString(document.ToText());

    std::vector<yutovo_calculator::Unit> cast_units;
    document.GetCastUnits({0, 0, 0, 0, 0, 0, 0, 2, 0}, cast_units);
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(std::find(cast_units.begin(), cast_units.end(), yutovo_calculator::Unit(U"Hz")) != cast_units.end());
    ASSERT_TRUE(std::find(cast_units.begin(), cast_units.end(), yutovo_calculator::Unit(U"kHz")) != cast_units.end());
    ASSERT_TRUE(std::find(cast_units.begin(), cast_units.end(), yutovo_calculator::Unit(U"MHz")) != cast_units.end());

    yutovo_calculator::Unit unit(U"ms", -1);
    document.WaitTask(document.SetUnit({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, unit, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"(50)/(3s)=(1)/(60)(1)/(ms)") << ToBasicString(document.ToText());
}

}
