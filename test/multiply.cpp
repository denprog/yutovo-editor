#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, multiply1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertMultiply(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>×</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

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
                        "<mo>×</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, multiply2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("123", true);
    document.WaitMainLoop();
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.InsertCode(false, true);
    document.InsertMultiply(true);
    document.WaitMainLoop();
    document.Save("multiply2_1.yut");

    document.New();
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.Load("multiply2_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>×</mo>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("56", true);
    document.WaitMainLoop();
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("67", true);
    document.WaitMainLoop();

    document.Save("multiply2_2.yut");

    document.New();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.Load("multiply2_2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>56</mi>"\
                        "<mo>×</mo>"\
                        "<mi>67</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

}
