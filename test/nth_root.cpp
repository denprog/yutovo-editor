#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(FormulaTest, nth_root1)
{
    document.InsertNthRoot(true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertString("2", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("3", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(FormulaTest, nth_root2)
{
    document.InsertNthRoot(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("12", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>312</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.InsertString("5", true);
    document.WaitMainLoop();
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5312</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5</mi>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true, false));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5312</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5</mi>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.WaitTask(document.InsertString("7", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5</mi>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>12</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                        "<mi>7</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(FormulaTest, nth_root3)
{
    document.InsertNthRoot(true);
    document.InsertString("3", true);
    document.InsertPlus(true);
    document.InsertString("1.4", true);
    document.WaitMainLoop();
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("2", true);
    document.InsertMultiply(true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("5", true));
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitCaretMoving();
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("67", true));
    document.WaitTask(document.Save("nth_root3_1.yut"));

    document.WaitTask(document.New());
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Load("nth_root3_1.yut"));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mroot>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>×</mo>"\
                                "<mfrac>"\
                                    "<mrow>"\
                                        "<mi>5</mi>"\
                                    "</mrow>"\
                                    "<mrow>"\
                                        "<mi>67</mi>"\
                                    "</mrow>"\
                                "</mfrac>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>3</mi>"\
                                "<mo>+</mo>"\
                                "<mi>1.4</mi>"\
                            "</mrow>"\
                        "</mroot>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();
}

}
