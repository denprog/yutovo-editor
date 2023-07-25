#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, caret1)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 1, 3)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertString(" Word2", true);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.InsertString(" Word3", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text Word2 Word3</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 5},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 5, 11},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 16}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 16, 10, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 6},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret2)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 42)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 41)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 11)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret3)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a little </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 42)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret4)
{
    Start(600);

    document.InsertString("Text Word2 Word3 ", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false), true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 7},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret5)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false), true);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.MoveCaretRight(false);
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 7}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 7, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 6}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 6, 2}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 6)) << document.GetEditorState().ToString();
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 6, 2}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret6)
{
    Start(600);
    
    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String", true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 3})) << document.GetEditorState().ToString();
}

//PageUp/PageDown
TEST_F(DocumentTest, caret7)
{
    Start(670);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.Load("../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 9, 0, 12})) << document.GetEditorState().ToString();

    document.MoveCaretPageUp(false);
    document.WaitTask(document.MoveCaretPageUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 11, 0, 8})) << document.GetEditorState().ToString();

    for (int i = 0; i < 6; ++i)
        document.MoveCaretPageDown(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 5, 1, 0, 42})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretPageUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 10, 0, 38})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret8)
{
    Start(368);
    
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 4})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret9)
{
    Start(600);

    document.InsertString("1234", true);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret10)
{
    Start(600);

    document.InsertString("1234", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret11)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret12)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 4})) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 1, 3})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret13)
{
    Start(368);
    
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 4, 7},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 38},
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 41},
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret14)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 24, false, true, false), true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 5})) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret15)
{
    Start(600);

    document.InsertString("12345", true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret16)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret17)
{
    Start(380);

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

}
