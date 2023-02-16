#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
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

    document.SetFontSize(22);
    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.InsertString("Text", true);
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

    document.SetFontSize(22);
    std::stringstream clipboard_array;
    std::string clipboard_text;
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
    std::this_thread::sleep_for(200ms);
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

    document.SetFontSize(22);
    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.InsertString("The source of the text itself is a little ", true);
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
            return Rect{0, 0, 620, 400};
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

    document.SetFontSize(22);
    std::stringstream clipboard_array;
    std::string clipboard_text;
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretWordLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 42}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0}, 42, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.Cut(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == "The source of the text itself is a little ") << document.ToText();

    for (int i = 0; i < 5; ++i)
        document.MoveCaretWordLeft(false);
    document.Paste(clipboard_array);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == "The source of the strangetext itself is a little ") << document.ToText();
}

TEST_F(DocumentTest, clipboard5)
{
    int width = 327;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
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

    document.SetFontSize(22);
    document.Paste("The <mrow> MathML element is used to group sub-expressions");
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "The <mrow> MathML element is used to group sub-expressions") << document.ToText();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "The <mrow> MathML element is used to group sub-expressions") << document.ToText();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "") << document.ToText();

    document.Redo();
    document.WaitRedo();
    width = 480;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "") << document.ToText();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "The <mrow> MathML element is used to group sub-expressions") << document.ToText();
}

//Paste with paragraph
TEST_F(DocumentTest, clipboard6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 400, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.Paste("Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\r\n"\
        "Desde el punto de vista de la informática, es un archivo."));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">medio de un documento </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">información era ingresada </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a mano.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Desde el punto de vista de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la informática, es un </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">archivo.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 2, 0, 8)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.SetFontSize(22);
    document.Paste("Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\n"\
        "Desde el punto de vista de la informática, es un archivo.");
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">medio de un documento </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">información era ingresada </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a mano.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Desde el punto de vista de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la informática, es un </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">archivo.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 2, 0, 8)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">medio de un documento </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">información era ingresada </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a mano.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Desde el punto de vista de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la informática, es un </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">archivo.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 2, 0, 8)) << document.GetEditorState().ToString();
}

//Paste with paragraph
TEST_F(DocumentTest, clipboard7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 400, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.Paste("Paragraph1.\r\n"\
        "Paragraph2");
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph2</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(100ms);

    document.SetFontSize(22);
    document.Paste("Paragraph1.\r\n"\
        "Paragraph2.\r\n"\
        "Paragraph3");
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph2.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(100ms);

    document.SetFontSize(22);
    document.Paste("Paragraph1.\n"\
        "Paragraph2.\n"\
        "Paragraph3");
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph2.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(100ms);

    document.Paste("Paragraph1.\n"\
        "\n"\
        "Paragraph3");
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();
}

}
