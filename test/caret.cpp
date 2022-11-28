#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, caret1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "Text") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretRight(false);
    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 1, 3)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertText(" Word2", true);
    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    document.WaitMainLoop();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.InsertText(" Word3", true);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Courier", 24, false, true, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 0)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 0, 0, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0, 0, 4, 2, 0, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11, 11, 5, 1, 0, 4, 2, 0, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5, 5, 11, 1, 0, 4, 2, 0, 6)) << document.GetEditorState().ToString();

    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 16, 1, 0, 4, 2, 0, 6)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10, 0, 10)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 16, 0, 16)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4, 0, 4, 0, 0, 16)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6, 0, 6, 0, 0, 16, 1, 0, 4)) << document.GetEditorState().ToString();
}

}
