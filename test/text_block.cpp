/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <fstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "mock.h"
#include "style.h"
#include "formulas/text_block.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Insert a text block into the document text and type a string
TEST_F(TextBlockTest, text_block1)
{
    Start(600);

    document.WaitTask(document.InsertTextBlock(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("m", true));
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//The equation inserted inside a text block never solves and its right part stays editable
TEST_F(TextBlockTest, text_block2)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    std::this_thread::sleep_for(100ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    //edit the right part of the text equation
    document.WaitTask(document.InsertString("3", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    //nothing is computed even after the solve delay - no waiting symbol, no result
    std::this_thread::sleep_for(500ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//The assignment inserted inside a text block does not register identifiers
TEST_F(TextBlockTest, text_block3)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    std::this_thread::sleep_for(100ms);
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
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(500ms);
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
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
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
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
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
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//A text block can be inserted inside a code block and stays a separate non-solving block
TEST_F(TextBlockTest, text_block4)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertTextBlock(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</math>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertString("x", true);
    document.InsertEquation(yutovo_solver::ResultType::AUTO, true);
    document.WaitTask(document.InsertString("7", true));
    std::this_thread::sleep_for(500ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
                                "<mrow>"\
                                    "<mi>7</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</math>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</math>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
                                "<mrow>"\
                                    "<mi>7</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</math>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Regular formulas work inside a text block as in a code block
TEST_F(TextBlockTest, text_block5)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.WaitTask(document.InsertString("2", true));
    std::this_thread::sleep_for(100ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>x</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup><mrow><mi>x</mi></mrow><mrow><mi>2</mi></mrow></msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Enter inserts a new code paragraph inside a text block
TEST_F(TextBlockTest, text_block6)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("a", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("b", true));
    std::this_thread::sleep_for(100ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>a</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>b</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>a</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>a</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>b</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Save and load a text block - nothing is computed after loading
TEST_F(TextBlockTest, text_block7)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.InsertEquation(yutovo_solver::ResultType::AUTO, true);
    document.WaitTask(document.InsertString("3", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Save("text_block7.yut"));
    document.Load("text_block7.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(500ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}

//Text block colors are configurable and serializable
TEST_F(TextBlockTest, text_block8)
{
    Start(600);

    Config cfg = document.config;
    cfg.text_block.background_color = Color::FromHex("#112233");
    cfg.text_block.frame_color = Color::FromHex("#445566");
    document.WaitTask(document.SetConfig(cfg, false));
    ASSERT_TRUE(document.config.text_block == cfg.text_block);
    ASSERT_TRUE(!(document.config.text_block == config.text_block));

    //the colors are application settings, they are not serialized into the document config
    std::string json;
    cfg.ToJson(json);
    ASSERT_TRUE(json.find("text_block") == std::string::npos) << json;
    Config loaded;
    ASSERT_TRUE(loaded.FromJson(json));
    ASSERT_TRUE(loaded.text_block == config.text_block);
}

//Typing "=" converts the preceding elements into the left part of the text equation
TEST_F(TextBlockTest, text_block9)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("2", true);
    document.InsertString("x", true);
    document.InsertEquation(yutovo_solver::ResultType::AUTO, true);
    document.WaitTask(document.InsertString("9", true));
    std::this_thread::sleep_for(500ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>9</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2x</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>9</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();
}


//A text block inserted into the document text - the right part of a text equation is a formula, not plain text
TEST_F(TextBlockTest, text_block10)
{
    Start(600);

    document.InsertString("abc ", true);
    document.InsertTextBlock(true);
    document.InsertString("123", true);
    document.InsertEquation(yutovo_solver::ResultType::AUTO, true);
    document.InsertString("4", true);
    document.InsertString("5", true);
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.WaitTask(document.InsertString("7", true));
    std::this_thread::sleep_for(100ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">abc </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>45</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>67</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2, 0, 2, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">abc </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>45</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">abc </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>45</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>67</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2, 0, 2, 0, 2})) << document.GetEditorState().ToString();

    //check the html - the block content is a formula (MathML), not plain text
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">abc </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi>45</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>67</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2, 0, 2, 0, 2})) << document.GetEditorState().ToString();
}

//Saving a document with a text block to a file - the yut file keeps the block structure and colors
TEST_F(TextBlockTest, text_block11)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("1", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.InsertEquation(yutovo_solver::ResultType::AUTO, true);
    document.WaitTask(document.InsertString("3", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1</mi>"\
                            "<mo>+</mo>"\
                            "<mi>2</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>3</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Save("text_block11.yut"));
    std::this_thread::sleep_for(200ms);

    //the file exists and is a gzip archive with the document json
    std::ifstream file("text_block11.yut", std::ios_base::in | std::ios_base::binary);
    ASSERT_TRUE(file.good());
    char magic[2] = {0, 0};
    file.read(magic, 2);
    ASSERT_TRUE(magic[0] == '\x1f' && magic[1] == '\x8b'); //gzip
    file.seekg(0);

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(file);
    std::istream stream(&in);
    std::string json((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    ASSERT_TRUE(json.size() > 0);

    //the saved document keeps the text block with a text equation and its code strings
    auto has_type =
        [&json](ElementType type) -> bool
        {
            return json.find("\"type\": " + std::to_string((int)type)) != std::string::npos;
        };

    ASSERT_TRUE(has_type(ElementType::TEXT_BLOCK)) << json;
    ASSERT_TRUE(has_type(ElementType::TEXT_EQUATION)) << json;
    ASSERT_TRUE(has_type(ElementType::CODE_STRING)) << json;
    ASSERT_TRUE(!has_type(ElementType::EQUATION)) << json;
    ASSERT_TRUE(!has_type(ElementType::AUTO_RESULT)) << json;

    //the text block colors are application settings - they are not stored in the document
    ASSERT_TRUE(json.find("\"text_block\"") == std::string::npos) << json;
}

//A code block cannot be inserted inside a text block
TEST_F(TextBlockTest, text_block12)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//A text block cannot be inserted inside a text block
TEST_F(TextBlockTest, text_block13)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertTextBlock(true));
    std::this_thread::sleep_for(200ms);

    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>x</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//A document saved by an older version may keep plain strings inside a text block - they become code strings on load
TEST_F(TextBlockTest, text_block14)
{
    Start(600);

    auto json = "{\"string_formats\":[{\"id\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\",\"family\":\"Arial\",\"size\":14,\"bold\":false,\"italic\":f" \
        "alse,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"89c69148-d1bc-4384-b009-7b752eab898c\",\"f" \
        "amily\":\"Courier New\",\"size\":14,\"bold\":false,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4" \
        "294967295}],\"paragraph_formats\":[{\"name\":\"Text body\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10" \
        ",\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\"e9fe76c1-fdcb-" \
        "41b4-a64c-b5d5e84eff91\"},{\"name\":\"Code\",\"alignment\":0,\"word_wrap\":0,\"line_spacing\":2,\"indent_before\":2,\"indent_after" \
        "\":2,\"indent_first_line\":0,\"spacing_before\":2,\"spacing_after\":2,\"default_string_format\":\"89c69148-d1bc-4384-b009-7b752eab" \
        "898c\"}],\"text\":{\"id\":\"0\",\"type\":1,\"elements\":[{\"id\":\"0,0\",\"type\":2,\"format_name\":\"Text body\",\"elements\":[{" \
        "\"id\":\"0,0,0\",\"type\":3,\"elements\":[{\"id\":\"0,0,0,0\",\"type\":55,\"elements\":[{\"id\":\"0,0,0,0,0\",\"type\":6,\"format_" \
        "name\":\"Code\",\"elements\":[{\"id\":\"0,0,0,0,0,0\",\"type\":7,\"elements\":[{\"id\":\"0,0,0,0,0,0,0\",\"type\":56,\"elements\":" \
        "[{\"id\":\"0,0,0,0,0,0,0,0\",\"type\":7,\"elements\":[{\"id\":\"0,0,0,0,0,0,0,0,0\",\"type\":8,\"elements\":\"23423\",\"format_id" \
        "\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}]},{\"id\":\"0,0,0,0,0,0,0,1\",\"type\":10,\"elements\":[]},{\"id\":\"0,0,0,0,0,0,0,2" \
        "\",\"type\":7,\"elements\":[{\"id\":\"0,0,0,0,0,0,0,2,0\",\"type\":4,\"elements\":\"345234\",\"format_id\":\"e9fe76c1-fdcb-41b4-a6" \
        "4c-b5d5e84eff91\"}]}]}]}]}]}]}]}]}}";

    document.WaitTask(document.LoadJson(json, 0));
    std::this_thread::sleep_for(300ms);

    //the plain string in the right part is converted into a code string
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>23423</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>345234</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Copy a code block with assignments and equations and paste it into a text block - nothing solves there
TEST_F(TextBlockTest, text_block15)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly(
        [&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });
    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly(
        [&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    //a code block with an assignment and a solved equation
    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertAssignment(true);
    document.InsertString("5", true);
    document.InsertParagraph(true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertEquation(yutovo_solver::ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2000ms);
    ASSERT_TRUE(document.ToText() == U"x=5\n2+2=4.") << ToBasicString(document.ToText());

    //copy the whole document and paste it into a text block
    document.SelectAll();
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertTextBlock(true);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(500ms);

    //the pasted assignment becomes a text assignment (the : sign) and the equation keeps its solved right part as plain text
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.ToText() == U"x=5\n2+2=4.x=5\n2+2=4.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
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
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>4.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
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
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>4.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
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
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>4.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() ==
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
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
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>4.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"\
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
                    "<br>"\
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                                "<mo>+</mo>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mi>4.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"\
            "</p>"\
        "</body>"
        ) << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a read-only element inside a text block
TEST_F(TextBlockTest, text_block16)
{
    Start(600);

    document.InsertTextBlock(true);
    document.InsertSum(true);
    document.WaitTask(document.InsertString("x", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munderover>"\
                            "<mo>Σ</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munderover>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<munderover>"\
                            "<mo>Σ</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>x</mi>"\
                                "</mrow>"\
                                "<mo>=</mo>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</munderover>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

}

