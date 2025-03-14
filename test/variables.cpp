#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_solver;
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
    std::this_thread::sleep_for(1s);
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
    std::this_thread::sleep_for(1s);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
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
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
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
    std::this_thread::sleep_for(1s);
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
    std::this_thread::sleep_for(2s);
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
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d=234\n" \
        U"5\n" \
        U"d=234."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
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
    std::this_thread::sleep_for(2s);
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
    std::this_thread::sleep_for(1s);
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 2}, start, size));
    ASSERT_TRUE(start == 0 && size == 1);
}

//Define a variable with recursion
TEST_F(VariablesTest, errors4)
{
    Start(600);

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

//Check a error position
TEST_F(VariablesTest, errors5)
{
    Start(600);

    document.SetLocale(yutovo_calculator::Language::English, true);
    document.InsertCode(false, true);
    document.InsertString("R", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.InsertString(" ", true);
    document.InsertString("см", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("R", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"R=1см\n" \
        U"R=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 2);
}

//Check error mark
TEST_F(VariablesTest, errors6)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d+5=Unknown identifier"
        ) << ToBasicString(document.ToText());
    auto el = document.FindByString({0}, U"d");
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(el->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);
    el = document.FindByString({0}, U"5");
    ASSERT_TRUE(!document.HasErrorMark(el->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(document.HasErrorMark(el->parent->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);
    ASSERT_TRUE(!document.HasErrorMark(el->parent->parent->id, start, size)) << ErrorMarks();
    ASSERT_TRUE(!document.HasErrorMark(el->parent->parent->parent->id, start, size)) << ErrorMarks();
}

//Rational variables
TEST_F(VariablesTest, variables8)
{
    Start(600);

    document.GetConfig(config);
    ResultType order1[4] = {ResultType::RATIONAL, ResultType::INTEGER, ResultType::REAL, ResultType::COMPLEX};
    std::copy(order1, order1 + 4, config.auto_result.results_order);
    document.WaitTask(document.SetConfig(config, true));

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
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"d=(1)/(3)\n" \
        U"d=(1)/(3)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

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
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"d=(1)/(3)\n" \
        U"d=(4)/(5)\n" \
        U"d=(4)/(5)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
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

//A variable inside a division with a unit
TEST_F(VariablesTest, variables10)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("a", true);
    document.InsertDivision(true);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertString("m", true);

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"a=2\n" \
        U"(a)/(4)m=5.dm"
        ) << ToBasicString(document.ToText());
}

//A variable inside a division with a unit
TEST_F(VariablesTest, variables11)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2\n" \
        U"d=1\n" \
        U"d=1."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=2\n" \
        U"d=2."
        ) << ToBasicString(document.ToText());
}

//Remove a second definition of a variable
TEST_F(VariablesTest, variables12)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("6", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=5\n" \
        U"d=6\n" \
        U"d=6."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.DeleteElements(false, true);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"d=5\n" \
        U"d=5."
        ) << ToBasicString(document.ToText());
}

//Remove a second definition of a variable
TEST_F(VariablesTest, variables13)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("c", true);
    document.InsertAssignment(true);
    document.InsertString("b", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("c", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"b=5\n" \
        U"c=b\n" \
        U"c=5."
        ) << ToBasicString(document.ToText());

    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n" \
        U"c=b\n" \
        U"c=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0}, start, size)) << ErrorMarks();;
    ASSERT_TRUE(start == 0 && size == 1);
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"b=5\n" \
        U"c=b\n" \
        U"c=5."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Remove a code block with a variable
TEST_F(VariablesTest, variables14)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("c", true);
    document.InsertAssignment(true);
    document.InsertString("a", true);
    document.InsertMultiply(true);
    document.InsertString("b", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("c", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"b=2\n" \
        U"c=a*b\n" \
        U"c=2."
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"\n" \
        U"c=a*b\n" \
        U"c=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 2, 0, 0, 0, 0, 0, 2, 2}, start, size)) << ErrorMarks();

    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"c=a*b\n" \
        U"c=Unknown identifier"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 0, 0, 0, 2, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"b=2\n" \
        U"c=a*b\n" \
        U"c=2."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a variable before its using
TEST_F(VariablesTest, variables15)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.InsertParagraph(true);
    document.MoveCaretUp(false);
    document.InsertString("b", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"b=Unknown identifier\n" \
        U"b=1"
        ) << ToBasicString(document.ToText());
}

//Insert a variable before its using
TEST_F(VariablesTest, variables16)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("F", true);
    document.InsertAssignment(true);
    document.InsertString("a", true);
    document.InsertMultiply(true);
    document.InsertString("b", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("F", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"F=a*b\n" \
        U"F=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 2}, start, size)) << ErrorMarks();

    document.MoveCaretUp(false);
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretUp(false);
    document.InsertString("b", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"a=1\n" \
        U"b=5\n" \
        U"F=a*b\n" \
        U"F=5."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a variable inside a code block before
TEST_F(VariablesTest, variables17)
{
    Start(600);
    
    document.SetLocale(yutovo_calculator::Language::Russian, true);
    document.InsertCode(false, true);
    document.InsertString("h", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("E", true);
    document.InsertAssignment(true);
    document.InsertString("m", true);
    document.InsertMultiply(true);
    document.InsertString("h", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("E", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"h=5\n" \
        U"E=m*h\n" \
        U"E=Unknown identifier"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 0, 0, 0, 2, 0}, start, size)) << ErrorMarks();

    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("m", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"m=2\n" \
        U"h=5\n" \
        U"E=m*h\n" \
        U"E=10."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Move a code block with a variable
TEST_F(VariablesTest, variables18)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("p", true);
    document.InsertAssignment(true);
    document.InsertString("1", true);
    document.WaitSolver();

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("L", true);
    document.InsertAssignment(true);
    document.InsertString("p", true);
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("L", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"p=1\n" \
        U"L=p\n" \
        U"L=1."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("L", true);
    document.InsertAssignment(true);
    document.InsertString("p", true);
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("L", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"p=1\n" \
        U"L=p\n" \
        U"L=1.\n" \
        U"L=p\n" \
        U"L=1."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n" \
        U"p=1\n" \
        U"L=p\n" \
        U"L=1.\n" \
        U"L=p\n" \
        U"L=1."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Insert a paragraph over a code block with a variable
TEST_F(VariablesTest, variables19)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"v=3\n" \
        U"v=3.\n" \
        U"v=2\n" \
        U"v=2."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"\n" \
        U"v=3\n" \
        U"v=3.\n" \
        U"v=2\n" \
        U"v=2."
        ) << ToBasicString(document.ToText());
}

//Check error marks
TEST_F(VariablesTest, variables20)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.SetLocale(yutovo_calculator::Language::Russian, true);
    document.InsertString("Example 1", true);
    document.InsertParagraph(true);
    document.InsertString("Вычисление", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("Р_З", true);
    document.InsertAssignment(true);
    document.InsertString("6370", true);
    document.InsertString(" ", true);
    document.InsertString("км", true);
    document.WaitSolver();

    document.InsertParagraph(true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.InsertSquareRoot(true);
    document.InsertString("Р_З", true);
    document.InsertMultiply(true);
    document.InsertString("g_a", true);
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertString("Example 2", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("String", true));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Example 1\n" \
        U"Вычисление\n" \
        U"Р_З=6370км\n" \
        U"v=sqrt(Р_З*g_a)\n" \
        U"v=7.904(км)/(с)\n" \
        U"Example 2\n"
        U"String"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Example 1\n" \
        U"Вычисление\n" \
        U"Р_З=6370\n" \
        U"v=sqrt(Р_З*g_a)\n" \
        U"v=Units are incompatible\n" \
        U"Example 2\n"
        U"String"
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 2, 0, 0, 1, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Вычисление\n" \
        U"Р_З=6370\n" \
        U"v=sqrt(Р_З*g_a)\n" \
        U"v=Units are incompatible\n" \
        U"Example 2\n"
        U"String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 1, 0, 0, 1, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ErrorMarks();
}

//Check error marks
TEST_F(VariablesTest, variables21)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.SetLocale(yutovo_calculator::Language::Russian, true);
    document.InsertString("Example 1", true);
    document.InsertParagraph(true);
    document.InsertString("Вычисление", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("Р_З", true);
    document.InsertAssignment(true);
    document.InsertString("6370", true);
    document.InsertString(" ", true);
    document.InsertString("км", true);
    document.WaitSolver();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Example 1\n" \
        U"Вычисление\n" \
        U"Р_З=6370к" \
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 2, 0, 0, 1, 0, 0, 2}, start, size)) << ErrorMarks();
    ASSERT_TRUE(start == 0 && size == 1);
}

}
