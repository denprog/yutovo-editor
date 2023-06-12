#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

TEST_F(SolverIntegerTest, solver1)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.InsertPlus(true);
    document.InsertString("35", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                            "<mo>+</mo>"\
                            "<mi>35</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2380</mi>"\
                            "</mrow>"\
                        "</mrow>"\
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
                        "<mi>2345</mi>"\
                        "<mo>+</mo>"\
                        "<mi>35</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();
}

TEST_F(SolverIntegerTest, solver2)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("23", true);
    document.InsertMultiply(true);
    document.InsertString("355", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mo>-</mo>"\
                            "<mi>23</mi>"\
                            "<mo>×</mo>"\
                            "<mi>355</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mo>-</mo>"\
                                "<mi>8165</mi>"\
                            "</mrow>"\
                        "</mrow>"\
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
                        "<mo>-</mo>"\
                        "<mi>23</mi>"\
                        "<mo>×</mo>"\
                        "<mi>355</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 3, 3})) << document.GetEditorState().ToString();
}

//Present Auto result as Integer result
TEST_F(SolverIntegerTest, solver3)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2345.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetResult({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>2345</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>2345</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345."
        ) << ToBasicString(document.ToText());
}

//Check result notation
TEST_F(SolverIntegerTest, solver4)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::BINARY;
    config.integer_result.show_notation = true;
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.result_notation = Notation::OCTAL;
    config.integer_result.show_notation = false;
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)\n"
        U"2345=4451"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.result_notation = Notation::DECIMAL;
    config.integer_result.show_notation = true;
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)\n"
        U"2345=4451\n"
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.integer_result.result_notation = Notation::HEXADECIMAL;
    document.SetConfig(config);

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)\n"
        U"2345=4451\n"
        U"2345=2345(dec)\n"
        U"2345=929(hex)"
        ) << ToBasicString(document.ToText());
}

//Set result notation
TEST_F(SolverIntegerTest, solver5)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::DECIMAL;
    config.integer_result.show_notation = true;
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 2}, Notation::BINARY, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"
        ) << ToBasicString(document.ToText());

    document.GetConfig(config);
    config.solve_delay = 0;
    document.SetConfig(config);

    document.Redo();
    document.WaitRedo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=100100101001(bin)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, Notation::OCTAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=4451(oct)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, Notation::HEXADECIMAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=929(hex)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"2345=4451(oct)"
        ) << ToBasicString(document.ToText());
}

//Set result notation
TEST_F(SolverIntegerTest, solver6)
{
    Start(600);
    
    Config config;
    document.GetConfig(config);
    config.integer_result.result_notation = Notation::DECIMAL;
    config.integer_result.show_notation = true;
    document.SetConfig(config);

    document.InsertCode(false, true);
    document.InsertString("567", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=567(dec)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetNotation({0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3}, Notation::HEXADECIMAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=237(hex)"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.DeleteElements(false, true, false));
    ASSERT_TRUE(document.ToText() == 
        U"567"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=237(hex)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"567=567(dec)"
        ) << ToBasicString(document.ToText());
}

}
