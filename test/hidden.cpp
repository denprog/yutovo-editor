/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "formulas/result.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//A result row with "hidden": true from the document json renders as the collapsed "Show result" box
TEST_F(HiddenTest, hidden1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1.23456789", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>1.23456789</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mtext>Show result</mtext>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") 
        << document.ToHtml();
    //the caret returns onto the '=' shape after the load
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    //the text and the result type are not affected by the collapsed state
    ASSERT_TRUE(document.ToText() == U"1.23456789=1.235") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetResultType(ElementId{0, 0, 0, 0, 0, 0, 0, 2}) == ResultType::AUTO);
}

//A click on the collapsed box reveals the content, the undo collapses it back
TEST_F(HiddenTest, hidden2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1.23456789", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    ElementId result_id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    ElementPtr el = document.GetElement(result_id);
    ASSERT_TRUE(el && el->type == ElementType::AUTO_RESULT);
    ASSERT_TRUE(((ResultRow*)el.get())->IsHidden());
    Rect r = el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id));
    ASSERT_TRUE(hold_type == MouseHoldType::SHOW_HIDDEN_RESULT && hold_id == result_id);

    document.WaitTask(document.ShowHiddenResult(hold_id, true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>1.23456789</mi>"
                        "</mrow>"
                        "<mo>=</mo>"
                        "<mrow>"
                            "<mrow>"
                                "<mi>1.235</mi>"
                            "</mrow>"
                        "</mrow>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") 
        << document.ToHtml();

    //a click on the shown result is not the reveal click anymore
    ASSERT_FALSE(document.MouseLButtonDown(r.left + r.width / 2, r.top + r.height / 2, hold_type, hold_id) &&
        hold_type == MouseHoldType::SHOW_HIDDEN_RESULT);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos && document.ToHtml().find("1.235") == std::string::npos) << 
        document.ToHtml(); //the box is collapsed again

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml().find("1.235") != std::string::npos && document.ToHtml().find("Show result") == std::string::npos) << 
        document.ToHtml();
}

//The collapsed box has no caret positions, the revealed content is traversed as usual
TEST_F(HiddenTest, hidden3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    //the collapsed result is skipped: one right from the '=' shape leaves the equation
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    //from the outside the equation's own stop is reached first
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    //the revealed content is traversed as usual: rights through the left part, the '=' shape and into the result content
    document.WaitTask(document.ShowHiddenResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));
    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    ElementId caret_id = document.GetEditorState().caret_state.id;
    ASSERT_TRUE(caret_id.size() > 9 && caret_id[7] == 2 && caret_id[8] == 0) << document.GetEditorState().ToString();
}

//An edit of the left part re-solves the equation - the result stays collapsed
TEST_F(HiddenTest, hidden4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    //2+1 instead of 1+1: three lefts from the '=' shape stand after the first '1'
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    document.InsertString("2", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"2+1=3.") << ToBasicString(document.ToText());
}

//A solve error does not break the collapsed box - the error appears after the reveal
TEST_F(HiddenTest, hidden5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    //make the identifier unknown: four lefts from the '=' shape stand before the first 'a'
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.InsertString("b", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    document.WaitTask(document.ShowHiddenResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));
    ASSERT_TRUE(document.ToHtml().find("Show result") == std::string::npos) << document.ToHtml();
}

//The flag is serialized only while the result is collapsed
TEST_F(HiddenTest, hidden6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    std::string json;
    document.WaitTask(document.SaveJson(json, 0, false));
    ASSERT_TRUE(json.find("\"hidden\": true") != std::string::npos);

    //after the reveal the flag is not written anymore
    document.WaitTask(document.ShowHiddenResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));
    json.clear();
    document.WaitTask(document.SaveJson(json, 0, false));
    ASSERT_TRUE(json.find("\"hidden\"") == std::string::npos) << "the revealed result must not save the flag";
}

//The HideResult call collapses the result, the flag is saved and the undo reveals it back
TEST_F(HiddenTest, hidden8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    //enter the result - the caret position the result context menu is built from
    document.WaitTask(document.MoveCaretRight(false));
    ElementId id = document.FindCurrentParentByType(ElementType::AUTO_RESULT);
    ASSERT_FALSE(id.empty());

    document.WaitTask(document.HideResult(id, true));
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    //the caret returns onto the '=' shape - the collapsed result has no caret positions
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    //the flag is serialized while the result is collapsed
    std::string json;
    document.WaitTask(document.SaveJson(json, 0, false));
    ASSERT_TRUE(json.find("\"hidden\": true") != std::string::npos);

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml().find("Show result") == std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"1+1=2.") << ToBasicString(document.ToText());
}

//The collapsed box stays on the text line of the row at any document scale
TEST_F(HiddenTest, hidden9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(HideResultViaJson());

    Config config = document.config;
    config.scale = 2.0f;
    document.WaitTask(document.SetConfig(config, false));
    std::this_thread::sleep_for(500ms);

    //the box must be measured with the scaled format like the strings of the left part
    ElementPtr result = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0});
    ElementPtr str = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}); //the first string of the left part
    ASSERT_TRUE(result && str);
    ASSERT_TRUE(result->rect.height == str->rect.height) << "box height " << result->rect.height << " vs string height " << str->rect.height;
}

//The direct api collapses a REAL result the same way as the AUTO one
TEST_F(HiddenTest, hidden10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretRight(false)); //into the result
    ElementId id = document.FindCurrentParentByType(ElementType::REAL_RESULT);
    ASSERT_FALSE(id.empty());
    document.WaitTask(document.HideResult(id, true));

    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"1+1=2.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetResultType(ElementId{0, 0, 0, 0, 0, 0, 0, 2}) == ResultType::REAL);

    document.WaitTask(document.ShowHiddenResult(id, true));
    ASSERT_TRUE(document.ToHtml().find("Show result") == std::string::npos) << document.ToHtml();
}

//The hide call with the caret outside the result keeps the caret where it stands
TEST_F(HiddenTest, hidden11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    //the caret stands on the '=' shape
    document.WaitTask(document.HideResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    //the caret was not inside the result - it must not move
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//The hide and reveal calls on an already collapsed or already shown result do nothing
TEST_F(HiddenTest, hidden12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ElementId id{0, 0, 0, 0, 0, 0, 0, 2, 0};
    document.WaitTask(document.HideResult(id, true));
    std::string html = document.ToHtml();
    ASSERT_TRUE(html.find("Show result") != std::string::npos);

    //the second hide and the reveal with a non-result id change nothing
    document.WaitTask(document.HideResult(id, true));
    document.WaitTask(document.ShowHiddenResult(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, true)); //the left row
    ASSERT_TRUE(document.ToHtml() == html) << document.ToHtml();

    //and a reveal of a shown result changes nothing either
    document.WaitTask(document.ShowHiddenResult(id, true));
    html = document.ToHtml();
    document.WaitTask(document.ShowHiddenResult(id, true));
    ASSERT_TRUE(document.ToHtml() == html) << document.ToHtml();
}

//The redo of the hide collapses the result back
TEST_F(HiddenTest, hidden13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.WaitTask(document.HideResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml().find("Show result") == std::string::npos) << document.ToHtml();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"1+1=2.") << ToBasicString(document.ToText());
}

//Two equations: only the hidden one collapses, the other keeps its result
TEST_F(HiddenTest, hidden14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == U"1+1=2.\n2+3=5.") << ToBasicString(document.ToText());

    //hide the result of the second equation only
    document.WaitTask(document.MoveCaretRight(false)); //into the second result
    ElementId second = document.FindCurrentParentByType(ElementType::AUTO_RESULT);
    ASSERT_FALSE(second.empty());
    document.WaitTask(document.HideResult(second, true));
    std::string html = document.ToHtml();
    ASSERT_TRUE(html.find("Show result") != std::string::npos) << html;
    ASSERT_TRUE(html.find("2.") != std::string::npos) << html; //the first result is still shown
    ASSERT_TRUE(html.find("5.") == std::string::npos) << html; //the second content is not drawn
    ASSERT_TRUE(document.ToText() == U"1+1=2.\n2+3=5.") << ToBasicString(document.ToText());
}

//The .yut file round trip: the flag set through the api survives save/load
TEST_F(HiddenTest, hidden15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.WaitTask(document.HideResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));

    document.Save("hidden15_1.yut");
    document.WaitTask(document.Load("hidden15_1.yut"));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"1+1=2.") << ToBasicString(document.ToText());
}

//The redo of the hide notifies the front-end (the undo/redo buttons) and empties the redo stack
TEST_F(HiddenTest, hidden16)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.WaitTask(document.HideResult(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, true));

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.CanUndo() && document.CanRedo());

    //the redo re-executes the CallFunc task - it must notify the window like the other tasks do,
    //otherwise the redo button of the front-end stays enabled until the next caret move
    EXPECT_CALL(window_mock, OnCaretMoved).Times(testing::AtLeast(1));
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_FALSE(document.CanRedo());
    ASSERT_TRUE(document.CanUndo());
}

//The undo of the reveal click must collapse the box back even when the hide came from the api
TEST_F(HiddenTest, hidden17)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ElementId id{0, 0, 0, 0, 0, 0, 0, 2, 0};

    //the full front-end flow: caret clicks around, the right-click into the result (the menu path),
    //the hide through the api, the reveal through the mouse hold
    for (int i = 0; i < 5; ++i)
        document.MoveCaret(100 + i * 10, 60, false);
    std::this_thread::sleep_for(300ms);
    ElementPtr result_el = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0});
    Rect result_rect = result_el->GetAbsoluteRect();
    document.WaitTask(document.MoveCaret(result_rect.left + 2, result_rect.top + 2, false)); //the right-click places the caret inside the result
    ElementId current = document.FindCurrentParentByType(ElementType::AUTO_RESULT);
    ASSERT_FALSE(current.empty());
    document.WaitTask(document.HideResult(current, true));
    document.SetCaretVisible(true);
    std::this_thread::sleep_for(200ms);

    //the reveal through the mouse hold, exactly like the front-end click
    result_el = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0});
    Rect box_rect = result_el->GetAbsoluteRect();
    MouseHoldType hold_type;
    ElementId hold_id;
    ASSERT_TRUE(document.MouseLButtonDown(box_rect.left + box_rect.width / 2, box_rect.top + box_rect.height / 2, hold_type, hold_id));
    ASSERT_TRUE(hold_type == MouseHoldType::SHOW_HIDDEN_RESULT);
    document.WaitTask(document.ShowHiddenResult(hold_id, true));
    ASSERT_TRUE(document.ToHtml().find("Show result") == std::string::npos) << document.ToHtml();

    //the undo of the reveal returns the collapsed box
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"1+2=3.") << ToBasicString(document.ToText());
}

//A dependency re-solve keeps the result collapsed
TEST_F(HiddenTest, hidden7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("a", true);
    document.WaitTask(document.InsertAssignment(true));
    document.WaitSolver();
    document.InsertString("5", true);
    document.WaitSolver();
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("a", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == U"a=5\na+1=6.") << ToBasicString(document.ToText());
    ASSERT_TRUE(HideResultViaJson());

    //7 instead of 5 in the assignment above - the equation below must re-solve and stay collapsed
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    document.InsertString("7", true);
    document.WaitSolver();
    std::this_thread::sleep_for(1s);

    ASSERT_TRUE(document.ToHtml().find("Show result") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToText() == U"a=7\na+1=8.") << ToBasicString(document.ToText());
}

}
