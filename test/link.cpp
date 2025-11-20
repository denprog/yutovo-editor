/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

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

//Insert a string before a link
TEST_F(DocumentTest, link5)
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

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertString("3", true));
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1234</span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert a link after a code block
TEST_F(DocumentTest, link6)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert a string after a link
TEST_F(DocumentTest, link7)
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

    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

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
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Insert a paragraph after a link
TEST_F(DocumentTest, link8)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("1", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

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
}

//Insert a link monospace
TEST_F(DocumentTest, link9)
{
    Start(600);

    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">123</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
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
                "<span style=\"font-family:'Courier New';font-size:12px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
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
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Courier New';font-size:12px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">123</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3})) << document.GetEditorState().ToString();
}

//Insert a string inside a link
TEST_F(DocumentTest, link10)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("n", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">linnk</a>"\
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
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, link11)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    document.SwitchInsertMode();
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("m", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">limk</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy link from another document
TEST_F(TwoDocumentsTest, link12)
{
    Start(600);

    document.WaitTask(document.InsertLink("link", "www.link.ru", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));

    document2.WaitTask(document2.Paste(clipboard_json));
    ASSERT_TRUE(document2.ToHtml() == 
        "<body>"\
            "<p>"\
                "<a url=\"www.link.ru\" style=\"font-family:'Arial';font-size:14px;text-decoration: underline;color:rgba(0,0,255,255);\">link</a>"\
            "</p>"\
        "</body>") << 
        document2.ToHtml();
    ASSERT_TRUE(clipboard_text == U"link") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document2.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document2.GetEditorState().ToString();
}

}
