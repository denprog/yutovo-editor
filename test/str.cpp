#include <gtest/gtest.h>
#include "mock.h"
#include <QPainter>

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, strings1)
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.InsertText("T", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.InsertText("e", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Te</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.InsertText("x", true);
    document.InsertText("t", true);
    document.InsertText("Text", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, strings2)
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.InsertText("Text", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.InsertText("Str", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();
    document.InsertText("i", true);
    document.InsertText("n", true);
    document.InsertText("g", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "TextString") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStrin") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStri") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStri") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.InsertText("Str", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TextStriStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections1)
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStrin") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "Test") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == "TestStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "Test") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStr") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestStrin") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToText() == "TestString") << document.ToText();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Test", true);
    document.MoveCaretHome(false);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 2)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bo</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 2)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2, 0, 2, 0, 1, 3)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>B</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2, 0, 2, 0, 1, 3)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Normal", std::make_shared<StringFormat>("Arial", 16, false, false, false), true);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Courier", 24, false, true, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 9; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 3, 0, 3, 1, 0, 4, 0, 4, 2)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:16px;\">Norm</span>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em>lic</em></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.MoveCaretHome(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 6, 0, 6)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 6, 0, 6)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    for (int i = 0; i < 20; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6, 0, 6, 1, 0, 4, 0, 0, 6)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em></em></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Test", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
}

TEST_F(DocumentTest, inserts2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Test", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.InsertText("X", true);
    document.WaitCaretMoving();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.InsertText("h", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Te</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>hld</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false), true);
    document.InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false), true);
    document.InsertText("String1 String2 String3", std::make_shared<StringFormat>("Arial", 20, false, false, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    for (int i = 0; i < 4; ++i)
        document.DeleteElements(true, true, false);
    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    for (int i = 0; i < 2; ++i)
        document.MoveCaretLeft(false);
    document.InsertText("Bold", std::make_shared<StringFormat>("Arial", 24, true, false, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">xt</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">xt</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    document.MoveCaretHome(false);
    document.InsertText("Bold", std::make_shared<StringFormat>("Arial", 24, true, false, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    document.MoveCaretEnd(false);
    document.InsertText("Bold", std::make_shared<StringFormat>("Arial", 24, true, false, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontFamily("Courier New");
    document.SetFontSize(12);
    document.InsertText("C", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">C</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.InsertText("o", true);
    document.InsertText("u", true);
    document.InsertText("r", true);
    document.InsertText("i", true);
    document.InsertText("e", true);
    document.InsertText("r", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
    {
        document.Undo();
        document.WaitMainLoop();
    }
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Co</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
    {
        document.Undo();
        std::this_thread::sleep_for(10ms);
    }
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 15; ++i)
    {
        document.Redo();
        std::this_thread::sleep_for(10ms);
    }
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.SetFontFamily("Times New Roman");
    document.SetFontSize(22);
    document.SetBold(true);
    document.SetItalic(true);
    document.SetUnderline(true);
    document.InsertText(" New", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
        document.Redo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertText("Text", std::make_shared<StringFormat>("Arial", 24, false, false, false), true);
    document.InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false), true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.DeleteElements(true, true, false);
    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Tex</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>talic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

}
