#include <gtest/gtest.h>
#include "str.h"
#include <QPainter>

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(StringsTest, strings1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.InsertText("T", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();

    document.InsertText("e", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Te</span></p></body>") << document.ToHtml();
    document.InsertText("x", true);
    document.InsertText("t", true);
    document.InsertText("Text", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();

    document.MoveCaretHome(false);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4));

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 0)) << document.caret.GetCaretState().ToString();
}

TEST_F(StringsTest, strings2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.Undo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.Undo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();

    document.InsertText("Text", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4)) << document.caret.GetCaretState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 3)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4)) << document.caret.GetCaretState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 3)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4)) << document.caret.GetCaretState().ToString();

    document.InsertText("Str", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 7)) << document.caret.GetCaretState().ToString();
    document.InsertText("i", true);
    document.InsertText("n", true);
    document.InsertText("g", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextString") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 10)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStrin") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 9)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStri") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 8)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 7)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "Text") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4)) << document.caret.GetCaretState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 7)) << document.caret.GetCaretState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStri") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 8)) << document.caret.GetCaretState().ToString();

    document.InsertText("Str", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStriStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 11)) << document.caret.GetCaretState().ToString();
}

TEST_F(StringsTest, selections1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("TestString", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestString") << document.ToText();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 9, 9, 1)) << document.caret.GetCaretState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStrin") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 9)) << document.caret.GetCaretState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 7, 7, 2)) << document.caret.GetCaretState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 7)) << document.caret.GetCaretState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4, 4, 3)) << document.caret.GetCaretState().ToString();

    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "Test") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4, 4, 3)) << document.caret.GetCaretState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "Test") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStr") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 4, 4, 3)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStrin") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 7, 7, 2)) << document.caret.GetCaretState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestString") << document.ToText();
    ASSERT_TRUE(document.caret.GetCaretState() == MakeCaretState(0, 0, 0, 9, 9, 1)) << document.caret.GetCaretState().ToString();
}

}
