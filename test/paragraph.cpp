#include <gtest/gtest.h>
#include "mock.h"
#include <QPainter>

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

struct ParagraphTest : DocumentTest
{
};

TEST_F(ParagraphTest, resizing1)
{
    int width = 476;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 22, false, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false), true);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.InsertText("String1 String2 String3", std::make_shared<StringFormat>("Arial", 20, false, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    width = 400;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 7)) << document.GetEditorState().ToString();

    width = 390;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 15)) << document.GetEditorState().ToString();

    width = 290;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 23)) << document.GetEditorState().ToString();

    width = 220;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 15)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitCaretMoving();
    width = 240;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 8)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    width = 220;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 1)) << document.GetEditorState().ToString();

    width = 240;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 9)) << document.GetEditorState().ToString();

    width = 290;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 9)) << document.GetEditorState().ToString();

    width = 330;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 1)) << document.GetEditorState().ToString();

    width = 200;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 3, 0, 1)) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, resizing2)
{
    int width = 476;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 22, false, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false), true);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.InsertText("String1 String2 String3", std::make_shared<StringFormat>("Arial", 20, false, false, false), true);
    document.WaitMainLoop();
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 21, 21, 2)) << document.GetEditorState().ToString();

    width = 440;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 5, 5, 2)) << document.GetEditorState().ToString();

    width = 476;
    document.Resize(width, 400);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 8, 8, 7)) << document.GetEditorState().ToString();

    width = 440;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 8, 8, 7)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(true);
    width = 390;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 3}, 3, 5}, 
        ElementSelectionState{ElementId{0, 0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    width = 420;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 3, 3, 12)) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, paragraph1)
{
    int width = 476;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 22, false, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false), true);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.InsertText("String1 String2 String3", std::make_shared<StringFormat>("Arial", 20, false, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    document.InsertParagraph(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretUp(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.InsertParagraph(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ita</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>lic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 3)) << document.GetEditorState().ToString();

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.InsertParagraph(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.InsertParagraph(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(3, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(3, 0, 0, 0)) << document.GetEditorState().ToString();
}

}
