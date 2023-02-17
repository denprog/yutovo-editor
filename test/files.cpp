#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, files1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", true);
    document.WaitMainLoop();
    document.Save("1.yut");
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

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
    document.Load("1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, files2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("The source of ", true);
    document.SetBold(true);
    document.SetFontFamily("Courier New");
    document.InsertString("the text ", true);
    document.SetBold(false);
    document.SetItalic(true);
    document.SetFontFamily("Times New Roman");
    document.SetFontSize(14);
    document.InsertString("itself ", true);
    document.SetFontSize(20);
    document.SetItalic(false);
    document.InsertString("is a little mysterious.", true);
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

    document.Save("2.yut");
    document.New();
    document.WaitMainLoop();
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Load("2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1000ms);
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
}

TEST_F(DocumentTest, files3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 630, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature, "\
        "a street sign, an arrangement of buildings on a city block, or styles of clothing.", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 8; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    document.SetBold(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    for (int i = 0; i < 10; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertParagraph(true);
    document.WaitMainLoop();
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    document.SetFontFamily("Courier New");
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    for (int i = 0; i < 21; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"><strong>literary</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> theory,</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"> a text</span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> is any object that</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> can be read, whether this object is a work </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">of literature, a street sign, an arrangement of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">buildings on a city block, or styles of clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Save("3.yut");
    document.WaitMainLoop();
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

    document.Load("3.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"><strong>literary</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> theory,</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"> a text</span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> is any object that</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> can be read, whether this object is a work </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">of literature, a street sign, an arrangement of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">buildings on a city block, or styles of clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

TEST_F(DocumentTest, files4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 610, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });
    
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature, "\
        "a street sign, an arrangement of buildings on a city block, or styles of clothing.", true);
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 20; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    document.InsertParagraph(true);
    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">a text is any object that can be read, whether this </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">object is a work of literature, a street sign, an </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">arrangement of buildings on a city block, or styles of </span>"
                "<span style=\"font-family:'Courier New';font-size:12px;\">clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Save("4.yut");
    document.WaitMainLoop();
    document.New();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Load("4.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">a text is any object that can be read, whether this </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">object is a work of literature, a street sign, an </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">arrangement of buildings on a city block, or styles of </span>"
                "<span style=\"font-family:'Courier New';font-size:12px;\">clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    document.MoveCaretToDocumentEnd(false);
    document.WaitCaretMoving();
    ParagraphFormat f;
    document.GetParagraphFormat(document.caret->GetElement()->id, f);
    ASSERT_TRUE(f.name == "Monospace") << f.name;
}

}
