/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Substring search in the plain document text: matches, highlight rectangles and the reported state
TEST_F(SearchTest, search1)
{
    Start(600);

    document.InsertString("cat", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("catalog", true));
    ASSERT_TRUE(document.ToText() == U"cat catalog") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();

    //allow the plain text draws - with a specific expectation present every other call would fail as unexpected
    EXPECT_CALL(window_mock, DrawText).Times(testing::AnyNumber());
    EXPECT_CALL(window_mock, DrawFillRect).Times(testing::AnyNumber());
    //both matches must be highlighted with the search color
    EXPECT_CALL(window_mock, DrawFillRect(testing::_, testing::_, testing::_, testing::_,
        testing::Eq(document.config.search_highlight_color))).Times(testing::AtLeast(2));
    //the matched glyphs get the search color as their background, otherwise the glyph background would erase the rect
    EXPECT_CALL(window_mock, DrawText(testing::_, testing::_, testing::_, testing::_,
        testing::Eq(document.config.search_highlight_color), testing::_)).Times(testing::AtLeast(2));

    document.WaitTask(document.Search(U"cat", false));
    //the highlight is drawn by the redraw task queued after the search task
    std::this_thread::sleep_for(200ms);

    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;
    ASSERT_TRUE(current == 0) << current; //the caret is after both matches - the first one is current

    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\">cat catalog</span></p></body>") << document.ToHtml();
}

//The search covers the plain text, code blocks and text blocks
TEST_F(SearchTest, search2)
{
    Start(600);

    document.WaitTask(document.InsertString("abc", true));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("abc", true)); //inside the code block
    document.WaitTask(document.InsertTextBlock(true)); //a nested text block inside the code block
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("abc", true)); //inside the text block

    //the code block matches are highlighted with the search color too (the CodeString drawing path)
    EXPECT_CALL(window_mock, DrawText).Times(testing::AnyNumber());
    EXPECT_CALL(window_mock, DrawFillRect).Times(testing::AnyNumber());
    EXPECT_CALL(window_mock, DrawFillRect(testing::_, testing::_, testing::_, testing::_,
        testing::Eq(document.config.search_highlight_color))).Times(testing::AtLeast(3));

    document.WaitTask(document.Search(U"abc", false));
    std::this_thread::sleep_for(100ms);

    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 3) << count;
}

//Formula signs are not strings and are not searched; no matches leaves the caret untouched
TEST_F(SearchTest, search3)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    document.InsertString("x", true);
    document.WaitTask(document.InsertAssignment(true));
    document.InsertString("1", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToText() == U"x=1+2") << ToBasicString(document.ToText());

    document.WaitTask(document.Search(U":", false)); //the assignment sign is a shape, not a string
    document.WaitTask(document.Search(U"+", false)); //the plus sign is a shape too
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    document.WaitTask(document.Search(U"1", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //no matches - FindNext does not move the caret
    auto state = document.GetEditorState();
    document.WaitTask(document.Search(U"zzz", false));
    document.WaitTask(document.FindNext(false));
    document.WaitTask(document.FindNext(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == state) << document.GetEditorState().ToString();
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0 && current == 0) << count << " " << current;
}

//Forward and backward navigation cycles through the matches with wrap-around and selects each match
TEST_F(SearchTest, search4)
{
    Start(600);

    document.InsertString("ab", true);
    document.InsertString(" ", true);
    document.InsertString("ab", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("ab", true));
    ASSERT_TRUE(document.ToText() == U"ab ab ab") << ToBasicString(document.ToText());

    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 3 && current == 0) << count << " " << current;

    //the search itself selects the first (wrapped) match
    document.WaitTask(document.FindNext(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5, 3, 2)) << document.GetEditorState().ToString();
    document.GetSearchState(count, current);
    ASSERT_TRUE(current == 1) << current;

    document.WaitTask(document.FindNext(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8, 6, 2)) << document.GetEditorState().ToString();
    document.GetSearchState(count, current);
    ASSERT_TRUE(current == 2) << current;

    //forward again wraps around to the first match
    document.WaitTask(document.FindNext(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 0, 2)) << document.GetEditorState().ToString();

    //backward from the first match wraps around to the last one
    document.WaitTask(document.FindNext(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8, 6, 2)) << document.GetEditorState().ToString();
    document.GetSearchState(count, current);
    ASSERT_TRUE(current == 2) << current;

    //backward again goes to the previous match without reselecting the current one
    document.WaitTask(document.FindNext(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5, 3, 2)) << document.GetEditorState().ToString();
    document.GetSearchState(count, current);
    ASSERT_TRUE(current == 1) << current;
}

//The search does not modify the document: no undo entries, the change flag stays clear
TEST_F(SearchTest, search5)
{
    Start(600);

    document.InsertString("one", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("two", true));

    size_t undo_size = document.GetUndoSize();

    document.WaitTask(document.Search(U"two", false));
    document.WaitTask(document.FindNext(false));
    std::this_thread::sleep_for(100ms);

    //the caret is at the end of the match, the match itself is selected
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 4, 3)) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToText() == U"one two") << ToBasicString(document.ToText());
    //the search itself creates no undo entries
    ASSERT_TRUE(document.GetUndoSize() == undo_size);
}

//The reported match count follows document edits while the search is active
TEST_F(SearchTest, search6)
{
    Start(600);

    document.WaitTask(document.InsertString("ab", true));
    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //leave the selected match before editing - typing over the selection would replace it
    document.MoveCaretEnd(false);

    //type another match in a new paragraph - the redraw updates the reported count
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("ab cd", true));
    std::this_thread::sleep_for(300ms);
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    //navigation sees the new match too - the caret is after it, so the search wraps to the first one;
    //the match covers the whole first paragraph, so the selection is stored in the optimized parent form
    document.WaitTask(document.FindNext(false));
    ElementSelectionState selection;
    selection.id = ElementId{0};
    selection.start = 0;
    selection.size = 1;
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, selection)) << document.GetEditorState().ToString();
}

//An empty search text deactivates the search and clears the highlights
TEST_F(SearchTest, search7)
{
    Start(600);

    document.InsertString("ab", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("ab", true));

    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(200ms);

    //after the search is cleared no search highlight may be drawn anymore
    EXPECT_CALL(window_mock, DrawText).Times(testing::AnyNumber());
    EXPECT_CALL(window_mock, DrawFillRect).Times(testing::AnyNumber());
    EXPECT_CALL(window_mock, DrawFillRect(testing::_, testing::_, testing::_, testing::_,
        testing::Eq(document.config.search_highlight_color))).Times(0);

    document.WaitTask(document.Search(U"", false));
    std::this_thread::sleep_for(300ms);

    uint count = 1, current = 1;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0 && current == 0) << count << " " << current;
}

//Case-insensitive by default (Latin and Cyrillic), case-sensitive with the flag
TEST_F(SearchTest, search8)
{
    Start(600);

    document.InsertString("Cat", true);
    document.InsertString(" ", true);
    document.InsertString("Ставки", true);
    document.WaitTask(document.InsertString(" i", true));
    ASSERT_TRUE(document.ToText() == U"Cat Ставки i") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"cat", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"CAT", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"ставки", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"cat", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    document.WaitTask(document.Search(U"CAT", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    document.WaitTask(document.Search(U"ставки", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //Yo folds its case too
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Ёлки", true));
    document.WaitTask(document.Search(U"ёлки", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//The whole word mode skips occurrences surrounded by word characters (letters, digits, underscore)
TEST_F(SearchTest, search14)
{
    Start(600);

    document.InsertString("cat", true);
    document.InsertString(" ", true);
    document.InsertString("catalog", true);
    document.InsertString(" ", true);
    document.InsertString("cat,", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("cat_1", true));
    ASSERT_TRUE(document.ToText() == U"cat catalog cat, cat_1") << ToBasicString(document.ToText());
    //a standalone digit at the string end - the string end is a word boundary too
    document.MoveCaretEnd(false);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToText() == U"cat catalog cat, cat_1 2") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"cat", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 4) << count;

    //only the standalone occurrences count as whole words
    document.WaitTask(document.Search(U"cat", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    //whole word + case-insensitive still folds the case
    document.WaitTask(document.Search(U"CAT", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    //digits and underscore are word characters too - the 1 in cat_1 is not a whole word
    document.WaitTask(document.Search(U"1", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //the trailing 2 is a whole word
    document.WaitTask(document.Search(U"2", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //a rejected whole-word candidate does not consume the range - a later match inside it is still found
    document.WaitTask(document.Search(U"tat", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;
}

//Deactivating the search clears the selection of the last match
TEST_F(SearchTest, search15)
{
    Start(600);

    document.WaitTask(document.InsertString("ab", true));

    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(100ms);
    ASSERT_FALSE(document.GetEditorState().selection_state.IsEmpty()) << document.GetEditorState().ToString();

    document.WaitTask(document.Search(U"", false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState().selection_state.IsEmpty()) << document.GetEditorState().ToString();

    //FindNext with the deactivated search does not move the caret either
    auto state = document.GetEditorState();
    document.WaitTask(document.FindNext(false));
    document.WaitTask(document.FindNext(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == state) << document.GetEditorState().ToString();

    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0 && current == 0) << count << " " << current;
}

//Link text is searched as well
TEST_F(SearchTest, search9)
{
    Start(600);

    document.WaitTask(document.InsertLink("example", "https://example.com", true));

    document.WaitTask(document.Search(U"example", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//The plain text of the selection prefills the find dialogs: a partial selection of a string
//leaf is char-based, an optimized whole-string selection sits at the parent and descends
TEST_F(SearchTest, search23)
{
    Start(600);

    document.InsertString("hello", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("world", true));
    ASSERT_TRUE(document.ToText() == U"hello world") << ToBasicString(document.ToText());

    std::u32string text;

    //a partial selection of the string
    EditorState state = MakeEditorState(0, 0, 0, 11, 6, 5);
    document.SetEditorState(state);
    ASSERT_TRUE(document.GetSelectedText(text)) << ToBasicString(text);
    ASSERT_TRUE(text == U"world") << ToBasicString(text);

    //the whole string raised to its parent (the row covering the single string)
    EditorState row_state = MakeEditorState(0, 0, 0, 11);
    row_state.selection_state.Add(ElementId{0, 0, 0}, 0, 1);
    document.SetEditorState(row_state);
    ASSERT_TRUE(document.GetSelectedText(text)) << ToBasicString(text);
    ASSERT_TRUE(text == U"hello world") << ToBasicString(text);

    EditorState caret_state = MakeEditorState(0, 0, 0, 11);

    //a link makes a second leaf in the row - the selection does not resolve to a single string
    //(clear the selection first - typing over a selection would replace it)
    document.SetEditorState(caret_state);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertLink("example", "https://example.com", true));
    ASSERT_TRUE(document.ToText() == U"hello world example") << ToBasicString(document.ToText());
    EditorState wide_state = MakeEditorState(0, 0, 0, 11);
    wide_state.selection_state.Add(ElementId{0, 0}, 0, 1);
    document.SetEditorState(wide_state);
    ASSERT_FALSE(document.GetSelectedText(text)) << ToBasicString(text);

    //no selection - nothing to prefill
    document.SetEditorState(caret_state);
    ASSERT_FALSE(document.GetSelectedText(text));
}

//Replace works on the current match, then moves to the next one; undo/redo restores the text
TEST_F(SearchTest, replace10)
{
    Start(600);

    document.InsertString("ab", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("ab cd", true));
    ASSERT_TRUE(document.ToText() == U"ab ab cd") << ToBasicString(document.ToText());

    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2 && current == 0) << count << " " << current;

    document.WaitTask(document.Replace(U"xy", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"xy ab cd") << ToBasicString(document.ToText());
    //the next match is selected
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5, 3, 2)) << document.GetEditorState().ToString();
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1 && current == 0) << count << " " << current;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"ab ab cd") << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"xy ab cd") << ToBasicString(document.ToText());
}

//Replace all covers the plain text and code blocks and is a single undo step
TEST_F(SearchTest, replace11)
{
    Start(600);

    document.WaitTask(document.InsertString("ab", true));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("ab", true));

    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.ReplaceAll(U"xy", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"xyxy") << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"abab") << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"xyxy") << ToBasicString(document.ToText());
}

//An empty replacement deletes the match
TEST_F(SearchTest, replace12)
{
    Start(600);

    document.InsertString("ab", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("ab", true));

    document.WaitTask(document.Search(U"ab", false));
    std::this_thread::sleep_for(100ms);

    document.WaitTask(document.Replace(U"", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U" ab") << ToBasicString(document.ToText());
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"ab ab") << ToBasicString(document.ToText());
}

//Replacing in a code block re-solves the equation
TEST_F(SearchTest, replace13)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    document.InsertString("1", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"1+2=3.") << ToBasicString(document.ToText());

    document.WaitTask(document.Search(U"2", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Replace(U"5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"1+5=6.") << ToBasicString(document.ToText());
}

//Replacing inside a solved result is rejected - the result rows are not editable; the document stays unchanged and no undo entry appears
TEST_F(SearchTest, replace14)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    document.InsertString("1", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString("2", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"1+2=3.") << ToBasicString(document.ToText());

    //the match sits inside the result row
    document.WaitTask(document.Search(U"3.", false));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    size_t undo_size = document.GetUndoSize();
    document.WaitTask(document.ReplaceAll(U"9.", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"1+2=3.") << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
    ASSERT_TRUE(document.GetUndoSize() == undo_size) << document.GetUndoSize() << " " << undo_size;
}

//Replace-all honors the whole word mode - only the standalone occurrences are replaced
TEST_F(SearchTest, replace15)
{
    Start(600);

    document.InsertString("cat", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString("catalog", true));
    ASSERT_TRUE(document.ToText() == U"cat catalog") << ToBasicString(document.ToText());

    document.WaitTask(document.Search(U"cat", false, true));
    std::this_thread::sleep_for(100ms);
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.ReplaceAll(U"dog", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"dog catalog") << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"cat catalog") << ToBasicString(document.ToText());
}

//German: umlauts fold their case; the sharp s has no uppercase letter and does not fold with ss
TEST_F(SearchTest, search16)
{
    Start(600);

    document.InsertString(U"Der große Bauernhof liegt über dem Hügel.", true);
    document.InsertParagraph(true);
    document.InsertString(U"Große Bäume wachsen südlich der Küste.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Die Mäuse mögen Käse und Käsesuppe über alles.", true));
    ASSERT_TRUE(document.ToText() == U"Der große Bauernhof liegt über dem Hügel.\nGroße Bäume wachsen südlich der Küste.\n"
        U"Die Mäuse mögen Käse und Käsesuppe über alles.") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    //case-insensitive: both the lowercase and the capitalized occurrence match
    document.WaitTask(document.Search(U"große", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"GROßE", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    //case-sensitive: each pattern finds only its own case
    document.WaitTask(document.Search(U"große", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"Große", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //umlauts fold in both directions
    document.WaitTask(document.Search(U"MÄUSE", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"MÄUSE", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //not a whole word: Käse inside Käsesuppe counts too; as a whole word only the standalone one
    document.WaitTask(document.Search(U"Käse", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"Käse", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"KÄSE", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //ss does not fold with the sharp s - äußerst has no uppercase form with SS
    document.WaitTask(document.Search(U"ÄUSSERST", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;
}

//German replace: a single replacement, then case-insensitive, case-sensitive and whole word replace-all
TEST_F(SearchTest, replace16)
{
    Start(600);

    document.InsertString(U"Der große Garten liegt hinter dem großen Haus.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Die Große Terrasse gehört zum Garten.", true));
    const std::u32string original = U"Der große Garten liegt hinter dem großen Haus.\nDie Große Terrasse gehört zum Garten.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    uint count = 0, current = 0;

    //a single Replace works on the current match - the first one in the document
    document.WaitTask(document.Search(U"große", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"kleine", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Der kleine Garten liegt hinter dem großen Haus.\nDie Große Terrasse gehört zum Garten.")
        << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: both cases are replaced
    document.WaitTask(document.Search(U"große", false));
    document.WaitTask(document.ReplaceAll(U"kleine", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Der kleine Garten liegt hinter dem kleinen Haus.\nDie kleine Terrasse gehört zum Garten.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"Der kleine Garten liegt hinter dem kleinen Haus.\nDie kleine Terrasse gehört zum Garten.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-sensitive: only the lowercase occurrence is replaced
    document.WaitTask(document.Search(U"große", true));
    document.WaitTask(document.ReplaceAll(U"kleine", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Der kleine Garten liegt hinter dem kleinen Haus.\nDie Große Terrasse gehört zum Garten.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //whole word: the adjective ending großen keeps its match
    document.WaitTask(document.Search(U"große", false, true));
    std::this_thread::sleep_for(100ms);
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.ReplaceAll(U"kleine", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Der kleine Garten liegt hinter dem großen Haus.\nDie kleine Terrasse gehört zum Garten.")
        << ToBasicString(document.ToText());
}

//English search: case and whole word modes on plain Latin text
TEST_F(SearchTest, search17)
{
    Start(600);

    document.InsertString(U"The quick brown fox jumps over the lazy dog.", true);
    document.InsertParagraph(true);
    document.InsertString(U"London Bridge is falling down, my fair lady.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"She sells seashells by the seashore.", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"The shepherds sell sheep.", true));
    ASSERT_TRUE(document.ToText() == U"The quick brown fox jumps over the lazy dog.\nLondon Bridge is falling down, my fair lady.\n"
        U"She sells seashells by the seashore.\nThe shepherds sell sheep.") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"bridge", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"BRIDGE", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"bridge", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    document.WaitTask(document.Search(U"Bridge", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //she sits inside seashells, shepherds and sheep - as a whole word only the standalone one
    document.WaitTask(document.Search(U"she", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 4) << count;

    document.WaitTask(document.Search(U"she", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"SHE", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//English replace: a single replacement, then case-insensitive, case-sensitive and whole word replace-all
TEST_F(SearchTest, replace17)
{
    Start(600);

    document.InsertString(U"She sells Seashells by the seashore.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"They see the sea and the seabirds.", true));
    const std::u32string original = U"She sells Seashells by the seashore.\nThey see the sea and the seabirds.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    uint count = 0, current = 0;

    //a single Replace works on the current match - the Sea of Seashells
    document.WaitTask(document.Search(U"sea", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"ocean", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"She sells oceanshells by the seashore.\nThey see the sea and the seabirds.")
        << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 3) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: every occurrence including the embedded ones is replaced
    document.WaitTask(document.Search(U"sea", false));
    document.WaitTask(document.ReplaceAll(U"ocean", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"She sells oceanshells by the oceanshore.\nThey see the ocean and the oceanbirds.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"She sells oceanshells by the oceanshore.\nThey see the ocean and the oceanbirds.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-sensitive: only the capitalized occurrence is replaced
    document.WaitTask(document.Search(U"Sea", true));
    document.WaitTask(document.ReplaceAll(U"Lake", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"She sells Lakeshells by the seashore.\nThey see the sea and the seabirds.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //whole word: only the standalone sea is replaced, the compounds keep theirs
    document.WaitTask(document.Search(U"sea", false, true));
    std::this_thread::sleep_for(100ms);
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.ReplaceAll(U"ocean", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"She sells Seashells by the seashore.\nThey see the ocean and the seabirds.")
        << ToBasicString(document.ToText());
}

//Spanish: ñ and the accented vowels fold their case; ¿ and ¡ are not word characters
TEST_F(SearchTest, search18)
{
    Start(600);

    document.InsertString(U"El niño pequeño soñó con el año nuevo.", true);
    document.InsertParagraph(true);
    document.InsertString(U"Los años pasan y el mañana trae otra música.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"¿Dónde está el pingüino? ¡Qué vergüenza!", true));
    ASSERT_TRUE(document.ToText() == U"El niño pequeño soñó con el año nuevo.\nLos años pasan y el mañana trae otra música.\n"
        U"¿Dónde está el pingüino? ¡Qué vergüenza!") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"niño", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //Ñ folds like the other uppercase letters
    document.WaitTask(document.Search(U"NIÑO", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"Niño", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //año sits inside años; as a whole word only the singular counts
    document.WaitTask(document.Search(U"año", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"año", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"MÚSICA", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"MÚSICA", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //the inverted punctuation does not break the word boundary
    document.WaitTask(document.Search(U"Qué", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//Spanish replace: a single replacement, then case-insensitive, case-sensitive and whole word replace-all
TEST_F(SearchTest, replace18)
{
    Start(600);

    document.InsertString(U"El niño pequeño juega con el niño grande.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"La niña lee un libro sobre los niños.", true));
    const std::u32string original = U"El niño pequeño juega con el niño grande.\nLa niña lee un libro sobre los niños.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //a single Replace works on the current match - the first niño
    document.WaitTask(document.Search(U"niño", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"gato", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"El gato pequeño juega con el niño grande.\nLa niña lee un libro sobre los niños.")
        << ToBasicString(document.ToText());
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: the plural niños is replaced too
    document.WaitTask(document.Search(U"niño", false));
    document.WaitTask(document.ReplaceAll(U"gato", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"El gato pequeño juega con el gato grande.\nLa niña lee un libro sobre los gatos.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"El gato pequeño juega con el gato grande.\nLa niña lee un libro sobre los gatos.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-sensitive: the capitalized Niño does not exist - nothing is replaced, no undo entry
    size_t undo_size = document.GetUndoSize();
    document.WaitTask(document.Search(U"Niño", true));
    document.WaitTask(document.ReplaceAll(U"gato", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetUndoSize() == undo_size) << document.GetUndoSize() << " " << undo_size;

    //whole word: the plural niños keeps its match
    document.WaitTask(document.Search(U"niño", false, true));
    document.WaitTask(document.ReplaceAll(U"gato", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"El gato pequeño juega con el gato grande.\nLa niña lee un libro sobre los niños.")
        << ToBasicString(document.ToText());
}

//French: the accented letters and the oe ligature fold their case; the apostrophe is a word boundary
TEST_F(SearchTest, search19)
{
    Start(600);

    document.InsertString(U"Le garçon est allé à l'école en été.", true);
    document.InsertParagraph(true);
    document.InsertString(U"Il était une fois un cœur français à Paris.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"La cuisine française est célèbre dans le cœur de la ville.", true));
    ASSERT_TRUE(document.ToText() == U"Le garçon est allé à l'école en été.\nIl était une fois un cœur français à Paris.\n"
        U"La cuisine française est célèbre dans le cœur de la ville.") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"cœur", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    //the oe ligature folds too
    document.WaitTask(document.Search(U"CŒUR", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"Cœur", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //français sits inside française; as a whole word only the noun counts
    document.WaitTask(document.Search(U"français", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"français", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //Ç and É fold
    document.WaitTask(document.Search(U"FRANÇAIS", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"ÉTÉ", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"ÉTÉ", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //the apostrophe of l'école does not break the word boundary
    document.WaitTask(document.Search(U"école", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//French replace: a single replacement, then case-insensitive and whole word replace-all
TEST_F(SearchTest, replace19)
{
    Start(600);

    document.InsertString(U"Le cœur français bat dans la capitale.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Un cœur vaillant vaut deux cœurs.", true));
    const std::u32string original = U"Le cœur français bat dans la capitale.\nUn cœur vaillant vaut deux cœurs.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //a single Replace works on the current match - the first cœur
    document.WaitTask(document.Search(U"cœur", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"esprit", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Le esprit français bat dans la capitale.\nUn cœur vaillant vaut deux cœurs.")
        << ToBasicString(document.ToText());
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: the plural cœurs is replaced too
    document.WaitTask(document.Search(U"cœur", false));
    document.WaitTask(document.ReplaceAll(U"esprit", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Le esprit français bat dans la capitale.\nUn esprit vaillant vaut deux esprits.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"Le esprit français bat dans la capitale.\nUn esprit vaillant vaut deux esprits.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //whole word: the plural cœurs keeps its match
    document.WaitTask(document.Search(U"cœur", false, true));
    document.WaitTask(document.ReplaceAll(U"esprit", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Le esprit français bat dans la capitale.\nUn esprit vaillant vaut deux cœurs.")
        << ToBasicString(document.ToText());
}

//Italian: the grave and acute accented vowels fold their case; an accented letter is a word character
TEST_F(SearchTest, search20)
{
    Start(600);

    document.InsertString(U"Il bambino è andato in città per comprare il caffè.", true);
    document.InsertParagraph(true);
    document.InsertString(U"Però il cittadino ama la città e la virtù.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"L'università organizza attività per gli studenti.", true));
    ASSERT_TRUE(document.ToText() == U"Il bambino è andato in città per comprare il caffè.\nPerò il cittadino ama la città e la virtù.\n"
        U"L'università organizza attività per gli studenti.") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"città", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    //À folds
    document.WaitTask(document.Search(U"CITTÀ", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"CITTÀ", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //per sits inside Però and stands in both paragraphs; as a whole word the two standalone
    //prepositions count, because ò belongs to the word just like an unaccented letter
    document.WaitTask(document.Search(U"per", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 3) << count;

    document.WaitTask(document.Search(U"per", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"VIRTÙ", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//Italian replace: a single replacement, then case-insensitive, case-sensitive and whole word replace-all
TEST_F(SearchTest, replace20)
{
    Start(600);

    document.InsertString(U"Il caffè per la colazione è pronto.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Però il tè per il pomeriggio è freddo.", true));
    const std::u32string original = U"Il caffè per la colazione è pronto.\nPerò il tè per il pomeriggio è freddo.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    uint count = 0, current = 0;

    //a single Replace works on the current match - the per of the first paragraph
    document.WaitTask(document.Search(U"per", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"con", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Il caffè con la colazione è pronto.\nPerò il tè per il pomeriggio è freddo.")
        << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: the per inside Però is replaced too
    document.WaitTask(document.Search(U"per", false));
    document.WaitTask(document.ReplaceAll(U"con", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Il caffè con la colazione è pronto.\nconò il tè con il pomeriggio è freddo.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"Il caffè con la colazione è pronto.\nconò il tè con il pomeriggio è freddo.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-sensitive: the lowercase per only
    document.WaitTask(document.Search(U"Per", true));
    std::this_thread::sleep_for(100ms);
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.ReplaceAll(U"dopo", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Il caffè per la colazione è pronto.\ndopoò il tè per il pomeriggio è freddo.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //whole word: Però keeps its match
    document.WaitTask(document.Search(U"per", false, true));
    document.WaitTask(document.ReplaceAll(U"con", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Il caffè con la colazione è pronto.\nPerò il tè con il pomeriggio è freddo.")
        << ToBasicString(document.ToText());
}

//Portuguese: ã, õ and ç fold their case
TEST_F(SearchTest, search21)
{
    Start(600);

    document.InsertString(U"O menino pequeno foi à escola em São Paulo.", true);
    document.InsertParagraph(true);
    document.InsertString(U"A mãe não fez pão e guardou a lição do filho.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Não é possível esquecer as lições das escolas de matemática.", true));
    ASSERT_TRUE(document.ToText() == U"O menino pequeno foi à escola em São Paulo.\nA mãe não fez pão e guardou a lição do filho.\n"
        U"Não é possível esquecer as lições das escolas de matemática.") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    //the plural lições moves the tilde from a to o, so it does not contain lição
    document.WaitTask(document.Search(U"lição", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    //Ã and Ç fold
    document.WaitTask(document.Search(U"LIÇÃO", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"LIÇÃO", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //escola sits inside escolas; as a whole word only the singular counts
    document.WaitTask(document.Search(U"escola", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"escola", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"SÃO", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"MÃE", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //não in both paragraphs, case-insensitive
    document.WaitTask(document.Search(U"NÃO", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;
}

//Portuguese replace: a single replacement, then case-insensitive and whole word replace-all
TEST_F(SearchTest, replace21)
{
    Start(600);

    document.InsertString(U"A mãe fez um bolo para o jantar.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"As mães dos alunos ajudaram na festa.", true));
    const std::u32string original = U"A mãe fez um bolo para o jantar.\nAs mães dos alunos ajudaram na festa.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //a single Replace works on the current match - the singular mãe
    document.WaitTask(document.Search(U"mãe", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"tia", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"A tia fez um bolo para o jantar.\nAs mães dos alunos ajudaram na festa.")
        << ToBasicString(document.ToText());
    uint count = 0, current = 0;
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: the plural mães is replaced too
    document.WaitTask(document.Search(U"mãe", false));
    document.WaitTask(document.ReplaceAll(U"tia", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"A tia fez um bolo para o jantar.\nAs tias dos alunos ajudaram na festa.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"A tia fez um bolo para o jantar.\nAs tias dos alunos ajudaram na festa.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //whole word: the plural mães keeps its match
    document.WaitTask(document.Search(U"mãe", false, true));
    document.WaitTask(document.ReplaceAll(U"tia", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"A tia fez um bolo para o jantar.\nAs mães dos alunos ajudaram na festa.")
        << ToBasicString(document.ToText());
}

//Russian: the Cyrillic letters fold their case
TEST_F(SearchTest, search22)
{
    Start(600);

    document.InsertString(U"Маленький мальчик шёл в школу через лес.", true);
    document.InsertParagraph(true);
    document.InsertString(U"Ёлка стояла в углу комнаты, дети веселились.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Подъезд дома был закрыт, лестница скрипела.", true));
    ASSERT_TRUE(document.ToText() == U"Маленький мальчик шёл в школу через лес.\nЁлка стояла в углу комнаты, дети веселились.\n"
        U"Подъезд дома был закрыт, лестница скрипела.") << ToBasicString(document.ToText());

    uint count = 0, current = 0;
    document.WaitTask(document.Search(U"мальчик", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"МАЛЬЧИК", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"МАЛЬЧИК", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 0) << count;

    //лес sits inside лестница; as a whole word only the standalone one counts
    document.WaitTask(document.Search(U"лес", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.WaitTask(document.Search(U"лес", false, true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"ЁЛКА", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"ШЁЛ", false));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.Search(U"шёл", true));
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;
}

//Russian replace: a single replacement, then case-insensitive, case-sensitive and whole word replace-all
TEST_F(SearchTest, replace22)
{
    Start(600);

    document.InsertString(U"Кот сидел на крыльце, а котёнок играл рядом.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"Коты любят рыбу и молоко.", true));
    const std::u32string original = U"Кот сидел на крыльце, а котёнок играл рядом.\nКоты любят рыбу и молоко.";
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    uint count = 0, current = 0;

    //a single Replace works on the current match - the capitalized Кот
    document.WaitTask(document.Search(U"кот", false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Replace(U"лев", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"лев сидел на крыльце, а котёнок играл рядом.\nКоты любят рыбу и молоко.")
        << ToBasicString(document.ToText());
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 2) << count;

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-insensitive: the derivative words are replaced too
    document.WaitTask(document.Search(U"кот", false));
    document.WaitTask(document.ReplaceAll(U"лев", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"лев сидел на крыльце, а левёнок играл рядом.\nлевы любят рыбу и молоко.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"лев сидел на крыльце, а левёнок играл рядом.\nлевы любят рыбу и молоко.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //case-sensitive: only the capitalized words Кот and Коты, the lowercase котёнок keeps its match
    document.WaitTask(document.Search(U"Кот", true));
    document.WaitTask(document.ReplaceAll(U"лев", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"лев сидел на крыльце, а котёнок играл рядом.\nлевы любят рыбу и молоко.")
        << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == original) << ToBasicString(document.ToText());

    //whole word: котёнок and Коты keep their matches
    document.WaitTask(document.Search(U"кот", false, true));
    std::this_thread::sleep_for(100ms);
    document.GetSearchState(count, current);
    ASSERT_TRUE(count == 1) << count;

    document.WaitTask(document.ReplaceAll(U"пёс", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"пёс сидел на крыльце, а котёнок играл рядом.\nКоты любят рыбу и молоко.")
        << ToBasicString(document.ToText());
}

}
