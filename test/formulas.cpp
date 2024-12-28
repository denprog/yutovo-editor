#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "formulas/code_string.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, delete1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.InsertPlus(true);
    document.InsertPlus(true);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete with undo a text and a code block
TEST_F(FormulaTest, delete2)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete with undo a text and a code block and a text
TEST_F(FormulaTest, delete3)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertString("Block", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Teck</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Block</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Teck</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Block</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();
}

//Delete a selected code block at the beginning of the text
TEST_F(FormulaTestCustom, delete4)
{
    Start(433);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of a selected formula
TEST_F(FormulaTestCustom, delete5)
{
    Start(450);

    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of a selected formula with a text
TEST_F(FormulaTestCustom, delete6)
{
    Start(380);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 17; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 14; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 16; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
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
                "<span style=\"font-family:'Arial';font-size:14px;\">Th text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of</span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\"> the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 2, 11},
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 0, 0, 3}, 0, 4})) << document.GetEditorState().ToString();
}

//Delete an element on the left of a string
TEST_F(FormulaTestCustom, delete7)
{
    Start(380);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("55", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d</mi>"\
                        "<mo>+</mo>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a character from the right
TEST_F(FormulaTestCustom, delete8)
{
    Start(380);

    document.InsertCode(false, true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertString("6", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                        "<mo>+</mo>"\
                        "<mi>6</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete an element from the right
TEST_F(FormulaTestCustom, delete9)
{
    Start(380);

    document.InsertCode(false, true);
    document.InsertString("45", true);
    document.InsertPlus(true);
    document.InsertPlus(true);
    document.InsertMinus(true);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>45</mi>"\
                        "<mo>+</mo>"\
                        "<mo>+</mo>"\
                        "<mo>-</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Delete a code block on the left
TEST_F(FormulaTestCustom, delete10)
{
    Start(368);

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 11})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 2})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, delete11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertPlus(true);
    document.WaitTask(document.DeleteElements(true, true));
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, delete12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"45") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123+45") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"45") << ToBasicString(document.ToText());
}

TEST_F(FormulaTest, delete13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.MoveCaretHome(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123+45") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
}

TEST_F(FormulaTestCustom, insert1)
{
    Start(450);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    std::this_thread::sleep_for(400ms);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el medio de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">un</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> documento era el papel y </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert code in text and reformat text
TEST_F(FormulaTestCustom, insert2)
{
    Start(433);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert formula, remake and undo
TEST_F(FormulaTestCustom, insert3)
{
    Start(380);

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">strange</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a formula at the end of a string
TEST_F(FormulaTestCustom, insert4)
{
    Start(432);

    document.WaitTask(document.InsertString("Text Text Text Text Text Text Text Text 3", true));
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("f", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text Text Text Text Text Text Text Text 3f</span>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 42})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTestCustom, insert5)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("56890", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>568</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>90</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
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
                        "<mi>123</mi>"\
                        "<mo>+</mo>"\
                        "<mi>56890</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Inserting a paragraph is prohibited
TEST_F(FormulaTestCustom, insert6)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertString("56", true);
    document.InsertPower(true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTestCustom, insert7)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a paragraph
TEST_F(FormulaTestCustom, insert8)
{
    Start(432);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertMultiply(true);
    document.InsertString("56", true);
    document.InsertPower(true);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>×</mo>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msup>"\
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
                        "<mi>3</mi>"\
                        "<mo>×</mo>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert, resize, undo and redo
TEST_F(FormulaTestCustom, insert9)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertSquareRoot(true));

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();

    ASSERT_TRUE(document.ToText() == 
        U"Tradicionalmente, el medio de un documento era el papel y la información"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 72})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Tradicionalmente, el medio de un documento era el papel y la información"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Tradicionalmente, el medio de un documento era el papel y la informaciónsqrt()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Insert minus inside a string
TEST_F(FormulaTestCustom, insert10)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("750", true);
    document.InsertMinus(true);
    document.InsertString("18", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertMinus(true));
    ASSERT_TRUE(document.ToText() == 
        U"750-1-8"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"750-18"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"750-1-8"
        ) << ToBasicString(document.ToText());
}

//Insert minus inside a string
TEST_F(FormulaTestCustom, insert11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("750", true);
    document.InsertMinus(true);
    document.InsertString("18", true);
    document.MoveCaretHome(true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertMinus(true));
    ASSERT_TRUE(document.ToText() == 
        U"(750-1-8)/(3)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"(750-18)/(3)"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"(750-1-8)/(3)"
        ) << ToBasicString(document.ToText());
}

//Insert a formula before a code block
TEST_F(FormulaTestCustom, insert12)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of a formula
TEST_F(FormulaTestCustom, select1)
{
    Start(600);

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(false));
    document.WaitTask(document.InsertString("25", true));
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection of a formula at the beginning of a text
TEST_F(FormulaTestCustom, select2)
{
    Start(600);

    document.InsertString("text", true);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection of a formula at the end of a row
TEST_F(FormulaTestCustom, select3)
{
    Start(400);

    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(800ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 2})) << document.GetEditorState().ToString();

    width = 380;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 2})) << document.GetEditorState().ToString();
}

//Selection of a child element of a formula at the end of a row
TEST_F(FormulaTestCustom, select4)
{
    Start(424);

    int width = 424;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    width = 410;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 1, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Selection of a part of row, a formula and a part of row
TEST_F(FormulaTestCustom, select5)
{
    Start(400);

    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 4})) << document.GetEditorState().ToString();

    width = 380;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 4})) << document.GetEditorState().ToString();

    width = 350;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2, 4}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 4, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 1, 2}, 0, 4})) << document.GetEditorState().ToString();

    width = 380;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 39, 3},
        ElementSelectionState{ElementId{0, 0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 1, 1}, 0, 4})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.InsertString("234", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(234)\n" \
        U"(123)/(123)"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select7)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("12", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("123245345", true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select8)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("12", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("123245345", true));
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a code block
TEST_F(FormulaTest, select9)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Select a code block
TEST_F(FormulaTest, select10)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("123", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection rows in a code block
TEST_F(FormulaTest, select11)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("234", true);
    document.InsertParagraph(true);
    document.InsertString("434", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3453", true));
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0, 0, 0, 1, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection after a power
TEST_F(FormulaTest, select12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.ToText() == U"pow(123,2)+45") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();
}

//Replace a selection
TEST_F(FormulaTest, select13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertPlus(true));
    ASSERT_TRUE(document.ToText() == U"+") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Replace a selection
TEST_F(FormulaTest, select14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertPlus(true));
    document.InsertString("55", true);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertMinus(true));
    ASSERT_TRUE(document.ToText() == U"123-") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123+55") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Select a symbol and a complex element
TEST_F(FormulaTest, select15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("67", true);
    document.InsertPlus(true);
    document.InsertSquareRoot(true);
    document.InsertString("3", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Select an element and insert an element
TEST_F(FormulaTest, select16)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("1", true);
    document.InsertDivision(true);
    document.InsertString("2", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msqrt>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</msqrt>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>3</mi>"\
                        "<mo>+</mo>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>1</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Check caret after undo
TEST_F(FormulaTest, select17)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.InsertString("34", true);
    document.InsertPlus(true);
    document.InsertString("45", true);
    document.InsertDivision(true);
    document.InsertString("67", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.InsertSquareRoot(true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(sqrt(34+(45)/(67)))"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(34+(45)/(67))"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertPlus(true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(34+(45)/(67)+)"
        ) << ToBasicString(document.ToText());
}

TEST_F(FormulaTest, fonts1)
{
    Start(600);

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.SetFontSize(12));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
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
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Set font
TEST_F(FormulaTest, fonts2)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.SetFontSize(12);
    document.WaitTask(document.InsertString("56", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);

    document.SetBold(true);
    document.WaitTask(document.InsertString("77", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                        "<mi>77</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 2});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 12);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->size == 14);
}

//Set color
TEST_F(FormulaTest, fonts3)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.SetColor(Color::Red());
    document.WaitTask(document.InsertString("56", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Red());

    document.SetBgColor(Color::Blue());
    document.WaitTask(document.InsertString("77", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                        "<mi>77</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 2});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_bg_color == Color::Blue());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi>56</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 1});
    ASSERT_TRUE(s && s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Red());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->text_color == Color::Black());
}

//Set a font attribute
TEST_F(FormulaTest, fonts4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    auto s = document.GetElement(ElementId{0, 0, 0, 0, 0, 0, 0});
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->bold == true);

    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(s->type == ElementType::CODE_STRING && ((CodeString*)s.get())->GetStringFormat()->bold == false);
}

//Set a font attribute
TEST_F(FormulaTest, fonts5)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("123", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("55", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));

    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0}, format));
    ASSERT_FALSE(format.italic);

    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));

    // ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    // ASSERT_TRUE(format.bold);
    // ASSERT_FALSE(format.italic);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.bold);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.SetBold(false));

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_FALSE(format.bold);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_FALSE(format.bold);
}

//Set a font attribute
TEST_F(FormulaTest, fonts6)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.InsertString("f", true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_FALSE(format.bold);

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0}, format));
    ASSERT_FALSE(format.bold);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_FALSE(format.underline);

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(false));
    // ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    // ASSERT_FALSE(format.bold);
    // ASSERT_FALSE(format.underline);

    document.WaitTask(document.InsertPlus(true));
    document.WaitTask(document.InsertString("55", true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 2}, format));
    ASSERT_FALSE(format.bold);
    ASSERT_FALSE(format.underline);

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetFontSize(16));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 2}, format));
    ASSERT_TRUE(format.size == 16);
    ASSERT_FALSE(format.bold);
    ASSERT_FALSE(format.underline);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("1", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(f1)+55"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 0}, format));
    ASSERT_FALSE(format.bold);
    ASSERT_FALSE(format.underline);
    ASSERT_TRUE(format.size == 16);

    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_FALSE(format.bold);
    ASSERT_FALSE(format.underline);
    ASSERT_FALSE(format.size == 16);

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.SetFontSize(22));
    document.WaitTask(document.InsertString("77", true));
    ASSERT_TRUE(document.ToText() == 
        U"(123)/(f1)+5577"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 2}, format));
    ASSERT_TRUE(format.size == 16);
    ASSERT_FALSE(format.bold);
    ASSERT_FALSE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0, 0, 3}, format));
    ASSERT_TRUE(format.size == 22);
    ASSERT_FALSE(format.bold);
    ASSERT_FALSE(format.underline);
}

}
