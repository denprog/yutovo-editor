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
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == U"Text") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    clipboard_json = U"";
    clipboard_text = U"";
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.InsertString("The source of ", true);
    document.SetBold(true);
    document.SetFontFamily("Courier New");
    document.WaitTask(document.InsertString("the text ", true));
    document.SetBold(false);
    document.SetItalic(true);
    document.WaitTask(document.SetFontFamily("Times New Roman"));
    document.WaitTask(document.SetFontSize(14));
    document.WaitTask(document.InsertString("itself ", true));
    document.SetFontSize(20);
    document.WaitTask(document.SetItalic(false));
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
    
    clipboard_json = U"";
    clipboard_text = U"";
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little ", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.PasteText(std::move(clipboard_text)));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 38)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard4)
{
    Start(620);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 42, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());

    for (int i = 0; i < 5; ++i)
        document.MoveCaretWordLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the strangetext itself is a little ") << ToBasicString(document.ToText());
}

TEST_F(DocumentTest, clipboard5)
{
    Start(327);
    int width = 327;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
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
    document.WaitTask(document.PasteText(std::u32string(U"The <mrow> MathML element is used to group sub-expressions")));
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
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
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
    document.WaitTask(document.Resize(width, 400));
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
    Start(400);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(U"Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\r\n"\
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
    document.WaitTask(document.PasteText(std::u32string(U"Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\n"\
        "Desde el punto de vista de la informática, es un archivo.")));
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
    Start(400);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(U"Paragraph1.\r\n"\
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
    document.WaitTask(document.PasteText(U"Paragraph1.\r\n"\
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
    document.WaitTask(document.PasteText(U"Paragraph1.\n"\
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

    document.WaitTask(document.PasteText(U"Paragraph1.\n"\
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
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little strange") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little strange") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block
TEST_F(DocumentTest, clipboard9)
{
    Start(600);

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
    document.WaitTask(document.MoveCaretRight(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

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
    document.WaitTask(document.MoveCaretRight(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.InsertString("Text", true);
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

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
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("56", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
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
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

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
    document.WaitTask(document.MoveCaretWordRight(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretRight(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

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
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
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
    Start(600);

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
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
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

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard15)
{
    Start(500);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.InsertString("The source of the text itself is a little strange.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(500);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Text.", true));
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
        "</body>") << 
        document.ToHtml();
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_json));
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
    Start(340);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
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

//Copy/Paste between documents
TEST_F(TwoDocumentsTest, clipboard18)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    EXPECT_CALL(window_mock2, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock2, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));

    document2.WaitTask(document2.Paste(clipboard_json));
    ASSERT_TRUE(document2.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document2.ToHtml();
    ASSERT_TRUE(clipboard_text == U"Text") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document2.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document2.GetEditorState().ToString();

    clipboard_json = U"";
    clipboard_text = U"";
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));

    document2.WaitTask(document2.Paste(clipboard_json));
    ASSERT_TRUE(document2.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Textext</span>"\
            "</p>"\
        "</body>") << 
        document2.ToHtml();
    ASSERT_TRUE(clipboard_text == U"ext") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document2.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document2.GetEditorState().ToString();
}

//Copy-paste rows
TEST_F(DocumentTest, clipboard19)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whetherIn literary theory, a text"\
        " is any object that can be read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Copy-paste rows
TEST_F(DocumentTest, clipboard20)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"In literaryIn literary theory, a text is any object that can be read, whether theory, a text"\
        " is any object that can be read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 20})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11})) << document.GetEditorState().ToString();
}

//Copy-paste rows
TEST_F(DocumentTest, clipboard21)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.PasteText(std::move(clipboard_text)));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be In literary theory, a text is any object"\
        " that can be read, whetherread, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 7})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 53})) << document.GetEditorState().ToString();
}

//Copy-paste a paragraph
TEST_F(DocumentTest, clipboard22)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature"\
        "In literary theory, a text is any object that can be read, whether this "\
        "object is a work of literature\nText") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 49})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this "\
        "object is a work of literature\nText") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste a text with spaces inside a code block
TEST_F(DocumentTest, clipboard23)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.WaitTask(document.PasteText(U" 123 45"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                        "<mi>123</mi>"\
                        "<mi>45</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste an empty and wrong json
TEST_F(DocumentTest, clipboard24)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::PasteError);
        });

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    clipboard_json = U"wrong";
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste after a code block
TEST_F(DocumentTest, clipboard25)
{
    Start(600);

    document.InsertString("String", true);
    document.InsertDivision(true);
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretHome(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretRight(false));
    EditorState s = document.GetEditorState();
    ASSERT_TRUE(document.IsEditable(s.caret_state.id));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"String()/()String()/()"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"String()/()"
        ) << ToBasicString(document.ToText());
}

//Paste inside a code block
TEST_F(DocumentTest, clipboard26)
{
    Start(600);

    document.InsertString("String", true);
    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"String()/()ring()/()"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"String()/()"
        ) << ToBasicString(document.ToText());
}

//Paste a division in a division
TEST_F(DocumentTest, clipboard27)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"()/(()/())"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Paste a division in a code block
TEST_F(DocumentTest, clipboard28)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"()/()()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Paste a division in a code block
TEST_F(DocumentTest, clipboard29)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"()/()()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Paste paragraphs in a code block
TEST_F(DocumentTest, clipboard30)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertPlus(true);
    document.InsertString("35", true);
    document.InsertParagraph(true);
    document.InsertString("1234", true);
    document.InsertPlus(true);
    document.InsertString("5678", true);
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretUp(true);
    document.MoveCaretUp(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"234+35\n"
        U"1234+5678\n"
        U"234+35\n"
        U"1234+5678"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3, 0, 2, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"234+35\n"
        U"1234+5678\n"
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste in second row
TEST_F(DocumentTest, clipboard31)
{
    Start(280);

    document.InsertString("123", true);
    document.InsertDivision(true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.Paste(clipboard_json);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        U"123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Paste in second row
TEST_F(DocumentTest, clipboard32)
{
    Start(280);

    document.InsertString("123", true);
    document.InsertDivision(true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.Paste(clipboard_json);
    document.Paste(clipboard_json);
    document.Paste(clipboard_json);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        U"123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        U"123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2})) << document.GetEditorState().ToString();
}

//Copy-paste between paragraphs
TEST_F(DocumentTest, clipboard33)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertString("that can be read", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 9})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literarybe read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11})) << document.GetEditorState().ToString();
}

//Copy-paste an image
TEST_F(DocumentTest, clipboard34)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    QImage test_image("../test/tests/Qt_small.png");
    test_image.convertTo(QImage::Format_ARGB32);
    std::vector<unsigned char> data(test_image.bits(), test_image.bits() + test_image.sizeInBytes());
    document.InsertImage(data, test_image.width(), test_image.height(), true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Paste an image
TEST_F(DocumentTest, clipboard35)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("Text", true);

    QImage test_image("../test/tests/Qt_small.png");
    test_image.convertTo(QImage::Format_ARGB32);
    std::vector<unsigned char> data(test_image.bits(), test_image.bits() + test_image.sizeInBytes());

    document.PasteImage(data, test_image.width(), test_image.height());
    document.WaitTask(document.InsertString("String", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 6})) << document.GetEditorState().ToString();
}

//Paste images one by one
TEST_F(DocumentTest, clipboard36)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    QImage test_image("../test/tests/Qt_small.png");
    test_image.convertTo(QImage::Format_ARGB32);
    std::vector<unsigned char> data(test_image.bits(), test_image.bits() + test_image.sizeInBytes());

    document.PasteImage(data, test_image.width(), test_image.height());
    document.WaitTask(document.PasteImage(data, test_image.width(), test_image.height()));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.PasteImage(data, test_image.width(), test_image.height()));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste of code blocks
TEST_F(DocumentTest, clipboard37)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("55", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();

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
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Paste a rational result
TEST_F(DocumentTest, clipboard38)
{
    Start(600);

    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::Improper;
    document.SetConfig(config, true);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("34", true);
    document.InsertDivision(true);
    document.InsertString("567", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)"\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)\n"\
        U"(69775)/(567)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)\n"\
        U"(69775)/(567)"
        ) << ToBasicString(document.ToText());
}

//Paste an auto result
TEST_F(DocumentTest, clipboard39)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123=123."\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"123."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"123."
        ) << ToBasicString(document.ToText());
}

//Paste a complex result
TEST_F(DocumentTest, clipboard40)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("1.", true);
    document.InsertPlus(true);
    document.InsertString("3.i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i\n"\
        U"-1.+3.i"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i\n"\
        U"-1.+3.i"
        ) << ToBasicString(document.ToText());
}

//Paste a real result
TEST_F(DocumentTest, clipboard41)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenFence(true);
    document.InsertString("1", true);
    document.InsertCloseFence(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 10; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U"1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U"1.571(rad)"
        ) << ToBasicString(document.ToText());
}

//Paste a real result
TEST_F(DocumentTest, clipboard42)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2.3", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3\n"\
        U"2.3"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3\n"\
        U"2.3"
        ) << ToBasicString(document.ToText());
}

//Paste an integer result
TEST_F(DocumentTest, clipboard43)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = true;
    document.SetConfig(config, true);

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 9; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)\n"\
        U"2345(dec)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)\n"\
        U"2345(dec)"
        ) << ToBasicString(document.ToText());
}

//Paste an integer result
TEST_F(DocumentTest, clipboard44)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.SetConfig(config, true);

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345"\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345\n"\
        U"2345"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345\n"\
        U"2345"
        ) << ToBasicString(document.ToText());
}

//Paste rows from a code block outside of it
TEST_F(DocumentTest, clipboard45)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("5678", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("789", true));

    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"5678\n"\
        U"789\n"\
        U"123=123.\n"\
        U"5678"
        ) << ToBasicString(document.ToText());
}

//Paste rows from a code block outside of it
TEST_F(DocumentTest, clipboard46)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("5678", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("789", true));

    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123\n"\
        U"5678=5678.\n"\
        U"789\n"\
        U"123\n"\
        U"5678=5678."
        ) << ToBasicString(document.ToText());
}

//Undo of cutting a code block
TEST_F(DocumentTest, clipboard47)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretEnd(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"123=123.") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
}

}
