#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
using namespace std::chrono_literals;

//Solve an array
TEST_F(SolverArrayRealTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2.3", true);
    document.InsertComma(true);
    document.InsertString("4.56", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>[</mo>"\
                            "<mi>1</mi>"\
                            "<mo>,</mo>"\
                            "<mi>2.3</mi>"\
                            "<mo>,</mo>"\
                            "<mi>4.56</mi>"\
                            "<mo>]</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                "<mo>[</mo>"\
                                "<mi>1.</mi>"\
                                "<mo>,</mo>"\
                                "<mi>2.3</mi>"\
                                "<mo>,</mo>"\
                                "<mi>4.56</mi>"\
                                "<mo>]</mo>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Empty array
TEST_F(SolverArrayRealTest, solver2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>[</mo>"\
                            "<mo>]</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                "<mo>[</mo>"\
                                "<mo>]</mo>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Sum of array
TEST_F(SolverArrayRealTest, solver3)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertSum(true);
    document.InsertString(U"n", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"5", true);
    document.MoveCaretRight(false);

    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertComma(true);
    document.InsertString("3", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.InsertComma(true);
    document.InsertString("5", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.MoveCaretRight(false));

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == U"sum(n=1,5,[1,2,3,4,5])=[5.,10.,15.,20.,25.]") << ToBasicString(document.ToText());
}

//Array as variable
TEST_F(SolverArrayRealTest, solver4)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1.23", true);
    document.InsertComma(true);
    document.InsertString("2", true);
    document.InsertPower(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.InsertComma(true);
    document.InsertMinus(true);
    document.InsertString("55", true);
    document.InsertComma(true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertSum(true);
    document.InsertString(U"n", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"size", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString(U"d", true);
    document.InsertCloseRoundBracket(true);
    document.InsertMinus(true);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.InsertString(U"d", true);
    document.InsertSubscript(true);
    document.InsertString(U"n", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == 
        U"d=[1.23,pow(2,3),-55,4]\n"\
        "sum(n=0,size(d)-1,d{n})=-41.77"
        ) << ToBasicString(document.ToText());
}

//Product with arrays
TEST_F(SolverArrayRealTest, solver5)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1.2", true);
    document.InsertComma(true);
    document.InsertString("77", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("e", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("44", true);
    document.InsertComma(true);
    document.InsertString("7", true);
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));

    document.InsertProduct(true);
    document.InsertString("i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"size", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString(U"d", true);
    document.InsertCloseRoundBracket(true);
    document.InsertMinus(true);
    document.InsertString(U"1", true);
    document.MoveCaretRight(false);
    document.InsertString(U"d", true);
    document.InsertSubscript(true);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.InsertMultiply(true);
    document.InsertString(U"e", true);
    document.InsertSubscript(true);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    ASSERT_TRUE(document.ToText() == 
        U"d=[1.2,77]\n"\
        U"e=[44,7]\n"\
        "prod(i=0,size(d)-1,d{i}*e{i})=28459.2"
        ) << ToBasicString(document.ToText());
}

//Array with units
TEST_F(SolverArrayRealTest, solver6)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("4м", true);
    document.InsertComma(true);
    document.InsertString("7с", true);
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::ARRAY_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"[4м,7с]=[4.м,7.с]"
        ) << ToBasicString(document.ToText());
}

}
