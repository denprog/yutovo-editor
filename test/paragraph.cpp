#include <gtest/gtest.h>
#include "mock.h"
#include <QPainter>
#include "util.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

struct ParagraphTest : DocumentTest
{
};

TEST_F(ParagraphTest, resizing1)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    width = 420;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 15)) << document.GetEditorState().ToString();

    width = 450;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 7)) << document.GetEditorState().ToString();

    width = 330;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 23)) << document.GetEditorState().ToString();

    width = 250;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 7)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(false));
    width = 240;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 8)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    width = 220;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 4, 0, 1)) << document.GetEditorState().ToString();

    width = 240;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 9)) << document.GetEditorState().ToString();

    width = 335;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 17)) << document.GetEditorState().ToString();

    width = 330;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 17)) << document.GetEditorState().ToString();

    width = 200;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 4, 0, 1)) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, resizing2)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 21}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 21, 2})) << document.GetEditorState().ToString();

    width = 440;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 5, 2})) << document.GetEditorState().ToString();

    width = 530;
    document.WaitTask(document.Resize(width, 400));
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 8, 8, 7)) << document.GetEditorState().ToString();

    width = 440;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 8, 8, 7)) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    width = 390;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 3, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    width = 420;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 3, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();
}

//Delete, resize and undo.
TEST_F(ParagraphTest, resizing3)
{
    Start(400);

    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.DeleteElements(true, true));
    width = 600;
    document.WaitTask(document.Resize(width, 400));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 53})) << document.GetEditorState().ToString();
}

//Resize with selection
TEST_F(ParagraphTest, resizing4)
{
    Start(305);

    int width = 305;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 23}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    width = 260;
    document.WaitTask(document.Resize(width, 400));

    width = 236;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 8}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 4})) << document.GetEditorState().ToString();
}

//Resize with selection
TEST_F(ParagraphTest, resizing5)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.InsertDivision(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 37}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 41, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 37})) << document.GetEditorState().ToString();

    width = 390;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 18}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 18})) << document.GetEditorState().ToString();
}

//Resize with selection
TEST_F(ParagraphTest, resizing6)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true));
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));

    width = 250;
    document.WaitTask(document.Resize(width, 400));

    width = 200;
    document.WaitTask(document.Resize(width, 400));

    width = 150;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 8}, 
        ElementSelectionState{ElementId{0, 0}, 4, 1})) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, paragraph1)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertParagraph(true));
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 3)) << document.GetEditorState().ToString();

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.InsertParagraph(true));
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(3, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(3, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertParagraph(true));
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
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(4, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, paragraph2)
{
    Start(494);

    int width = 494;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little mysterious.") << ToBasicString(document.ToText());

    document.WaitTask(document.SetCurrentParagraphFormat("Header 1"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>The source of the text </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>itself is a little </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>mysterious.</strong></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 11)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 53)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>The source of the text </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>itself is a little </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>mysterious.</strong></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 11)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 53)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBold(true));
    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">The </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"><strong>source</strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"> of the text itself is a little </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>source</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10, 4, 6)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>source</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">The </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"><strong>source</strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"> of the text itself is a little </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">The </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"><strong>source</strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"> of the text itself is a little </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>source</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Divide by rows with a code block
TEST_F(ParagraphTest, paragraph3)
{
    Start(100);

    int width = 100;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("45", document.GetStringFormat("Arial", 14, false, false, false), true);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">45</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a formula at the beginning of a row and insert a paragraph at the beginning
TEST_F(ParagraphTest, paragraph4)
{
    Start(380);

    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });
    
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Insert paragraphs in a loaded file
TEST_F(ParagraphTest, paragraph5)
{
    Start(600);

    document.Load("../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertParagraph(true));

    auto el = document.GetElement(ElementId{0, 0});
    ASSERT_TRUE(el->type == ElementType::PARAGRAPH && el->ToText() == U"Арифме́тика") << ToBasicString(el->ToText());
    ASSERT_TRUE(el->elements->Count() == 1) << el->elements->Count();

    el = document.GetElement(ElementId{0, 1});
    ASSERT_TRUE(el->type == ElementType::PARAGRAPH && el->ToText().rfind(U" (др.-греч.", 0) == 0) << ToBasicString(el->ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();
    ASSERT_TRUE(el->elements->Count() == 16) << el->elements->Count();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    el = document.GetElement(ElementId{0, 0});
    ASSERT_TRUE(el->type == ElementType::PARAGRAPH && el->ToText().rfind(U"Арифме́тика (др.-греч.", 0) == 0) << ToBasicString(el->ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

//Insert paragraphs in a multiline text
TEST_F(ParagraphTest, paragraph6)
{
    Start(390);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Backspace at the beginning of a paragraph
TEST_F(ParagraphTest, paragraph7)
{
    Start(400);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, wh</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ether this object is a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, whether this object is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, wh</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ether this object is a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Insert a paragraph when caret is on a code block
TEST_F(ParagraphTest, paragraph8)
{
    Start(400);

    document.WaitTask(document.InsertString("Text Code", document.GetStringFormat("Arial", 14, false, false, false), true));
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text </span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Code</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Code</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert paragraph when the caret is on a formula
TEST_F(ParagraphTest, paragraph9)
{
    Start(400);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                        "<mi>5</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d</mi>"\
                        "<mo>+</mo>"\
                        "<mi>5</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert paragraphs in a multiline text
TEST_F(ParagraphTest, paragraph10)
{
    Start(390);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 11})) << document.GetEditorState().ToString();
}

//Insert a paragraph and a text after a code block
TEST_F(ParagraphTest, paragraph11)
{
    Start(400);

    document.InsertCode(false, true);
    document.WaitTask(document.MoveCaretRight(false));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.name == "Text body");
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 1, 0, 11})) << document.GetEditorState().ToString();
}

//Insert a paragraph before a code block
TEST_F(ParagraphTest, paragraph12)
{
    Start(400);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">xt</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Check format
TEST_F(ParagraphTest, format1)
{
    Start(600);

    document.InsertString("In literary theory", true);
    document.SetCurrentParagraphFormat("Header 1");
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("a text is any object", true));
    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 4}, format));
    ASSERT_TRUE(format.name == "Header 1");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 1, 0, 0, 4}, format));
    ASSERT_TRUE(format.name == "Monospace");
}

//Check format
TEST_F(ParagraphTest, format2)
{
    Start(600);

    document.InsertString("theory", true);
    document.SetCurrentParagraphFormat("Monospace");
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true);
    document.WaitTask(document.DeleteElements(true, true));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Monospace");
    StringFormat str_format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, str_format));
    ASSERT_TRUE(str_format.family == "Courier New");
}

//Change style of a paragraph with a code block
TEST_F(ParagraphTest, format3)
{
    Start(400);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.SetCurrentParagraphFormat("Header 1"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>Text</strong></span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Header 1");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 1, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Code") << format.name;

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Text body");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 1, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Code") << format.name;
}

//Delete a paragraph
TEST_F(ParagraphTest, delete1)
{
    Start(600);

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 49},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

//Delete a paragraph
TEST_F(ParagraphTest, delete2)
{
    Start(600);

    document.InsertString("Text.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String.", true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretHome(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 7}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

//Delete a selection between paragraphs
TEST_F(ParagraphTest, delete3)
{
    Start(600);

    document.InsertString("ParagraphText.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String.", true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Parag.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ParagraphText.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 10}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, delete4)
{
    Start(600);

    document.InsertString("Paragraph1.", true);
    document.InsertParagraph(true);
    document.InsertString("Paragraph2", true);
    document.InsertParagraph(true);
    document.InsertString("ParagraphText.", true);
    document.InsertParagraph(true);
    document.InsertString("String.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Paragraph5.", true));
    document.MoveCaretUp(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Parag.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph5.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ParagraphText.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph5.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 2, 0, 0}, 4, 10}, 
        ElementSelectionState{ElementId{0, 3, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete5)
{
    Start(390);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete6)
{
    Start(390);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.SetBold(true);
    document.MoveCaretLeft(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>that</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>that</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete7)
{
    Start(390);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.SetBold(true);
    document.MoveCaretHome(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete8)
{
    Start(390);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.SetBold(true);
    document.MoveCaretHome(false);
    document.MoveCaretWordRight(false);
    document.InsertCode(true, true);
    document.MoveCaretHome(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> can be read, whether this </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> can be read, whether this </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows between paragraphs
TEST_F(ParagraphTest, delete9)
{
    Start(390);

    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Ttring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a selection between paragraphs
TEST_F(ParagraphTest, delete10)
{
    Start(600);

    document.InsertString("ParagraphText.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String.", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Paragraph3.", true));
    document.MoveCaretUp(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Parag.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ParagraphText.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 10}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Delete rows with a formula
TEST_F(ParagraphTest, delete11)
{
    Start(390);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> object is a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether re</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 26})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> object is a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 29}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 26, 4}, 
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 29})) << document.GetEditorState().ToString();
}

//Delete rows with a formula
TEST_F(ParagraphTest, delete12)
{
    Start(310);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether iterature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 8})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this()/() object is a work of literature") << 
        ToBasicString(document.ToText());;
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 8, 4}, 
        ElementSelectionState{ElementId{0, 0, 2}, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 3, 0}, 0, 9})) << document.GetEditorState().ToString();
}

//Delete rows in two paragraphs
TEST_F(ParagraphTest, delete13)
{
    Start(610);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 15}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 15})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Арифме́тикаa work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.\n"\
        "In literary theory, a text is any object that can be read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 15}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 50}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 15})) << document.GetEditorState().ToString();
}

}
