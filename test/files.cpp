#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, files1)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("Text", true));
    document.Save("1.yut");
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
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
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    document.WaitTask(document.InsertString("is a little mysterious.", true));
    std::this_thread::sleep_for(1s);
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
    document.WaitTask(document.New());
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
    Start(630);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature, "\
        "a street sign, an arrangement of buildings on a city block, or styles of clothing.", true));
    std::this_thread::sleep_for(100ms);
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(100ms);
    for (int i = 0; i < 9; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"><strong>literary</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> theory,</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> a text is any object that can be read, </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">whether this object is a work of literature, a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">street sign, an arrangement of buildings on a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">city block, or styles of clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    for (int i = 0; i < 6; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.SetFontFamily("Courier New"));
    std::this_thread::sleep_for(100ms);
    for (int i = 0; i < 20; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
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

    document.WaitTask(document.Save("3.yut"));
    document.WaitTask(document.New());
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
    Start(610);

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

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });
    
    document.WaitTask(document.Save("4.yut"));
    document.WaitTask(document.New());
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

//Load a text file
TEST_F(DocumentTest, files5)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.Load("../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    auto el = document.GetElement(ElementId{0, 0, 0, 0, 0});
    ASSERT_TRUE(el->type == ElementType::STRING && el->ToText().rfind(U"Арифме́тика", 0) == 0);

    auto text = document.GetElement(ElementId{0});
    auto paragraph = document.GetElement(ElementId{0, text->elements->Count() - 1});
    el = paragraph->elements->Get(0)->elements->Get(0);
    ASSERT_TRUE(el->type == ElementType::STRING && el->ToText().rfind(U"Основам арифметики", 0) == 0);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Load a broken file
TEST_F(DocumentTest, files6)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::InputStreamError);
        });

    document.Load("../test/tests/broken_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);
}

//Load an abcent file
TEST_F(DocumentTest, files7)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::InputStreamError);
        });

    document.Load("../test/tests/abcent.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);
}

}
