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

//Insert a limit and edit it
TEST_F(FormulaTest, limit1)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munder>"\
                            "<mi>lim</mi>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>→</mo>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munder>"\
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
                        "<munder>"\
                            "<mi>lim</mi>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mo>→</mo>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munder>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    //fill the variable, the value and the function after the limit
    document.InsertString(U"x", true);
    document.MoveCaretRight(false); //cross the arrow into the value row
    document.InsertString(U"0", true);
    document.MoveCaretRight(false); //leave the element into the containing row
    document.InsertString(U"x", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString(U"1", true));
    ASSERT_TRUE(document.ToText() == U"lim[x→0]x+1") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]x+") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]x+1") << ToBasicString(document.ToText());
}

//The lim text and the arrow are not caret stops
TEST_F(FormulaTest, limit2)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true); //the variable, the caret after "x"
    document.MoveCaretLeft(false); //before "x", still inside the variable row
    const std::string variable_start = document.GetEditorState().ToString();

    //leaving left: the single stop left of the rows is the limit itself - no stop on the lim text
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0 []") << document.GetEditorState().ToString();

    //entering from the left lands directly in the variable row - no stop on the lim text
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == variable_start)
        << document.GetEditorState().ToString() << " != " << variable_start;

    //one right from the variable crosses the arrow into the value row
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,3,0,0 []") << document.GetEditorState().ToString();
    document.WaitTask(document.InsertString(U"0", true));

    //one more right leaves the element into the containing row - the only position after the limit
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,1 []") << document.GetEditorState().ToString();

    //an equation can be appended right away
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    ASSERT_TRUE(document.ToText() == U"lim[x→0]=~") << ToBasicString(document.ToText());

    //a single left press from the equation returns to the same position after the limit
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0,1 []") << document.GetEditorState().ToString();

    //entering from the right returns straight into the value row - no stop on the arrow
    //(the limit id got one component longer when the equation swallowed it)
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0,0,3,0,1 []") << document.GetEditorState().ToString();
}

//The approach direction is a power of the value
TEST_F(FormulaTest, limit3)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.WaitTask(document.InsertPower(true));
    document.WaitTask(document.InsertPlus(true));
    document.MoveCaretRight(false); //leave the exponent
    document.MoveCaretRight(false); //the stop after the limit
    document.WaitTask(document.InsertString(U"1", true));
    document.WaitTask(document.InsertDivision(true));
    document.WaitTask(document.InsertString(U"x", true));

    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,+)](1)/(x)") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munder>"\
                            "<mi>lim</mi>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>→</mo>"\
                                "<mrow>"\
                                    "<msup>"\
                                        "<mrow>"\
                                            "<mi>0</mi>"\
                                        "</mrow>"\
                                        "<mrow>"\
                                            "<mo>+</mo>"\
                                        "</mrow>"\
                                    "</msup>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munder>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,+)](1)/()") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→pow(0,+)](1)/(x)") << ToBasicString(document.ToText());
}

//Copy-paste the limit
TEST_F(FormulaTest, limit4)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0 [[{0},0,1]]") << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"lim[→]lim[→]") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[→]") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[→]lim[→]") << ToBasicString(document.ToText());
}

//Clear the rows by selection and delete
TEST_F(FormulaTest, limit5)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"0", true));
    ASSERT_TRUE(document.ToText() == U"lim[x→0]") << ToBasicString(document.ToText());

    //clear the value row
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"lim[x→]") << ToBasicString(document.ToText());

    //clear the variable row
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"lim[→]") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→]") << ToBasicString(document.ToText());
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]") << ToBasicString(document.ToText());
}

//Save and load a document with the limit
TEST_F(FormulaTest, limit6)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"x", true);
    document.WaitTask(document.InsertString(U"y", true));
    ASSERT_TRUE(document.ToText() == U"lim[x→0]xy") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("limit1.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Load("limit1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]xy") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munder>"\
                            "<mi>lim</mi>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>→</mo>"\
                                "<mrow>"\
                                    "<mi>0</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munder>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'FreeMono';font-size:12px;\">xy</span>"\
            "</p>"\
        "</body>") <<
        document.ToHtml();

    //the lim text and the arrow stay non-editable and skipped after the load
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,1,0,0 []") << document.GetEditorState().ToString();
}

//The lim text and the arrow are not editable - the mouse cursor stays an arrow over them
TEST_F(FormulaTest, limit7)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"0", true));

    //collect the limit children ids without descending into strings
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

    bool has_lim = false;
    bool has_variable_string = false;
    for (const ElementId& element_id : ids)
    {
        if (!document.IsString(element_id))
            continue;
        if (document.GetElementType(element_id) != ElementType::CODE_STRING)
            continue;
        //the direct string children of the limit are the lim text (0) and the arrow (2) - not editable;
        //the strings inside the var/value rows are plain editable strings
        if (element_id.size() == 8 && (element_id[7] == 0 || element_id[7] == 2))
        {
            has_lim = true;
            ASSERT_FALSE(document.IsEditable(element_id)) << document.GetEditorState().ToString();
        }
        else if (element_id.size() == 9 && (element_id[7] == 1 || element_id[7] == 3))
        {
            has_variable_string = true;
            ASSERT_TRUE(document.IsEditable(element_id)) << document.GetEditorState().ToString();
        }
    }
    ASSERT_TRUE(has_lim) << "the lim/arrow strings not found";
    ASSERT_TRUE(has_variable_string) << "the row strings not found";
}

//Delete the whole limit by Backspace and Delete around it
TEST_F(FormulaTest, limit8)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"0", true));
    ASSERT_TRUE(document.ToText() == U"lim[x→0]") << ToBasicString(document.ToText());

    //Backspace after the limit deletes it, the following row content stays
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"f", true));
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretEnd(false));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"f") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→0]f") << ToBasicString(document.ToText());
}

//An incomplete limit does not solve and shows no error
TEST_F(FormulaTest, limit9)
{
    Start(600);

    document.WaitTask(document.InsertLimit(true));
    document.InsertString(U"x", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"x", true);
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    //the value row is empty - the limit changes nothing and the function solves on its own
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"lim[x→]x=x") << ToBasicString(document.ToText());
}

}
