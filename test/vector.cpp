/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Insert a vector and undo/redo it
TEST_F(FormulaTest, vector1)
{
    Start(600);

    document.WaitTask(document.InsertVectorElement(true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Type inside the vector: the text and the parser string are the row content
TEST_F(FormulaTest, vector2)
{
    Start(600);

    document.WaitTask(document.InsertVectorElement(true));
    document.InsertString(U"a", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString(U"b", true));
    ASSERT_TRUE(document.ToText() == U"a+b") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>a</mi>"\
                                    "<mo>+</mo>"\
                                    "<mi>b</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 2, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"a+") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"a+b") << ToBasicString(document.ToText());
}

//The arrow is the first stop entering from the left; the row is entered from it
TEST_F(FormulaTest, vector3)
{
    Start(600);

    document.WaitTask(document.InsertVectorElement(true));
    document.InsertString(U"a", true);
    document.WaitTask(document.MoveCaretLeft(false)); //before "a", still inside the vector row
    const std::string row_start = document.GetEditorState().ToString();

    //leaving left: the single stop left of the row is the arrow shape - Backspace there dissolves the vector
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0 []") << document.GetEditorState().ToString();

    //from the arrow right goes straight into the row
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == row_start)
        << document.GetEditorState().ToString() << " != " << row_start;

    //one right from the row end leaves the element into the containing row
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,1 []") << document.GetEditorState().ToString();

    //an equation can be appended right away - it swallows the vector into the left part
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    ASSERT_TRUE(document.ToText() == U"a=~") << ToBasicString(document.ToText());

    //a single left press from the equation returns to the same position after the vector
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0,1 []") << document.GetEditorState().ToString();

    //entering from the right returns straight into the row end - no stop on the arrow
    //(the vector id got one component longer when the equation swallowed it)
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0,0,1,0,1 []") << document.GetEditorState().ToString();
}

//Selected elements are wrapped into the vector row, the vector takes their place
TEST_F(FormulaTest, vector4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString(U"x", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString(U"1", true));

    //select x+1 and wrap it
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.InsertVectorElement(true));
    ASSERT_TRUE(document.ToText() == U"x+1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                    "<mo>+</mo>"\
                                    "<mi>1</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                        "<mo>+</mo>"\
                        "<mi>1</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                    "<mo>+</mo>"\
                                    "<mi>1</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    //the selected row content is cleared back to the placeholder
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"x+1") << ToBasicString(document.ToText());
}

//Deleting the vector from the arrow returns the row content into the containing row
TEST_F(FormulaTest, vector5)
{
    Start(600);

    document.WaitTask(document.InsertVectorElement(true));
    document.InsertString(U"a", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString(U"b", true));
    ASSERT_TRUE(document.ToText() == U"a+b") << ToBasicString(document.ToText());

    //one left from the row start stands on the arrow shape - Backspace there dissolves the vector
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0 []") << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));

    //Backspace on the arrow dissolves the vector - the content returns into the containing row
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"a+b") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>a</mi>"\
                        "<mo>+</mo>"\
                        "<mi>b</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"a+b") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>a</mi>"\
                                    "<mo>+</mo>"\
                                    "<mi>b</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"a+b") << ToBasicString(document.ToText());

    //an empty vector dissolves back into the empty code row - like deleting any other lone formula
    document.WaitTask(document.New());
    document.WaitTask(document.InsertVectorElement(true)); //the caret starts at the empty row start
    document.WaitTask(document.MoveCaretLeft(false)); //the arrow
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0 []") << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mi></mi>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    //the insert and the delete collapse into one undo step back to the empty document
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
}

//Copy-paste the vector
TEST_F(FormulaTest, vector6)
{
    Start(600);

    document.WaitTask(document.InsertVectorElement(true));
    document.InsertString(U"a", true);
    document.WaitTask(document.MoveCaretRight(false)); //the stop after the vector
    document.WaitTask(document.MoveCaretLeft(true)); //select the whole vector
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0 [[{0},0,1]]") << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"aa") << ToBasicString(document.ToText());
    //the paste after the selected element lands after the code block - the pasted vector opens an own code block
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>a</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>a</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"aa") << ToBasicString(document.ToText());
}

//Save and load a document with the vector
TEST_F(FormulaTest, vector7)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertVectorElement(true));
    document.InsertString(U"a", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString(U"b", true));
    document.MoveCaretRight(false); //leave the vector into the containing row
    document.MoveCaretRight(false); //the stop after the vector
    document.InsertString(U"x", true);
    document.WaitTask(document.InsertString(U"y", true));
    ASSERT_TRUE(document.ToText() == U"a+bxy") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("vector1.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Load("vector1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"a+bxy") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi>a</mi>"\
                                    "<mo>+</mo>"\
                                    "<mi>b</mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'FreeMono';font-size:14px;\">xy</span>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    //after the load entering from the left stops on the arrow first, the second right enters the row
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0 []") << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,1,0,0 []") << document.GetEditorState().ToString();
}

//The vector is inserted into the document text through an own code block; the row strings are editable
TEST_F(FormulaTest, vector8)
{
    Start(600);

    document.InsertString(U"t", true); //plain text before the code block
    document.WaitTask(document.InsertVectorElement(true));
    ASSERT_TRUE(document.ToText() == U"t") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">t</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mover accent='true'>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>⃗</mo>"\
                        "</mover>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,1,0,0,0,1,0,0 []") << document.GetEditorState().ToString();

    //collect the vector children ids without descending into strings
    std::vector<ElementId> ids;
    std::function<void(const ElementPtr&)> collect =
        [&](const ElementPtr& el)
        {
            ids.push_back(el->id);
            if (el->type == ElementType::STRING || el->type == ElementType::CODE_STRING || el->type == ElementType::LINK)
                return;
            for (int i = 0; i < el->elements->Count(); ++i)
                collect(el->elements->Get(i));
        };
    collect(document.GetElement(ElementId{0}));

    bool has_row_string = false;
    for (const ElementId& element_id : ids)
    {
        //the arrow shape (child 0) is not a string - the mouse cursor stays an arrow over it
        if (element_id.size() == 8 && element_id[7] == 0)
            ASSERT_FALSE(document.IsString(element_id));
        //the strings inside the row (child 1) are editable
        if (document.IsString(element_id) && document.GetElementType(element_id) == ElementType::CODE_STRING &&
            element_id.size() == 9 && element_id[7] == 1)
        {
            has_row_string = true;
            ASSERT_TRUE(document.IsEditable(element_id)) << document.GetEditorState().ToString();
        }
    }
    ASSERT_TRUE(has_row_string) << "the row strings not found";
}

//A full pass through the vector: entering from the left stops on the arrow first, the row keeps the normal size
TEST_F(FormulaTest, vector9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString(U"x", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertVectorElement(true));
    document.InsertString(U"a", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString(U"b", true));
    ASSERT_TRUE(document.ToText() == U"x+a+b") << ToBasicString(document.ToText());

    //the row content keeps the normal size - the same level as the vector, not a shrunk level + 1
    ElementPtr vector_el = document.FindByType(ElementId{0}, ElementType::VECTOR);
    ASSERT_TRUE(vector_el != nullptr);
    ASSERT_TRUE(vector_el->elements->Get(1)->level == vector_el->level) << "the vector row level is shrunk";

    //walk left out of the vector: the row start is preceded by the arrow
    document.WaitTask(document.MoveCaretHome(false)); //the vector row start
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,0 []") << document.GetEditorState().ToString(); //the arrow
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2 []") << document.GetEditorState().ToString(); //the vector itself
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,1 []") << document.GetEditorState().ToString(); //the plus before the vector

    //the full pass right: entering the vector from the left stops on the arrow first
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,0 []") << document.GetEditorState().ToString(); //the arrow

    //from the arrow into the row, then across the three inner elements a, +, b
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,1,0,0 []") << document.GetEditorState().ToString(); //before "a"
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,1,0,1 []") << document.GetEditorState().ToString(); //after "a"
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,1,1 []") << document.GetEditorState().ToString(); //the inner plus
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,1,2,0 []") << document.GetEditorState().ToString(); //before "b"
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,2,1,2,1 []") << document.GetEditorState().ToString(); //after "b" - the row end

    //one more right leaves the vector - the last position of the containing row
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,3 []") << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToText() == U"x+a+b") << ToBasicString(document.ToText());
}

//The vector is a notation only - the equation solves the row content
TEST_F(SolverAutoTest, vector1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertVectorElement(true));
    InsertExpression(U"x+1");
    document.WaitTask(document.MoveCaretRight(false)); //leave the vector into the containing row

    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"x+1=1+x") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//A vector result is shown with the arrow over the column
TEST_F(SolverVectorTest, vector_arrow_result1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("1", true);
    document.InsertComma(true);
    document.WaitTask(document.InsertString("2", true));
    document.InsertCloseSquareBracket(true);
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[1,2]=[[1.],[2.]]") << ToBasicString(document.ToText());
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

}
