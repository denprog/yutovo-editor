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

    ASSERT_TRUE(document.IsChanged() == false);
    document.WaitTask(document.InsertString("Text", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.Save("1.yut"));
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);
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
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
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
    document.WaitTask(document.SetItalic(true));
    document.SetFontFamily("Times New Roman");
    document.SetFontSize(14);
    document.WaitTask(document.InsertString("itself ", true));
    document.SetFontSize(20);
    document.SetItalic(false);
    document.WaitTask(document.InsertString("is a little mysterious.", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.IsChanged() == true);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 12}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
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

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("5678", true));

    document.WaitTask(document.Save("3.yut"));
    document.WaitTask(document.New());

    document.Load("3.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
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

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });
    
    document.WaitTask(document.Save("4.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
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
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    ParagraphFormat f;
    document.GetParagraphFormat(document.caret->GetElement()->id, f);
    ASSERT_TRUE(f.name == "Monospace") << f.name;
}

//Load a text file
TEST_F(DocumentTest, files5)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
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

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
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

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::InputStreamError);
        });

    document.Load("../test/tests/abcent.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);
}

//Check paragraph style after New
TEST_F(DocumentTest, files8)
{
    Start(600);

    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Load json from string
TEST_F(DocumentTest, files9)
{
    Start(600);

    auto json = U"{\"string_formats\":[{\"id\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\",\"family\":\"Arial\",\"size\":14,\"bold\":false,"
        "\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"d7dbac2d-be4a-4895-b0a2-57ec557622a9\","
        "\"family\":\"Arial\",\"size\":30,\"bold\":true,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},"
        "{\"id\":\"0d8fb519-1505-4eb1-a282-bd7623c09a6f\",\"family\":\"Arial\",\"size\":26,\"bold\":true,\"italic\":false,\"underline\":false,"
        "\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"f3b45b0f-dd34-4758-9fd8-02ab0c0b2cb5\",\"family\":\"Arial\",\"size\":22,"
        "\"bold\":true,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},"
        "{\"id\":\"511ee5e7-6e85-4e76-bb43-3423611c5c6a\",\"family\":\"Courier New\",\"size\":12,\"bold\":false,\"italic\":false,\"underline\":false,"
        "\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"89c69148-d1bc-4384-b009-7b752eab898c\",\"family\":\"Courier New\",\"size\":14,"
        "\"bold\":false,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295}],\"paragraph_formats\":"
        "[{\"name\":\"Text body\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,"
        "\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\"},{\"name\":\"Header 1\","
        "\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,"
        "\"spacing_after\":10,\"default_string_format\":\"d7dbac2d-be4a-4895-b0a2-57ec557622a9\"},{\"name\":\"Header 2\",\"alignment\":0,\"word_wrap\":1,"
        "\"line_spacing\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,"
        "\"default_string_format\":\"0d8fb519-1505-4eb1-a282-bd7623c09a6f\"},{\"name\":\"Header 3\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,"
        "\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":"
        "\"f3b45b0f-dd34-4758-9fd8-02ab0c0b2cb5\"},{\"name\":\"Monospace\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,"
        "\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\"511ee5e7-6e85-4e76-bb43-3423611c5c6a\"}"
        ",{\"name\":\"Code\",\"alignment\":0,\"word_wrap\":0,\"line_spacing\":2,\"indent_before\":2,\"indent_after\":2,\"indent_first_line\":0,"
        "\"spacing_before\":2,\"spacing_after\":2,\"default_string_format\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}],\"text\":{\"id\":\"0\",\"type\":1,"
        "\"elements\":[{\"id\":\"0,0\",\"type\":2,\"elements\":[{\"id\":\"0,0,0\",\"type\":3,\"elements\":[{\"id\":\"0,0,0,0\",\"type\":4,\"elements\":"
        "\"Newton's second law of motion\",\"format_id\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\"}]}],\"format_name\":\"Text body\"},{\"id\":\"0,1\",\"type\":2,"
        "\"elements\":[{\"id\":\"0,1,0\",\"type\":3,\"elements\":[{\"id\":\"0,1,0,0\",\"type\":5,\"elements\":[{\"id\":\"0,1,0,0,0\",\"type\":6,"
        "\"elements\":[{\"id\":\"0,1,0,0,0,0\",\"type\":7,\"elements\":[{\"id\":\"0,1,0,0,0,0,0\",\"type\":27,\"elements\":[{\"id\":\"0,1,0,0,0,0,0,0\","
        "\"type\":7,\"elements\":[{\"id\":\"0,1,0,0,0,0,0,0,0\",\"type\":8,\"elements\":\"mass\",\"format_id\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}]},"
        "{\"id\":\"0,1,0,0,0,0,0,1\",\"type\":10,\"elements\":[]},{\"id\":\"0,1,0,0,0,0,0,2\",\"type\":7,\"elements\":[{\"id\":\"0,1,0,0,0,0,0,2,0\","
        "\"type\":8,\"elements\":\"1kg\",\"format_id\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}]}]}]}],\"format_name\":\"Code\"}],\"code_id\":1}]}],"
        "\"format_name\":\"Text body\"}]}}";

    document.WaitTask(document.LoadJson(json, 0));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Newton's second law of motion</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>mass</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>1kg</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Split a code string with a space
TEST_F(DocumentTest, files10)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(" 123   4355 45 ", true));
    document.WaitTask(document.Save("files10.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Load("files10.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                        "<mi>123</mi>"\
                        "<mi>4355</mi>"\
                        "<mi>45</mi>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Text colors
TEST_F(DocumentTest, files11)
{
    Start(600);

    document.WaitTask(document.InsertString("Text ", true));
    document.SetColor(Color::Red());
    document.WaitTask(document.InsertString("red ", true));
    document.SetBgColor(Color::Blue());
    document.WaitTask(document.InsertString("blue", true));
    document.WaitTask(document.Save("files11.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Load("files11.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);\">red </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);bgcolor:rgba(0,0,255,255);\">blue</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 4})) << document.GetEditorState().ToString();
}

//Check is changed after undo/redo
TEST_F(DocumentTest, files12)
{
    Start(600);

    ASSERT_TRUE(document.IsChanged() == false);
    document.WaitTask(document.InsertString("Text ", true));
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.InsertString("s", true));
    document.WaitTask(document.InsertString("t", true));
    document.WaitTask(document.InsertString("ring", true));
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text st") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text s") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.IsChanged() == true);

    document.WaitTask(document.Save("files12.yut"));
    ASSERT_TRUE(document.IsChanged() == false);

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"Text st") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text s") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.IsChanged() == false);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.IsChanged() == true);
}

//Save/load with caret on a result
TEST_F(DocumentTest, files13)
{
    Start(600);

    document.config.solve_delay = 10000;

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("5678", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);

    document.WaitTask(document.Save("files13.yut"));
    document.WaitTask(document.New());

    document.Load("files13.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToText() == U"5678=~") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Save/load with a file with a language
TEST_F(DocumentTest, files14)
{
    Start(600);

    document.SetLocale(yutovo_calculator::Language::Russian, ',');
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1+j", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.Save("files14.yut"));
    document.WaitTask(document.New());
    document.SetLocale(yutovo_calculator::Language::English, '.');
    document.Load("files14.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"1+j=1.+1.j") << ToBasicString(document.ToText());
}

}
