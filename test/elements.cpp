/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "str.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Out of range positions and sizes in the Elements container must not corrupt the document
TEST_F(DocumentTest, elements1)
{
    Start(600);

    document.WaitTask(document.InsertString(U"a", true));
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    auto text = document.GetElement(ElementId{0});
    ASSERT_TRUE(text != nullptr);
    Elements* paragraphs = text->elements.get();
    ASSERT_TRUE(paragraphs->Count() == 1);
    ASSERT_FALSE(paragraphs->GetElementId(0).empty());

    //an out of range position returns an empty id
    ASSERT_TRUE(paragraphs->GetElementId(100).empty());

    //removing at an out of range position does nothing
    paragraphs->RemoveAt(100, 1);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    //removing at a position past the end does nothing
    paragraphs->RemoveAt(1, 100);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    //removing an unknown id does nothing (the position resolves to -1)
    paragraphs->Remove(ElementId{100});
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    //replacing at an out of range position does nothing
    paragraphs->Replace(ElementPtr(paragraphs->Get(0)->Clone()), 100);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    //the no-op calls keep the undo stack consistent
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    //undo/redo replaces the elements, take the actual container
    text = document.GetElement(ElementId{0});
    paragraphs = text->elements.get();
    ASSERT_TRUE(paragraphs->Count() == 1);

    //inserting at an out of range position appends to the end
    paragraphs->Insert(ElementPtr(paragraphs->Get(0)->Clone()), 100);
    ASSERT_TRUE(paragraphs->Count() == 2);
    ASSERT_TRUE(document.ToText() == U"a\na") << ToBasicString(document.ToText());

    //replacing with an out of range position appends to the end
    std::vector<ElementPtr> new_elements;
    new_elements.push_back(ElementPtr(paragraphs->Get(0)->Clone()));
    paragraphs->Replace(1000, 1, new_elements);
    ASSERT_TRUE(paragraphs->Count() == 3);
    ASSERT_TRUE(document.ToText() == U"a\na\na") << ToBasicString(document.ToText());

    //replacing with an out of range size removes up to the end only
    paragraphs->RemoveAt(1, 1000);
    ASSERT_TRUE(paragraphs->Count() == 1);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());

    //inserting a null element does nothing
    paragraphs->Insert(nullptr, 0);
    ASSERT_TRUE(paragraphs->Count() == 1);
    ASSERT_TRUE(document.ToText() == U"a") << ToBasicString(document.ToText());
}

//Out of range positions and sizes in the StringElements container must not corrupt the string
TEST_F(DocumentTest, elements2)
{
    Start(600);

    document.WaitTask(document.InsertString(U"ab", true));
    ASSERT_TRUE(document.ToText() == U"ab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    auto str = document.GetElement(ElementId{0, 0, 0, 0});
    ASSERT_TRUE(str != nullptr);
    ASSERT_TRUE(str->elements != nullptr);
    StringElements* string_elements = dynamic_cast<StringElements*>(str->elements.get());
    ASSERT_TRUE(string_elements != nullptr);
    ElementPtr clone(str->Clone()); //a copy of the "ab" string

    //an out of range position clamps to the last caret position of the string
    ASSERT_TRUE(string_elements->GetElementId(100) == string_elements->GetElementId(2));

    //removing at an out of range position does nothing
    string_elements->RemoveAt(100, 1);
    ASSERT_TRUE(document.ToText() == U"ab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    //removing an unknown id does nothing (the position resolves to -1)
    str->elements->Remove(ElementId{100, 100, 100, 100});
    ASSERT_TRUE(document.ToText() == U"ab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    //inserting at an out of range position appends to the end
    string_elements->Insert(clone, 100);
    ASSERT_TRUE(document.ToText() == U"abab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    //replacing at an out of range position appends to the end
    string_elements->Replace(clone, 100);
    ASSERT_TRUE(document.ToText() == U"ababab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    //inserting a null element does nothing
    string_elements->Insert(nullptr, 0);
    ASSERT_TRUE(document.ToText() == U"ababab") << ToBasicString(document.ToText());

    //replacing with a null element does nothing
    string_elements->Replace(nullptr, 0);
    ASSERT_TRUE(document.ToText() == U"ababab") << ToBasicString(document.ToText());

    //removing with an out of range size removes up to the end of the string only
    string_elements->RemoveAt(2, 100);
    ASSERT_TRUE(document.ToText() == U"ab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
}

}
