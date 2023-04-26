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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_array));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == U"Text") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    clipboard_array.clear();
    clipboard_text = U"";
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_array));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextTextext</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == U"ext") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
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
    std::this_thread::sleep_for(400ms);
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
    clipboard_text = U"";
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    document.Copy(clipboard_array, clipboard_text);
    document.WaitMainLoop();
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_array));
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
    ASSERT_TRUE(clipboard_text == U" of the") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 2, 3)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.WaitTask(document.InsertString("The source of the text itself is a little ", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_text));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 38)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 620, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 42, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.Cut(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());

    for (int i = 0; i < 5; ++i)
        document.MoveCaretWordLeft(false);
    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the strangetext itself is a little ") << ToBasicString(document.ToText());
}

TEST_F(DocumentTest, clipboard5)
{
    int width = 327;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    document.WaitTask(document.Paste(U"The <mrow> MathML element is used to group sub-expressions"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The <mrow> </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">MathML element is </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">used to group </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">sub-expressions</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    std::this_thread::sleep_for(200ms);
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The <mrow> MathML element is used to group sub-expressions") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    width = 480;
    document.Resize(width, 400);
    std::this_thread::sleep_for(100ms);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The <mrow> MathML element is used to group sub-expressions") << ToBasicString(document.ToText());
}

//Paste with paragraph
TEST_F(DocumentTest, clipboard6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 400, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.Paste(U"Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\r\n"\
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
    document.WaitTask(document.Paste(U"Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\n"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.Paste(U"Paragraph1.\r\n"\
        "Paragraph2"));
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
    document.WaitTask(document.Paste(U"Paragraph1.\r\n"\
        "Paragraph2.\r\n"\
        "Paragraph3"));
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
    document.WaitTask(document.Paste(U"Paragraph1.\n"\
        "Paragraph2.\n"\
        "Paragraph3"));
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

    document.WaitTask(document.Paste(U"Paragraph1.\n"\
        "\n"\
        "Paragraph3"));
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
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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

//Cut/Paste at the beginning of the second row
TEST_F(DocumentTest, clipboard8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Cut(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little strange") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();
    
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little strange") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block
TEST_F(DocumentTest, clipboard9)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();

    std::stringstream clipboard_array;
    std::u32string clipboard_text;
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.Paste(clipboard_array));
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block after a text
TEST_F(DocumentTest, clipboard10)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitCaretMoving();

    std::stringstream clipboard_array;
    std::u32string clipboard_text;
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.InsertString("Text", true);
    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy/Paste of different code blocks
TEST_F(DocumentTest, clipboard11)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("56", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);

    std::stringstream clipboard_array;
    std::u32string clipboard_text;
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.WaitCaretMoving();

    document.WaitTask(document.Paste(clipboard_array));
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
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();

    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.WaitCaretMoving();

    document.WaitTask(document.Paste(clipboard_array));
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
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block inside text
TEST_F(DocumentTest, clipboard12)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretHome(false);
    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();

    std::stringstream clipboard_array;
    std::u32string clipboard_text;
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">e</span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\"> so</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 3})) << document.GetEditorState().ToString();
}

//Insert a part of a formula into text
TEST_F(DocumentTest, clipboard13)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();

    std::stringstream clipboard_array;
    std::u32string clipboard_text;
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();

    document.WaitTask(document.Paste(clipboard_array));
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
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>23</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a part of a formula into text
TEST_F(DocumentTest, clipboard14)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();

    std::stringstream clipboard_array;
    std::u32string clipboard_text;
    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();

    document.WaitTask(document.Paste(clipboard_array));
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard15)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 500, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.InsertString("The source of the text itself is a little strange.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 3, 0, 0, 5})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard16)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 500, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Text.", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_array, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.The source of the text itself is </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 5})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 5})) << document.GetEditorState().ToString();
}

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard17)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 340, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
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
    std::u32string clipboard_text;
    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_array, clipboard_text));

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_array));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little strange.The source of the </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">text itself is a little strange.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 2, 0, 32})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little strange.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 15})) << document.GetEditorState().ToString();
}

}
