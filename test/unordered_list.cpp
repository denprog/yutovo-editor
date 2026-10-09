/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "paragraph.h"
#include "style.h"
#include <fstream>

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Apply every list marker to a paragraph
TEST_F(UnorderedListTest, unordered_list1)
{
    Start(600);

    document.InsertString("Item", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(paragraph->marker_format->text_color == Color::Black()) << paragraph->marker_format->text_color.ToHex();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Item</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    //switching the marker replaces it
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::large_circle_marker, true));
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::large_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">●&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Item</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::diamond_marker, true));
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::diamond_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml().find("♦&nbsp;") != std::string::npos) << document.ToHtml();

    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::square_marker, true));
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::square_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml().find("■&nbsp;") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

//Applying the same marker again toggles it off
TEST_F(UnorderedListTest, unordered_list2)
{
    Start(600);

    document.InsertString("Item", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true)); //toggle off
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker.empty()) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(!paragraph->marker_format) << "the marker format must be cleared together with the marker";
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Item</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(paragraph->marker_format->text_color == Color::Black()) << "undo must restore the marker format";
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker.empty()) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") == std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

//Enter in the middle of a list paragraph splits it - both parts keep the marker
TEST_F(UnorderedListTest, unordered_list3)
{
    Start(600);

    document.InsertString("First", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Fir</span>"
            "</p>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">st</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">First</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Fir</span>"
            "</p>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">st</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Enter at the end of a list paragraph continues the list
TEST_F(UnorderedListTest, unordered_list4)
{
    Start(600);

    document.InsertString("First", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.WaitTask(document.InsertParagraph(true));
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(paragraph->marker_format->text_color == Color::Black()) << "the new paragraph inherits the marker format";
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">First</span>"
            "</p>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("Second", true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">First</span>"
            "</p>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Second</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 6)) << document.GetEditorState().ToString();
}

//Enter on an empty list item exits the list instead of adding a new paragraph
TEST_F(UnorderedListTest, unordered_list5)
{
    Start(600);

    document.InsertString("One", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") != std::string::npos) << document.ToHtml();

    document.WaitTask(document.InsertParagraph(true)); //exits the list
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker.empty()) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">One</span>"
            "</p>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">One</span>"
            "</p>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker.empty()) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") == std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Applying a marker to a selection marks all the selected paragraphs, applying it again clears them
TEST_F(UnorderedListTest, unordered_list6)
{
    Start(600);

    document.InsertString("One text", true);
    document.InsertParagraph(true);
    document.InsertString("Two", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Three", true));
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretUp(true)); //select from the middle of the first paragraph through the whole second one

    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::diamond_marker, true));
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 0}).get())->marker == ParagraphFormat::diamond_marker) << "the first paragraph must be marked";
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 1}).get())->marker == ParagraphFormat::diamond_marker) << "the second paragraph must be marked";
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 2}).get())->marker.empty()) << "the third paragraph must stay plain";
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">♦&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">One text</span>"
            "</p>"
            "<p>"
                "<span style=\"color: #000000;\">♦&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Two</span>"
            "</p>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Three</span>"
            "</p>"
        "</body>") << document.ToHtml();

    //all the selected paragraphs carry the marker - applying it again clears them
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::diamond_marker, true));
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 0}).get())->marker.empty());
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 1}).get())->marker.empty());
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 2}).get())->marker.empty());
    ASSERT_TRUE(document.ToHtml().find("♦&nbsp;") == std::string::npos) << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 0}).get())->marker == ParagraphFormat::diamond_marker);
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 1}).get())->marker == ParagraphFormat::diamond_marker);
    ASSERT_TRUE(document.ToHtml().find("♦&nbsp;") != std::string::npos) << document.ToHtml();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 0}).get())->marker.empty());
    ASSERT_TRUE(((Paragraph*)document.GetElement(ElementId{0, 1}).get())->marker.empty());
    ASSERT_TRUE(document.ToHtml().find("♦&nbsp;") == std::string::npos) << document.ToHtml();
}

//Backspace at the beginning of a list paragraph removes the marker first, the second backspace joins the paragraphs
TEST_F(UnorderedListTest, unordered_list7)
{
    Start(600);

    document.InsertString("One", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Two", true));
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.WaitTask(document.MoveCaretHome(false));

    document.WaitTask(document.DeleteElements(true, true)); //backspace removes the marker
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker.empty()) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">One</span>"
            "</p>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Two</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker.empty()) << ToBasicString(paragraph->marker);

    document.WaitTask(document.DeleteElements(true, true)); //the second backspace joins the paragraphs
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\">OneTwo</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

//The list marker survives save/load together with its format
TEST_F(UnorderedListTest, unordered_list8)
{
    Start(600);

    document.InsertString("First", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Second", true));
    std::string html = document.ToHtml();
    auto format_id = ((Paragraph*)document.GetElement(ElementId{0, 0}).get())->marker_format->id;
    ASSERT_TRUE(html.find("•&nbsp;") != std::string::npos) << html;

    document.WaitTask(document.Save("unordered_list8.yut"));
    std::this_thread::sleep_for(200ms);
    document.Load("unordered_list8.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.ToHtml() == html) << document.ToHtml();
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(paragraph->marker_format) << "the marker format must be restored by its saved id";
    ASSERT_TRUE(paragraph->marker_format->id == format_id) << "the marker format must keep its serialization id";
    ASSERT_TRUE(paragraph->marker_format->text_color == Color::Black());
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 1}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::small_circle_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(paragraph->marker_format) << "the marker format must be restored by its saved id";
}

//Pasting text into a list paragraph does not disturb the marker
TEST_F(UnorderedListTest, unordered_list9)
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

    document.InsertString("Item", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.MoveCaretHome(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">ItemItem</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToText() == U"ItemItem") << ToBasicString(document.ToText());
}

//ToText does not contain the list markers
TEST_F(UnorderedListTest, unordered_list10)
{
    Start(600);

    document.InsertString("One", true);
    document.SetCurrentParagraphMarker(ParagraphFormat::square_marker, true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Two", true));
    ASSERT_TRUE(document.ToText() == U"One\nTwo") << ToBasicString(document.ToText());
}

//The list marker sits at the text indent of a plain paragraph - the text follows the marker
TEST_F(UnorderedListTest, unordered_list11)
{
    Start(600);

    document.WaitTask(document.InsertString("Item", true));
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    int left = paragraph->elements->Get(0)->rect.left; //the text position of a plain paragraph
    Rect rect = paragraph->GetAbsoluteRect();

    //allow the plain text draws - with a specific expectation present every other DrawText call would fail as unexpected
    EXPECT_CALL(window_mock, DrawText).Times(testing::AnyNumber());
    //the marker must be drawn exactly at the plain paragraph text position
    EXPECT_CALL(window_mock, DrawText(ToBasicString(ParagraphFormat::small_circle_marker), testing::_,
        testing::Field(&Rect::left, rect.left + left), testing::_, testing::_, testing::_)).Times(testing::AtLeast(1));

    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    //the marker is drawn by a separate redraw task - give it time before the mock expectation is verified
    std::this_thread::sleep_for(200ms);
    paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->elements->Get(0)->rect.left > left) << "the text must follow the marker";
}

//The list marker format follows the paragraph format
TEST_F(UnorderedListTest, unordered_list12)
{
    Start(600);

    document.InsertString("Head", true);
    document.WaitTask(document.SetCurrentParagraphFormat("Header 1"));
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::diamond_marker, true));
    auto paragraph = (Paragraph*)document.GetElement(ElementId{0, 0}).get();
    ASSERT_TRUE(paragraph->marker == ParagraphFormat::diamond_marker) << ToBasicString(paragraph->marker);
    ASSERT_TRUE(paragraph->marker_format->size == 30) << "the marker must follow the paragraph font size";
    ASSERT_TRUE(paragraph->marker_format->bold) << "the marker must follow the paragraph font style";
    ASSERT_TRUE(paragraph->marker_format->text_color == Color::Black());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<span style=\"color: #000000;\">♦&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>Head</strong></span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

//Changing the alignment of a list paragraph keeps the marker - the marker is a part of the paragraph format
TEST_F(UnorderedListTest, unordered_list13)
{
    Start(600);

    document.InsertString("Item", true);
    document.WaitTask(document.SetCurrentParagraphMarker(ParagraphFormat::small_circle_marker, true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p align=\"center\">"
                "<span style=\"color: #000000;\">•&nbsp;</span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">Item</span>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml().find("align=\"center\"") == std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") != std::string::npos) << document.ToHtml();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml().find("align=\"center\"") != std::string::npos) << document.ToHtml();
    ASSERT_TRUE(document.ToHtml().find("•&nbsp;") != std::string::npos) << document.ToHtml();
}

}
