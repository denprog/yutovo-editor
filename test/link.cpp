#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Insert link
TEST_F(DocumentTest, link1)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLinkClicked).WillOnce([&](ElementId id, const std::u32string& url)
        {
            ASSERT_TRUE(url == U"www.link.ru");
        });

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 1, rect.top + 1, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(255,105,180,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Change a link
TEST_F(DocumentTest, link2)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertLink(U"new link", U"www.link1.ru", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link1.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">new link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 8})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link1.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">new link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 8})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link1.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">new lin</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 7})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link1.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">new link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 8})) << document.GetEditorState().ToString();
}

//Insert a string after a code block and a link
TEST_F(DocumentTest, link3)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertCode(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertString("Text", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 4})) << document.GetEditorState().ToString();
}

//Insert a space after a link
TEST_F(DocumentTest, link4)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString(" ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

}
