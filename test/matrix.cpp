/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "../src/formulas/matrix.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

static const ElementId matrix_id{0, 0, 0, 0, 0, 0, 0};

//Insert an empty 2x2 matrix and undo/redo it
TEST_F(FormulaTest, matrix1)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mo>(</mo>"
                        "<mtable>"
                            "<mtr>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                            "</mtr>"
                            "<mtr>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                            "</mtr>"
                        "</mtable>"
                        "<mo>)</mo>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"[[,],[,]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//The cells cannot be deleted - an empty cell keeps its empty string placeholder and the caret stays inside it
TEST_F(FormulaTest, matrix2)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));

    //each cell contains exactly one empty string element
    for (uint i = 1; i <= 4; ++i)
    {
        Element* cell = document.GetElement(GetChild(matrix_id, i)).get();
        ASSERT_TRUE(cell != nullptr);
        ASSERT_TRUE(cell->type == ElementType::CODE_ROW) << (int)cell->type;
        ASSERT_TRUE(cell->elements->Count() == 1) << cell->elements->Count();
        ASSERT_TRUE(cell->elements->Get(0)->type == ElementType::CODE_STRING);
        ASSERT_TRUE(cell->elements->Get(0)->elements->Count() == 0);
        ASSERT_TRUE(cell->IsEmpty());
    }

    //the caret in an empty cell points inside the string element, not at the cell itself
    ASSERT_TRUE(document.GetEditorState().caret_state.id.size() == matrix_id.size() + 3) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    //backspace and delete on the placeholder do nothing
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"[[,],[,]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"[[,],[,]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    //backspace at the start of a cell does not delete anything - the cells are fixed
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"[[,],[,]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Fill the cells, move the caret through the grid, clear a cell back to its placeholder
TEST_F(FormulaTest, matrix3)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 4, 0, 1})) << document.GetEditorState().ToString();

    //vertical movements stay in the same column
    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 4, 0, 1})) << document.GetEditorState().ToString();

    //horizontal movements pass through the cells in reading order
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3, 0, 1})) << document.GetEditorState().ToString();

    //clear the first cell - the cell itself is not deleted, the placeholder is restored
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"[[,2],[3,4]]") << ToBasicString(document.ToText());
    Element* cell = document.GetElement(GetChild(matrix_id, 1)).get();
    ASSERT_TRUE(cell->elements->Count() == 1 && cell->elements->Get(0)->elements->Count() == 0);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());
}

//A non-square matrix with column alignment
TEST_F(FormulaTest, matrix4)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 3));
    ASSERT_TRUE(document.ToText() == U"[[,,],[,,]]") << ToBasicString(document.ToText());

    document.InsertString("11", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("333", true);
    document.MoveCaretRight(false);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertString("55", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("6", true));
    ASSERT_TRUE(document.ToText() == U"[[11,2,333],[4,55,6]]") << ToBasicString(document.ToText());

    //the columns are aligned - the cells of one column share the center
    Element* cell_00 = document.GetElement(GetChild(matrix_id, 1)).get();
    Element* cell_01 = document.GetElement(GetChild(matrix_id, 2)).get();
    Element* cell_02 = document.GetElement(GetChild(matrix_id, 3)).get();
    Element* cell_10 = document.GetElement(GetChild(matrix_id, 4)).get();
    Element* cell_11 = document.GetElement(GetChild(matrix_id, 5)).get();
    ASSERT_TRUE(cell_00->rect.left + cell_00->rect.width / 2 == cell_10->rect.left + cell_10->rect.width / 2)
        << cell_00->rect.left << " " << cell_10->rect.left;
    ASSERT_TRUE(cell_01->rect.left + cell_01->rect.width / 2 == cell_11->rect.left + cell_11->rect.width / 2)
        << cell_01->rect.left << " " << cell_11->rect.left;
    ASSERT_TRUE(cell_02->rect.width > cell_01->rect.width) << cell_02->rect.width << " " << cell_01->rect.width;
}

//Determinant: vertical line brackets and det(...) text
TEST_F(FormulaTest, matrix5)
{
    Start(600);

    document.WaitTask(document.InsertDeterminant(true, 2, 2));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"
            "<p>"
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mo>|</mo>"
                        "<mtable>"
                            "<mtr>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                            "</mtr>"
                            "<mtr>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                                "<mtd><mrow><mi></mi></mrow></mtd>"
                            "</mtr>"
                        "</mtable>"
                        "<mo>|</mo>"
                    "</mrow>"
                "</math>"
            "</p>"
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.ToText() == U"det([[1,2],[3,4]])") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"det([[1,2],[3,4]])") << ToBasicString(document.ToText());
}

//The whole element is deleted from a bracket, undo restores it
TEST_F(FormulaTest, matrix6)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    //the brackets are not caret stops - the single stop after the grid is the position after the matrix
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,6 []") << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"[[1,],[,]]") << ToBasicString(document.ToText());
}

//Copy-paste a matrix
TEST_F(FormulaTest, matrix7)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());

    //move the caret past the matrix and select back - the whole matrix is selected as one element
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]][[1,2],[3,4]]") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());
}

//Save and load with the grid size
TEST_F(FormulaTest, matrix8)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertMatrix(true, 2, 3));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(document.ToText() == U"[[1,2,3],[,,]]") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("matrix1.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Load("matrix1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"[[1,2,3],[,,]]") << ToBasicString(document.ToText());

    //the grid size survives the roundtrip
    uint rows = 0, columns = 0;
    ASSERT_TRUE(document.GetMatrixSize(ElementId{0, 0, 0, 0, 0, 0, 0}, rows, columns));
    ASSERT_TRUE(rows == 2 && columns == 3) << rows << " " << columns;
}

//Resize: grow adds empty cells to the right and bottom, shrink removes them from the right and bottom
TEST_F(FormulaTest, matrix9)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());

    //grow to 3x3
    document.WaitTask(document.SetMatrixSize(matrix_id, 3, 3, true));
    ASSERT_TRUE(document.ToText() == U"[[1,2,],[3,4,],[,,]]") << ToBasicString(document.ToText());
    uint rows = 0, columns = 0;
    ASSERT_TRUE(document.GetMatrixSize(matrix_id, rows, columns));
    ASSERT_TRUE(rows == 3 && columns == 3) << rows << " " << columns;
    //the new cells contain their placeholders
    Element* cell = document.GetElement(GetChild(matrix_id, 1 + 0 * 3 + 2)).get();
    ASSERT_TRUE(cell->elements->Count() == 1 && cell->elements->Get(0)->elements->Count() == 0);

    //shrink back to 2x2 - the cells are removed from the right and bottom
    document.WaitTask(document.SetMatrixSize(matrix_id, 2, 2, true));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());

    //undo restores the 3x3 grid with its content
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"[[1,2,],[3,4,],[,,]]") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetMatrixSize(matrix_id, rows, columns));
    ASSERT_TRUE(rows == 3 && columns == 3) << rows << " " << columns;

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]") << ToBasicString(document.ToText());
}

//The caret keeps working after the cell it was in is removed by shrinking
TEST_F(FormulaTest, matrix10)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 4, 0, 1})) << document.GetEditorState().ToString();

    //the caret sits in the last cell, shrink removes that cell
    document.WaitTask(document.SetMatrixSize(matrix_id, 1, 1, true));
    ASSERT_TRUE(document.ToText() == U"[[1]]") << ToBasicString(document.ToText());
    //the caret was moved into the surviving cell
    ASSERT_TRUE(document.GetEditorState().caret_state.IsInsideElement(matrix_id)) << document.GetEditorState().ToString();

    //typing still works
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"[[15]]") << ToBasicString(document.ToText());
}

//A partially filled matrix inside an equation does not solve and shows no errors
TEST_F(FormulaTest, matrix11)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[,]]") << ToBasicString(document.ToText());

    //move the caret out of the matrix and append an equation
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(!document.HasErrorMarks({0})) << ToBasicString(document.ToText()) << " marks: " << ErrorMarks();
}

//The cells get a deeper level than the matrix
TEST_F(FormulaTest, matrix12)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    Element* matrix = document.GetElement(matrix_id).get();
    ASSERT_TRUE(matrix->level == 1) << (int)matrix->level;
    for (uint i = 1; i <= 4; ++i)
    {
        Element* cell = document.GetElement(GetChild(matrix_id, i)).get();
        ASSERT_TRUE(cell->level == 2) << (int)cell->level;
    }
}

//Determinant save/load
TEST_F(FormulaTest, matrix13)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertDeterminant(true, 3, 3));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.InsertString("4", true);
    document.MoveCaretRight(false);
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.InsertString("6", true);
    document.MoveCaretRight(false);
    document.InsertString("7", true);
    document.MoveCaretRight(false);
    document.InsertString("8", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("10", true));
    ASSERT_TRUE(document.ToText() == U"det([[1,2,3],[4,5,6],[7,8,10]])") << ToBasicString(document.ToText());

    document.WaitTask(document.Save("determinant1.yut"));
    document.WaitTask(document.New());
    document.Load("determinant1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"det([[1,2,3],[4,5,6],[7,8,10]])") << ToBasicString(document.ToText());

    Element* el = document.GetElement(matrix_id).get();
    ASSERT_TRUE(el != nullptr && el->type == ElementType::DETERMINANT) << (int)(el ? el->type : ElementType::NONE);
}


//Vertical moves in a matrix with empty cells keep the column of the current cell
TEST_F(FormulaTest, matrix14)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 3, 3));

    //cell (1,0) is the fourth cell in the reading order
    for (int i = 0; i < 2; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 4, 0, 0})) << document.GetEditorState().ToString();

    //up goes to the cell above in the same column, not one column right
    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 4, 0, 0})) << document.GetEditorState().ToString();

    //the same from the middle column
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 5, 0, 0})) << document.GetEditorState().ToString();
}

//Selecting down from an empty cell selects the whole matrix
TEST_F(FormulaTest, matrix15)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));

    //shift+down from the first cell selects the whole matrix as one element
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    //the same for the determinant
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertDeterminant(true, 2, 2));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//A single caret position separates the grid and the following element
TEST_F(FormulaTest, matrix16)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));

    //one press leaves the grid: the stop right after the closing bracket
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,6 []") << document.GetEditorState().ToString();

    //an equation can be appended right away
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    ASSERT_TRUE(document.ToText() == U"[[1,2],[3,4]]=~") << ToBasicString(document.ToText());

    //a single left press from the equation returns to the same position after the closing bracket -
    //the only caret position between the bracket and "="
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,0,0,6 []") << document.GetEditorState().ToString();
}

//Large matrices up to 100x100
TEST_F(FormulaTest, matrix17)
{
    Start(600);

    //an empty 50x50 grid
    document.WaitTask(document.InsertMatrix(true, 50, 50));
    uint rows = 0, columns = 0;
    ASSERT_TRUE(document.GetMatrixSize(matrix_id, rows, columns));
    ASSERT_TRUE(rows == 50 && columns == 50) << rows << " " << columns;
    std::u32string text = document.ToText();
    ASSERT_TRUE(text.size() == 51 * 51) << text.size(); //an empty n x n grid text is (n + 1)^2 characters

    //grow to the maximum size
    document.WaitTask(document.SetMatrixSize(matrix_id, 100, 100, true));
    ASSERT_TRUE(document.GetMatrixSize(matrix_id, rows, columns));
    ASSERT_TRUE(rows == 100 && columns == 100) << rows << " " << columns;
    text = document.ToText();
    ASSERT_TRUE(text.size() == 101 * 101) << text.size();

    //a filled 12x12 matrix
    document.WaitTask(document.New());
    document.WaitTask(document.InsertMatrix(true, 12, 12));
    for (uint i = 0; i < 12 * 12; ++i)
    {
        document.InsertString("7", true);
        if (i + 1 < 12 * 12)
            document.MoveCaretRight(false);
    }
    std::this_thread::sleep_for(500ms);
    std::u32string expected = U"[";
    for (uint r = 0; r < 12; ++r)
    {
        if (r > 0)
            expected += U",";
        expected += U"[";
        for (uint c = 0; c < 12; ++c)
        {
            if (c > 0)
                expected += U",";
            expected += U"7";
        }
        expected += U"]";
    }
    expected += U"]";
    ASSERT_TRUE(document.ToText() == expected) << ToBasicString(document.ToText()).substr(0, 80);
}

//The brackets are glyphs whose line boxes stay inside the element rect, their ink covers the grid,
//and the grid is centered on the ink center - the matrix baseline
TEST_F(FormulaTest, matrix18)
{
    Start(600);

    //a glyph line box covering the requested height, like the real fonts return it from GetSymbolSize
    ON_CALL(window_mock, GetSymbolSize).WillByDefault(
        [](const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline)
        {
            size.Set(std::max(2, height / 3), height);
            baseline = height * 4 / 5;
            return height;
        });

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));

    Element* matrix = document.GetElement(matrix_id).get();
    Element* left = document.GetElement(GetChild(matrix_id, 0)).get();
    Element* right = document.GetElement(GetChild(matrix_id, 5)).get();
    Element* first_cell = document.GetElement(GetChild(matrix_id, 1)).get();
    Element* last_cell = document.GetElement(GetChild(matrix_id, 4)).get();
    //the whole line boxes sit inside the element rect, from its very top
    ASSERT_TRUE(left->rect.top == 0 && right->rect.top == 0) << left->rect.top << " " << right->rect.top;
    ASSERT_TRUE(left->rect.height > 0 && right->rect.height > 0) << left->rect.height << " " << right->rect.height;
    ASSERT_TRUE(left->rect.GetBottom() <= matrix->rect.height && right->rect.GetBottom() <= matrix->rect.height)
        << left->rect.GetBottom() << " " << right->rect.GetBottom() << " " << matrix->rect.height;
    ASSERT_TRUE(right->rect.left > left->rect.GetRight() && right->rect.GetRight() == matrix->rect.width)
        << right->rect.left << " " << left->rect.GetRight() << " " << right->rect.GetRight() << " " << matrix->rect.width;
    //the bracket ink (spans ~[height/6, height] of the line box) covers the grid
    ASSERT_TRUE(left->rect.height / 6 <= first_cell->rect.top)
        << left->rect.height / 6 << " " << first_cell->rect.top;
    ASSERT_TRUE(left->rect.height >= last_cell->rect.GetBottom())
        << left->rect.height << " " << last_cell->rect.GetBottom();
    //the grid is centered on the ink center - it is the matrix baseline
    ASSERT_TRUE(left->rect.height * 7 / 12 == matrix->baseline)
        << left->rect.height << " " << matrix->baseline;
}

//The brackets are not caret stops in any direction: leaving left lands on the matrix itself, entering left
//goes straight into the first cell, leaving right is the single after-matrix stop, entering right returns
//into the last cell
TEST_F(FormulaTest, matrix19)
{
    Start(600);

    document.WaitTask(document.InsertMatrix(true, 2, 2));
    document.InsertString("1", true); //cell(0,0), the caret after "1"
    document.MoveCaretLeft(false); //before "1", still inside the first cell
    const std::string first_cell_start = document.GetEditorState().ToString();

    //leaving left: the single stop left of the grid is the matrix itself - no stop on the left bracket
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0 []") << document.GetEditorState().ToString();

    //entering from the left lands directly in the first cell - no stop on the left bracket
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == first_cell_start)
        << document.GetEditorState().ToString() << " != " << first_cell_start;

    //fill up to the last cell (4 rights: one crosses the "1" itself, then one per remaining cell)
    //and leave right: a single stop after the grid, no stop on the right bracket
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("4", true));
    const std::string last_cell_end = document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == "0,0,0,0,0,0,0,6 []") << document.GetEditorState().ToString();

    //entering from the right returns straight into the last cell - no stop on the right bracket
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState().ToString() == last_cell_end)
        << document.GetEditorState().ToString() << " != " << last_cell_end;
}

//Insert a vector column: a single-column grid; the parser text is the column vector literal
TEST_F(FormulaTest, vector_column1)
{
    Start(600);

    document.WaitTask(document.InsertVectorColumn(true, 3));
    ASSERT_TRUE(document.ToText() == U"[[],[],[]]") << ToBasicString(document.ToText());

    //fill the cells in reading order: the caret starts in the first cell
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("3", true));
    ASSERT_TRUE(document.ToText() == U"[[1],[2],[3]]") << ToBasicString(document.ToText());
}

//A vector inserted by the button solves through the vector result type
TEST_F(SolverVectorTest, vector_insert1)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertVectorColumn(true, 3));
    document.InsertString("1", true);
    document.MoveCaretRight(false);
    document.InsertString("2", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString("3", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::VECTOR_REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == U"[[1],[2],[3]]=[[1.],[2.],[3.]]") << ToBasicString(document.ToText());
}

}
