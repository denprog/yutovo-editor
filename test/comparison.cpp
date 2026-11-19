/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "formulas/equation.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Insert a less comparison into the document text
TEST_F(FormulaTest, comparison1)
{
    Start(600);

    document.WaitTask(document.InsertLess(true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi></mi>"
                        "</mrow>"
                        "<mo>&lt;</mo>"
                        "<mrow>"
                            "<mi></mi>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"<") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Type into both parts of a comparison
TEST_F(FormulaTest, comparison2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    ASSERT_TRUE(document.ToText() == U"2<") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString(U"3", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<3") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>2</mi>"
                        "</mrow>"
                        "<mo>&lt;</mo>"
                        "<mrow>"
                            "<mi>3</mi>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<3") << ToBasicString(document.ToText());
}

//The text of all six comparison signs
TEST_F(FormulaTest, comparison3)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());

    document.WaitTask(document.InsertGreater(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x>5") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());

    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x<5") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());

    document.WaitTask(document.InsertGreaterEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x⩾5") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());

    document.WaitTask(document.InsertLessEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x⩽5") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x") << ToBasicString(document.ToText());

    document.WaitTask(document.InsertNotEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x≠5") << ToBasicString(document.ToText());
}

//Deleting on the sign dissolves the comparison into the containing row
TEST_F(FormulaTest, comparison4)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"3", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<3") << ToBasicString(document.ToText());

    //move the caret onto the sign
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretLeft(false));

    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"23") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi>23</mi>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<3") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"23") << ToBasicString(document.ToText());
}

//Clear a part of the comparison by selection and delete
TEST_F(FormulaTest, comparison5)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"3", true));
    std::this_thread::sleep_for(200ms);

    //clear the right part
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<3") << ToBasicString(document.ToText());

    //clear the left part
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"<3") << ToBasicString(document.ToText());
}

//Copy-paste the comparison
TEST_F(FormulaTest, comparison6)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"3", true));
    std::this_thread::sleep_for(200ms);

    //leave the comparison and select it whole
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<32<3") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<3") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"2<32<3") << ToBasicString(document.ToText());
}

//Save and load a document with all six comparison signs
TEST_F(FormulaTest, comparison7)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison
    document.WaitTask(document.InsertString(U"a", true));
    document.WaitTask(document.InsertGreater(true));
    document.WaitTask(document.InsertString(U"b", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString(U"c", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"d", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString(U"e", true));
    document.WaitTask(document.InsertGreaterEqual(true));
    document.WaitTask(document.InsertString(U"f", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString(U"g", true));
    document.WaitTask(document.InsertLessEqual(true));
    document.WaitTask(document.InsertString(U"h", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString(U"i", true));
    document.WaitTask(document.InsertNotEqual(true));
    document.WaitTask(document.InsertString(U"j", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5a>bc<de⩾fg⩽hi≠j") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("comparison1.yut"));
    std::this_thread::sleep_for(500ms);
    document.Load("comparison1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5a>bc<de⩾fg⩽hi≠j") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow><mi>x</mi></mrow><mo>=</mo><mrow><mi>5</mi></mrow>"
                        "<mrow><mi>a</mi></mrow><mo>&gt;</mo><mrow><mi>b</mi></mrow>"
                        "<mrow><mi>c</mi></mrow><mo>&lt;</mo><mrow><mi>d</mi></mrow>"
                        "<mrow><mi>e</mi></mrow><mo>⩾</mo><mrow><mi>f</mi></mrow>"
                        "<mrow><mi>g</mi></mrow><mo>⩽</mo><mrow><mi>h</mi></mrow>"
                        "<mrow><mi>i</mi></mrow><mo>≠</mo><mrow><mi>j</mi></mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();
}

//A comparison inside a text block is not converted and keeps its editable right part
TEST_F(FormulaTest, comparison8)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertTextBlock(true));
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertEqual(true));
    document.WaitTask(document.InsertString(U"5", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5") << ToBasicString(document.ToText());

    //typing "=" inside a text block converts an equation, the comparison stays a comparison
    document.WaitTask(document.MoveCaretRight(false)); //leave the comparison
    document.WaitTask(document.InsertString(U"y", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitTask(document.InsertString(U"2", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5y=2") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5y=") << ToBasicString(document.ToText());
}

//A second comparison typed inside the right part of the first one nests there
TEST_F(FormulaTest, comparison9)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"3", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"1<x<3") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"1<x<") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"1<x") << ToBasicString(document.ToText());
}

//A comparison after an assignment wraps only the elements after the assignment
TEST_F(FormulaTest, comparison10)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"x", true));
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString(U"5", true));
    document.WaitTask(document.MoveCaretRight(false)); //leave the assignment
    document.WaitTask(document.InsertString(U"y", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"2", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5y<2") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x=5y<") << ToBasicString(document.ToText());
}

//A comma typed in the right part of a comparison separates the arguments of the enclosing if call -
//it goes into the containing row after the comparison, a comma inside brackets of the operand stays in it
TEST_F(FormulaTest, comparison11)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"if", true));
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertComma(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"if(1<2,") << ToBasicString(document.ToText());
    //the caret stays in the containing row - the comma is its child, not a child of the comparison
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    //a comma inside the brackets of a function call in the right part stays in the operand
    document.WaitTask(document.InsertString(U"f", true));
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"3", true));
    document.WaitTask(document.InsertComma(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"if(1<2,f(3,") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"if(1<2,f(3") << ToBasicString(document.ToText());
}

//A comma typed in the right part of an assignment separates the arguments of the enclosing for head -
//it goes into the containing row after the assignment
TEST_F(FormulaTest, comparison12)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"for", true));
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"i", true));
    document.WaitTask(document.InsertAssignment(true));
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.InsertComma(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"for(i=1,") << ToBasicString(document.ToText());
    //the caret stays in the containing row - the comma is its child, not a child of the assignment
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//A naive left-to-right typed if with a comparison element: the wrap of the comparison stops at the open
//bracket of the call, the comma leaves the operand - the head solves as a whole
TEST_F(FormulaTest, comparison13)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(U"if", true));
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.InsertLess(true));
    document.WaitTask(document.InsertString(U"2", true));
    document.WaitTask(document.InsertComma(true));
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"3", true));
    document.InsertCloseRoundBracket(true);
    document.InsertComma(true);
    document.InsertOpenRoundBracket(true);
    document.WaitTask(document.InsertString(U"4", true));
    document.InsertCloseRoundBracket(true);
    document.InsertCloseRoundBracket(true);
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"if(1<2,(3),(4))") << ToBasicString(document.ToText());

    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    {
        auto eq13 = document.FindByType(ElementId{0}, ElementType::EQUATION);
        std::cout << "DEBUG c13 parser: [" << ToBasicString(((Equation*)eq13.get())->last_expression.Text()) << "]"
                  << " text: " << ToBasicString(document.ToText()) << std::endl;
    }
    ASSERT_TRUE(document.ToText() == U"if(1<2,(3),(4))=3.") << ToBasicString(document.ToText());
}

}
