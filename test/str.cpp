#include <gtest/gtest.h>
#include "str.h"
#include <QPainter>

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(StringsTest, simple_strings)
{
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

    document.InsertText("T", true);
    std::this_thread::sleep_for(100ms);
    CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>");

    document.Undo();
    std::this_thread::sleep_for(100ms);
    CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

    document.Redo();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>");

    document.InsertText("e", true);
    std::this_thread::sleep_for(100ms);
    CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Te</span></p></body>");
    document.InsertText("x", true);
    std::this_thread::sleep_for(100ms);
    document.InsertText("t", true);
    document.InsertText("Text", true);
    std::this_thread::sleep_for(100ms);
    CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>");
}

// TEST_F(StringsTest, delete_strings)
// {
//     EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
//         {
//             return Rect{0, 0, 600, 400};
//         });

//     EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
//         {
//             return GetTextSizeMock(text, format);
//         });

//     CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

//     // document.DeleteElements(true, true);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

//     // document.Undo();
//     // std::this_thread::sleep_for(100ms);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

//     // document.DeleteElements(false, true);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

//     // document.Undo();
//     // std::this_thread::sleep_for(100ms);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\"></span></p></body>");

//     // document.InsertText("Text", true);
//     // std::this_thread::sleep_for(100ms);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>");

//     // document.DeleteElements(true, true);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>");

//     // document.Undo();
//     // std::this_thread::sleep_for(100ms);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>");

//     // document.Undo();
//     // std::this_thread::sleep_for(100ms);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>");

//     // document.Redo();
//     // std::this_thread::sleep_for(100ms);
//     // CheckString(document.ToHtml(), "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>");
// }

}
