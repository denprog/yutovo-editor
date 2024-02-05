#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

//Changing value of a variable and resolve a dependent expression
TEST_F(VariablesTest, variables1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>9.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("2", true));
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>42</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>47.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Insert unknown variable and add it above later
TEST_F(VariablesTest, variables2)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                            "<mo>+</mo>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>9.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretDown(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                            "<mo>+</mo>"\
                            "<mi>54</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>59.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete a paragraph with a variable
TEST_F(VariablesTest, variables3)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(1s);
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
                            "<mi>d</mi>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>Unknown identifier</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>9.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Redefine a variable
TEST_F(VariablesTest, variables4)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("d", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("7", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=4\n" \
        U"d+5=9.\n" \
        U"d=45+d\n" \
        U"d+7=56."
        ) << ToBasicString(document.ToText());
    
    for (int i = 0; i < 3; ++i)
        document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=42\n" \
        U"d+5=47.\n" \
        U"d=45+d\n" \
        U"d+7=94."
        ) << ToBasicString(document.ToText());
}

//Recalculate after changing a variable
TEST_F(VariablesTest, variables5)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=(5)/(4)\n" \
        U"d=1.25"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=(5)/()\n" \
        U"d=Unknown identifier"
        ) << ToBasicString(document.ToText());
}

//Variable with a long number
TEST_F(VariablesTest, variables6)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2244444444444", true);
    document.WaitSolver();

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2244444444444\n" \
        U"d=2.244*pow(10,12)"
        ) << ToBasicString(document.ToText());
}

//Insert a paragraph in a variable's value
TEST_F(VariablesTest, variables7)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2345", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2345\n" \
        U"d=2345."
        ) << ToBasicString(document.ToText());

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.InsertParagraph(true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=234\n" \
        U"5\n" \
        U"d=234."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2345\n" \
        U"d=2345."
        ) << ToBasicString(document.ToText());
}

//Define a variable with an empty placeholder
TEST_F(VariablesTest, errors1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"d=") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);

    document.InsertString("4", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"d=4+") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 2);
}

//Define a variable with unknown variable
TEST_F(VariablesTest, errors2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.InsertPlus(true);
    document.InsertString("t", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"d=4+t") << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(!document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 2}, start, size));
}

//Define a variable with unknown variable
TEST_F(VariablesTest, errors3)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.InsertPlus(true);
    document.InsertString("t", true);
    document.WaitSolver();
    
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=4+t\n" \
        U"d+5=Unknown identifier"
        ) << ToBasicString(document.ToText());
    std::this_thread::sleep_for(600ms);
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);
}

//Define a variable with recursion
TEST_F(VariablesTest, errors4)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("4", true);
    document.InsertPlus(true);
    document.InsertString("d", true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=4+d\n" \
        U"d+5=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);
}

//Rational variables
TEST_F(VariablesTest, variables8)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=(1)/(3)\n" \
        U"d=(1)/(3)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.error_marks.size() == 0);

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=(1)/(3)\n" \
        U"d=(4)/(5)\n" \
        U"d=(4)/(5)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.error_marks.size() == 0);
}

//Variable with equation
TEST_F(VariablesTest, variables9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("12", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=12=12."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("a", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=12=12.\n" \
        U"a=12."
        ) << ToBasicString(document.ToText());
}

}
