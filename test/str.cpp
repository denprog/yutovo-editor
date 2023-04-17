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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.InsertString("T", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.InsertString("e", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Te</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.InsertString("x", true);
    document.InsertString("t", true);
    document.InsertString("Text", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
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
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, strings2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.SetFontSize(22);
    document.InsertString("Text", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.InsertString("Str", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();
    document.InsertString("i", true);
    document.InsertString("n", true);
    document.InsertString("g", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TextString") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TextStrin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TextStri") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"TextStri") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.InsertString("Str", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TextStriStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("TestString", true));
    ASSERT_TRUE(document.ToText() == U"TestString") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToText() == U"TestStrin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToText() == U"Test") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"Test") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TestStrin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TestString") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("Test", true);
    document.MoveCaretHome(false);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
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
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bo</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
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
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>B</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Normal", document.GetStringFormat("Arial", 16, false, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
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
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:16px;\">Norm</span>"\
        "<span style=\"font-family:'Courier';font-size:24px;\"><em>lic</em></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
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
    document.WaitUndo();
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
        "<span style=\"font-family:'Arial';font-size:16px;\"></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Selection of rows
TEST_F(DocumentTest, selections4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.Load("../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 61})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 56}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 11})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("Test", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("Test", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.InsertString("X", true);
    document.WaitCaretMoving();
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
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
    document.WaitCaretMoving();
    document.InsertString("h", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:22px;\">Teh</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>ld</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
        "</p></body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"></span>"\
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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    for (int i = 0; i < 4; ++i)
        document.DeleteElements(true, true, false);
    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
        "</p></body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    for (int i = 0; i < 2; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false), true));
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
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitCaretMoving();
    document.InsertString("T", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertString("d", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">TText</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Boldd</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Arial';font-size:24px;\">TText</span>"\
        "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Boldd</strong></span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();
}

//Insert a string wider than the screen and add chars
TEST_F(DocumentTest, inserts8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 300, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_isa", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString(" ", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString("l", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">l</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Insert a string wider than the screen
TEST_F(DocumentTest, inserts9)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 300, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_isa", true));
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
}

TEST_F(DocumentTest, fonts1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontFamily("Courier New");
    document.SetFontSize(12);
    document.WaitTask(document.InsertString("C", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">C</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.InsertString("o", true);
    document.InsertString("u", true);
    document.InsertString("r", true);
    document.InsertString("i", true);
    document.InsertString("e", true);
    document.WaitTask(document.InsertString("r", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
    {
        document.Undo();
        document.WaitUndo();
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
        "<span style=\"font-family:'Courier New';font-size:12px;\"></span>"\
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
    document.InsertString(" New", true);
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
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body><p>"\
        "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
        "</p></body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

TEST_F(DocumentTest, fonts2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Tex</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;\">t</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3, 3, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;text-decoration: underline;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Courier New", 14, false, true, false), true);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>xt</strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\"><strong><em>Ita</em></strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0, 0, 2, 2, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 3)) << document.GetEditorState().ToString();
}

//Insert text, code and text with the same font as previous text
TEST_F(DocumentTest, fonts4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("Text", true);
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("T", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">T</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 1)) << document.GetEditorState().ToString();
}

//Insert code and text with the same font as next text
TEST_F(DocumentTest, fonts5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.SetFontSize(22);
    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("T", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">T</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.DeleteElements(true, true, false);
    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Tex</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>talic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>talic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ialic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>talic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Normal", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.WaitMainLoop();
    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Itali</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ital</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ital</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>ta</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Normal", document.GetStringFormat("Arial", 22, false, false, false), true);
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true, false);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(true, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 6)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">N</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 1)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 6)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">N</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 0)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.InsertString("orm", true);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(false, true, false));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">N</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 1)) << document.GetEditorState().ToString();
}

//Delete of Utf-8 characters
TEST_F(DocumentTest, delete4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("La versión 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.", true);
    document.WaitMainLoop();
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 8; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToText() == U"La versin 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"La versión 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.") << 
        ToBasicString(document.ToText());
}

//Delete 3 rows
TEST_F(DocumentTest, delete5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 390, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 41}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 41},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 20})) << document.GetEditorState().ToString();
}

//Delete all
TEST_F(DocumentTest, delete6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 368, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Concatinate a row below, with result wider then the window
TEST_F(DocumentTest, delete7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 330, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">alittle mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 35})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">alittle mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 34})) << document.GetEditorState().ToString();

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
}

//Concatinate a row below with a row wider then the window
TEST_F(DocumentTest, delete8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 227, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.WaitTask(document.InsertString("The_source_of_the_text_itself_is a little mysterious.", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Concatinate a row below with a row wider then the window
TEST_F(DocumentTest, delete9)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 300, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.WaitTask(document.InsertString("The_source_of_the_text_itself_is a little mysterious.", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isalittle </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

}
