#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//sin
TEST_F(FormulaTest, functions1)
{
    document.InsertFunction("sin", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>sin</mi>" \
                        "<mo>(</mo>" \
                        "<mo>)</mo>" \
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>sin</mi>" \
                        "<mo>(</mo>" \
                        "<mo>)</mo>" \
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Recalculate a dependent expression
TEST_F(FormulaTest, functions2)
{
    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("x", true);
    document.InsertCloseFence(true);
    document.InsertAssignment(true);
    document.InsertString("x", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("5", true);
    document.InsertCloseFence(true);
    document.InsertEquation(ResultType::AUTO, true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x*2\n" \
        U"f(5)=10.") << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x*22\n"\
        U"f(5)=110.") << ToBasicString(document.ToText());
}

//Recalculate a dependent expression
TEST_F(FormulaTest, functions3)
{
    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("x", true);
    document.InsertCloseFence(true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("2", true);
    document.InsertCloseFence(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("x", true);
    document.InsertCloseFence(true);
    document.InsertAssignment(true);
    document.InsertString("x", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("3", true);
    document.InsertCloseFence(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x\n" \
        U"f(2)=2.\n" \
        U"f(x)=x+5\n" \
        U"f(3)=8.") << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("2", true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x\n" \
        U"f(2)=2.\n" \
        U"f(x)=x+25\n" \
        U"f(3)=28.") << ToBasicString(document.ToText());
    
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("4", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"f(x)=x+4\n" \
        U"f(2)=6.\n" \
        U"f(x)=x+25\n" \
        U"f(3)=28.") << ToBasicString(document.ToText());
}

}
