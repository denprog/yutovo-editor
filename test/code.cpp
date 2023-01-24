#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

struct CodeTest : DocumentTest
{
};

TEST_F(CodeTest, code1)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertString("m", true);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.InsertString("Text", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("Normal", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2, 6})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code2)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    document.InsertString("Text", true);
    document.InsertCode(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertParagraph(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code3)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code4)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertCode(true);
    document.WaitMainLoop();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertCode(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code5)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertCode(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.DeleteElements(false, true, false);
    document.WaitMainLoop();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code6)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
    document.MoveCaretLeft(false);
    document.WaitCaretMoving();
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, ElementSelectionState{{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, ElementSelectionState{{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertCode(true);
    document.WaitMainLoop();
    document.MoveCaretLeft(false);
    document.MoveCaretHome(false);
    document.WaitCaretMoving();
    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, ElementSelectionState{{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.WaitCaretMoving();
    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, ElementSelectionState{{0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, ElementSelectionState{{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitCaretMoving();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, ElementSelectionState{{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code7)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 327, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.WaitCaretMoving();
    document.InsertCode(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sourc</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">e of the </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretToDocumentEnd(false);
    document.WaitCaretMoving();
    document.InsertCode(true);
    document.WaitMainLoop();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sourc</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">e of the </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sourc</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">e of the </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sourc</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">e of the </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code8)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitMainLoop();
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.InsertString("Text", true);
    document.InsertCode(true);
    document.MoveCaretRight(false);
    document.WaitCaretMoving();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 4})) << document.GetEditorState().ToString();
}

//Multiline code
TEST_F(CodeTest, code9)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertCode(true);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>Null</mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Multiline code
TEST_F(CodeTest, code10)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });
    
    document.InsertString("Text", true);
    document.InsertCode(true);
    document.InsertString("1234", true);
    document.WaitMainLoop();
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1234</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>Null</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>4</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>4</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

}
