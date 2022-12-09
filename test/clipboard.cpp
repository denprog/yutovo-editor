#include <gtest/gtest.h>
#include "mock.h"
#include "editor/style.h"
#include <sstream>

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(DocumentTest, clipboard1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.InsertText("Text", true);
    document.WaitMainLoop();
    document.MoveCaretHome(true);
    document.WaitCaretMoving();
    document.Copy(clipboard_array, clipboard_text);
    document.WaitMainLoop();
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.Paste(clipboard_array);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == "Text") << clipboard_text;
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    clipboard_array.clear();
    clipboard_text = "";
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.Copy(clipboard_array, clipboard_text);
    document.WaitMainLoop();
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.Paste(clipboard_array);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextTextext</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == "ext") << clipboard_text;
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.InsertText("The source of ", true);
    document.SetBold(true);
    document.SetFontFamily("Courier New");
    document.InsertText("the text ", true);
    document.SetBold(false);
    document.SetItalic(true);
    document.SetFontFamily("Times New Roman");
    document.SetFontSize(14);
    document.InsertText("itself ", true);
    document.SetFontSize(20);
    document.SetItalic(false);
    document.InsertText("is a little mysterious.", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the text </strong></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:14px;\"><em>itself </em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">is a little </span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    clipboard_array.clear();
    clipboard_text = "";
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    document.Copy(clipboard_array, clipboard_text);
    document.WaitMainLoop();
    document.MoveCaretToDocumentEnd(false);
    document.WaitCaretMoving();
    document.Paste(clipboard_array);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the text </strong></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:14px;\"><em>itself </em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">is a little </span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">mysterious.</span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the</strong></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(clipboard_text == " of the") << clipboard_text;
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 2, 3)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.InsertText("The source of the text itself is a little ", true);
    document.WaitMainLoop();
    document.MoveCaretHome(true);
    document.WaitCaretMoving();
    document.Copy(clipboard_array, clipboard_text);
    document.WaitMainLoop();
    document.MoveCaretToDocumentEnd(false);
    document.WaitCaretMoving();
    document.Paste(clipboard_text);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "The source of the text itself is a little The source of the text itself is a little ") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 38)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.InsertText("The source of the text itself is a little strange", true);
    document.WaitMainLoop();
    document.MoveCaretWordLeft(true);
    document.Cut(clipboard_array, clipboard_text);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "The source of the text itself is a little ") << document.ToText();

    for (int i = 0; i < 5; ++i)
        document.MoveCaretWordLeft(false);
    document.Paste(clipboard_array);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "The source of the strangetext itself is a little ") << document.ToText();
}

}
