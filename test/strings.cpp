#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "editor/window.h"
#include "editor/document.h"

using namespace yutovo;

class WindowMock : public Window
{
public:
    MOCK_METHOD(void, DrawText, (const std::string& text, const StringFormatPtr format, const Rect& rect), (override));
    MOCK_METHOD(void, DrawLine, (const int x1, const int y1, const int x2, const int y2), (override));
    //MOCK_METHOD(void, DrawRect, (const Rect& rect), (override));
    MOCK_METHOD(void, DrawRect, (const int x1, const int y1, const int width, const int height, const Color color), (override));
    //MOCK_METHOD(void, DrawFillRect, (const Rect& rect), (override));
    MOCK_METHOD(void, DrawFillRect, (const int x1, const int y1, const int width, const int height, const Color color), (override));
    MOCK_METHOD(int, GetFontAscent, (const StringFormatPtr), (override));
    MOCK_METHOD(void, ClearSurface, (), (override));

    MOCK_METHOD(void, StoreRect, (const Rect& rect), (override));
    MOCK_METHOD(void, RestoreRect, (), (override));

    MOCK_METHOD(Size, GetTextSize, (const std::string& text, const StringFormatPtr format), (override));

    MOCK_METHOD(void, Update, (const Rect& rect), (override));

    MOCK_METHOD(void, SetViewPort, (const Rect), (override));
    MOCK_METHOD(void, AddViewPort, (const Rect), (override));
    MOCK_METHOD(Rect, GetViewPort, (const int), (override));

    MOCK_METHOD(void, Resize, (uint width, uint height), (override));

    MOCK_METHOD(Rect, GetRect, (), (override));
};

struct StringsTest : public testing::Test
{
    StringsTest() :
        document(&window_mock)
    {
    }

    void SetUp()
    {
    }

    void TearDown()
    {
    }

    Document document;
    ::testing::NiceMock<WindowMock> window_mock;
};

using namespace std::chrono_literals;

TEST(StringsTest, simple_strings)
{
    WindowMock window_mock;

    EXPECT_CALL(window_mock, GetRect).WillOnce([&]()
        {
            return Rect{0, 0, 600, 400};
        });

    EXPECT_CALL(window_mock, DrawFillRect).WillRepeatedly([&](const int x1, const int y1, const int width, const int height, const Color color)
        {
        });

    EXPECT_CALL(window_mock, DrawRect).WillRepeatedly([&](const int x1, const int y1, const int width, const int height, const Color color)
        {
        });

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::string& text, const StringFormatPtr format)
        {
            return Size{40, 20};
        });

    EXPECT_CALL(window_mock, Update).WillRepeatedly([&](const Rect& rect)
        {
        });

    Document document(&window_mock);

    ASSERT_TRUE(document.ToHtml() == "<body><p></p></body>");

    document.InsertText("T", true);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p>T</p></body>");

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p></p></body>");

    document.Redo();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(document.ToHtml() == "<body><p>T</p></body>");

    document.InsertText("e", true);
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p>Te</p></body>");
}
