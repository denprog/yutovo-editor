#include <gtest/gtest.h>
#include "mock.h"
#include <QPainter>

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, strings1)
{
    Start(600);

    document.WaitTask(document.SetFontSize(22));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("T", true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("e", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Te</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.InsertString("x", true);
    document.InsertString("t", true);
    document.WaitTask(document.InsertString("Text", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true));
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
    Start(600);

    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
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

    document.WaitTask(document.InsertString("Str", true));
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();
    document.InsertString("i", true);
    document.InsertString("n", true);
    document.WaitTask(document.InsertString("g", true));
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

    document.WaitTask(document.InsertString("Str", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TextStriStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, strings3)
{
    Start(600);

    document.InsertString("Str", true);
    document.InsertString("i", true);
    document.InsertString("n", true);
    document.InsertString("g", true);
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitTask(document.DeleteElements(false, true));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Strin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections1)
{
    Start(600);

    document.WaitTask(document.InsertString("TestString", true));
    ASSERT_TRUE(document.ToText() == U"TestString") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"TestStrin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Test") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
    Start(600);

    document.SetFontSize(22);
    document.InsertString("Test", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bo</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2, 0, 2, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>B</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2, 0, 2, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections3)
{
    Start(600);

    document.InsertString("Normal", document.GetStringFormat("Arial", 16, false, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 9; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 2},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Norm</span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretHome(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    for (int i = 0; i < 20; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Selection of rows
TEST_F(DocumentTest, selections4)
{
    Start(600);

    document.Load("../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 11})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, selections5)
{
    Start(600);

    document.InsertString("1234", true);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.SelectAll());
    document.WaitTask(document.InsertString("55", true));
    ASSERT_TRUE(document.ToText() == U"55") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Select all with rows
TEST_F(DocumentTest, selections6)
{
    Start(370);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 41},
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Select all with rows
TEST_F(DocumentTest, selections7)
{
    Start(370);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 3, 39},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 3, 39},
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts1)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Test", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

TEST_F(DocumentTest, inserts2)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Test", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertString("X", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertString("h", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Teh</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>ld</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts3)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

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

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts4)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    for (int i = 0; i < 4; ++i)
        document.DeleteElements(true, true);
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
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts5)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    for (int i = 0; i < 2; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">xt</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">xt</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts6)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts7)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
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
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.InsertString("T", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertString("d", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">TText</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Boldd</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">TText</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Boldd</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();
}

//Insert a string wider than the screen and add chars
TEST_F(DocumentTest, inserts8)
{
    Start(300);

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
    Start(300);

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_isa", true));
    document.Undo();
    document.WaitUndo();
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
    Start(600);

    document.SetFontFamily("Courier New");
    document.SetFontSize(12);
    document.WaitTask(document.InsertString("C", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">C</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.InsertString("o", true);
    document.InsertString("u", true);
    document.InsertString("r", true);
    document.InsertString("i", true);
    document.InsertString("e", true);
    document.WaitTask(document.InsertString("r", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
    {
        document.Undo();
        document.WaitUndo();
    }
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Co</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
    {
        document.Undo();
        std::this_thread::sleep_for(10ms);
    }
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 15; ++i)
    {
        document.Redo();
        std::this_thread::sleep_for(10ms);
    }
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.SetFontFamily("Times New Roman");
    document.SetFontSize(22);
    document.SetBold(true);
    document.SetItalic(true);
    document.SetUnderline(true);
    document.WaitTask(document.InsertString(" New", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
        document.Redo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts2)
{
    Start(600);

    document.WaitTask(document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Tex</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;\">t</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;text-decoration: underline;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts3)
{
    Start(600);

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
    Start(600);

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
    Start(600);

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

//Check font
TEST_F(DocumentTest, fonts6)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 4}, format));
    ASSERT_TRUE(format.size == 22);
}

TEST_F(DocumentTest, fonts7)
{
    Start(600);

    document.InsertString("Italic", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Italic</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts8)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.InsertString("Italic", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Italic</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts9)
{
    Start(600);

    document.InsertString("TextItalic", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">TextIta</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Te</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">xtIta</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Check font of a selected string
TEST_F(DocumentTest, fonts10)
{
    Start(600);

    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);

    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
}

//Set font attributes
TEST_F(DocumentTest, fonts11)
{
    Start(600);

    document.InsertString("TestText", true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">TestTe</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>xt</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.bold);

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Test</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Te</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong><em>xt</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 2}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">Test</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><em>Te</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong><em>xt</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 2}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
}

//Set font attributes
TEST_F(DocumentTest, fonts12)
{
    Start(600);

    document.InsertString("123456789", true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetUnderline(true));

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetItalic(true));

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>1234</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong><em>567</em></strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong><em>89</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1234</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>567</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><em>89</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete1)
{
    Start(600);

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
    document.DeleteElements(true, true);
    document.WaitTask(document.DeleteElements(true, true));
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

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

    document.WaitTask(document.DeleteElements(false, true));
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
    document.WaitTask(document.DeleteElements(false, true));
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
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Normal", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.WaitMainLoop();
    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    document.DeleteElements(true, true);
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
    document.DeleteElements(true, true);
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

    document.WaitTask(document.DeleteElements(false, true));
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

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
        document.DeleteElements(true, true);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
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
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Normal", document.GetStringFormat("Arial", 22, false, false, false), true);
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(true, true));
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
    document.WaitTask(document.DeleteElements(false, true));
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

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertString("orm", true));
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
    Start(600);

    document.WaitTask(document.InsertString("La versión 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.", true));
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 8; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.DeleteElements(false, true));
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
    Start(390);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
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
    std::this_thread::sleep_for(600ms);
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
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Delete all
TEST_F(DocumentTest, delete6)
{
    Start(368);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(false, true));
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
    Start(330);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(400ms);
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
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
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
    document.WaitTask(document.DeleteElements(false, true));
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
    Start(227);

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
    document.WaitTask(document.DeleteElements(false, true));
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
    Start(300);

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_is a little mysterious.", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true));
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
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete at the end of a string
TEST_F(DocumentTest, delete10)
{
    Start(300);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">s a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 30})) << document.GetEditorState().ToString();
}

//Backspace at the beginning of a string
TEST_F(DocumentTest, delete11)
{
    Start(288);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">itselfis a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 30})) << document.GetEditorState().ToString();
}

//Backspace at the end of a string on the second row
TEST_F(DocumentTest, delete12)
{
    Start(400);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 10})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11})) << document.GetEditorState().ToString();
}

//Restrict Undo
TEST_F(DocumentTest, undo1)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.undo_size = 4;
    document.SetConfig(config);

    document.InsertString("T", true);
    document.InsertString("h", true);
    document.InsertString("e", true);
    document.InsertString(" ", true);
    document.InsertString("s", true);
    document.InsertString("o", true);
    document.WaitTask(document.InsertString("u", true));

    for (int i = 0; i < 4; ++i)
    {
        document.Undo();
        document.WaitUndo();
    }
    document.Undo();
    std::this_thread::sleep_for(200ms);
    document.Undo();
    std::this_thread::sleep_for(200ms);
    document.Undo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

}
