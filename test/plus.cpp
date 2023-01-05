#include <gtest/gtest.h>
#include "mock.h"
#include "editor/style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, plus1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertPlus(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(true);
    document.InsertText("123", true);
    document.InsertPlus(true);
    document.InsertText("45", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>45</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.InsertPlus(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 0, 3}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(true);
    document.InsertPlus(true);
    document.InsertPlus(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(true);
    document.InsertText("123", true);
    document.InsertPlus(true);
    document.InsertText("45", true);
    document.InsertPlus(true);
    document.InsertText("3456", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3456</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 4, 4})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 2, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 2, 2}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 4, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 4, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 4}, 0, 1})) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 4, 4}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1},
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 4}, 0, 4})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1}, ElementSelectionState{{0, 0, 0, 0, 0, 4}, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 2, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1}, ElementSelectionState{{0, 0, 0, 0, 0, 4}, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 4}, 0, 3}, ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 2}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1}, ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0}, 3, 1}, ElementSelectionState{{0, 0, 0, 0, 0, 4}, 0, 3}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 2}, 0, 2}, ElementSelectionState{{0, 0, 0, 0, 0}, 1, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, plus5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertCode(true);
    document.InsertText("123", true);
    document.WaitMainLoop();
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.InsertPlus(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                        "<mo>+</mo>"\
                        "<mi>3</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

}
