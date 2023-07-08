#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

TEST_F(AssignmentTest, assignment1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//User variable
TEST_F(AssignmentTest, assignment2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("67", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>67</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 2})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertParagraph(true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>67</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>72.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//User function
TEST_F(AssignmentTest, assignment3)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("x", true);
    document.InsertCloseFence(true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>f</mi>"\
                            "<mo>(</mo>"\
                            "<mi>x</mi>"\
                            "<mo>)</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertParagraph(true);
    document.InsertString("f", true);
    document.InsertOpenFence(true);
    document.InsertString("4", true);
    document.InsertCloseFence(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>f</mi>"\
                            "<mo>(</mo>"\
                            "<mi>x</mi>"\
                            "<mo>)</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>f</mi>"\
                            "<mo>(</mo>"\
                            "<mi>4</mi>"\
                            "<mo>)</mo>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>9.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Edit an assignment
TEST_F(AssignmentTest, assignment4)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("6", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>6</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                            "<mo>+</mo>"\
                            "<mi>6</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 2, 1})) << document.GetEditorState().ToString();
}

//Insert a paragraph after assignment
TEST_F(AssignmentTest, assignment5)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of assignment
TEST_F(AssignmentTest, delete1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x5</mi>"\
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
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Deletion of assignment
TEST_F(AssignmentTest, delete2)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();
}

//Assign to a number is a error
TEST_F(AssignmentTest, error1)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertAssignment(true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"123="
        ) << ToBasicString(document.ToText());
    int start, size;
    ASSERT_TRUE(document.HasErrorMark(ElementId{0, 0, 0, 0, 0, 0, 0}, start, size)) << ErrorMarks();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.error_marks.empty());
}

}
