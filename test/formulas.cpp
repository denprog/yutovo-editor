#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/code_string.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, delete1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.WaitMainLoop();
    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.InsertPlus(true);
    document.InsertPlus(true);
    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete with undo a text and a code block
TEST_F(FormulaTest, delete2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Delete with undo a text and a code block and a text
TEST_F(FormulaTest, delete3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.InsertString("Block", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Teck</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Block</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Teck</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Block</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
}

//Delete a selected code block at the beginning of the text
TEST_F(FormulaTest, delete4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 433, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretHome(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Deletion of a selected formula
TEST_F(FormulaTest, delete5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 450, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitCaretMoving();
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of a selected formula with a text
TEST_F(FormulaTest, delete6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 17; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 14; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 16; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Th text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 2, 11},
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 0, 0, 3}, 0, 4})) << document.GetEditorState().ToString();
}

//Delete an element on the left of a string
TEST_F(FormulaTest, delete7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("55", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d55</mi>"\
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
                        "<mi>d</mi>"\
                        "<mo>+</mo>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a character from the right
TEST_F(FormulaTest, delete8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(false, true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("6", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
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
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete an element from the right
TEST_F(FormulaTest, delete9)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(false, true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertPlus(true);
    document.InsertMinus(true);
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete a code block on the left
TEST_F(FormulaTest, delete10)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 368, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 11})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, insert1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 450, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert code in text and reformat text
TEST_F(FormulaTest, insert2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 433, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert formula, remake and undo
TEST_F(FormulaTest, insert3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    std::this_thread::sleep_for(100ms);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertDivision(true));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a formula at the end of a string
TEST_F(FormulaTest, insert4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 432, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("Text Text Text Text Text Text Text Text 3", true));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("f", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text Text Text Text Text Text Text Text 3f</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTest, insert5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 432, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("56890", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>568</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>90</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Inserting a paragraph is prohibited
TEST_F(FormulaTest, insert6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 432, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertString("56", true);
    document.InsertPower(true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTest, insert7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 432, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTest, insert8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 432, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertMultiply(true);
    document.InsertString("56", true);
    document.InsertPower(true);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>×</mo>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>×</mo>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Selection of a formula
TEST_F(FormulaTest, select1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 450, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertString("25", true));
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection of a formula at the beginning of a text
TEST_F(FormulaTest, select2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 450, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("text", true);
    document.MoveCaretHome(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection of a formula at the end of a row
TEST_F(FormulaTest, select3)
{
    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 2})) << document.GetEditorState().ToString();

    width = 380;
    document.Resize(width, 400);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 2})) << document.GetEditorState().ToString();
}

//Selection of a child element of a formula at the end of a row
TEST_F(FormulaTest, select4)
{
    int width = 424;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    width = 410;
    document.Resize(width, 400);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 1, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Selection of a part of row, a formula and a part of row
TEST_F(FormulaTest, select5)
{
    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 4})) << document.GetEditorState().ToString();

    width = 380;
    document.Resize(width, 400);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 4})) << document.GetEditorState().ToString();

    width = 350;
    document.Resize(width, 400);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2, 4}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 4, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 2}, 0, 4})) << document.GetEditorState().ToString();

    width = 380;
    document.Resize(width, 400);
    document.WaitMainLoop();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 4})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.InsertString("234", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(234)\n" \
        U"(123)/(123)"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 1, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("12", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("123245345", true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 0, 3},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 2}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("12", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("123245345", true));
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 2}, 0, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, fonts1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    document.WaitTask(document.SetFontSize(12));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
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
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

}
