#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <QMainWindow>
#include <QApplication>
#include <gmock/gmock.h>
#include "editor/document.h"
#include "editor/util.h"
#include "editor/window.h"

namespace yutovo_test
{

using namespace yutovo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) :
        QMainWindow(parent)
    {
        qRegisterMetaType<Rect>("Rect");
    }
};

class WindowMock : public Window
{
public:
    MOCK_METHOD(void, DrawText, (const std::string& text, const StringFormatPtr format, const Rect& rect), (override));
    MOCK_METHOD(void, DrawLine, (const int x1, const int y1, const int x2, const int y2), (override));
    MOCK_METHOD(void, DrawRect, (const int x1, const int y1, const int width, const int height, const Color color), (override));
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
        app(argc, argv),
        document(&window_mock)
    {
    }

    Size GetTextSizeMock(const std::string& text, const StringFormatPtr format)
    {
        QFont font(format->family.c_str(), format->size);
        font.setBold(format->bold);
        font.setItalic(format->italic);
        font.setUnderline(format->underline);
        QFontMetrics m(font);
        QString str(text.c_str());
        QSize s = m.size(Qt::TextSingleLine, str);
        int cx = m.horizontalAdvance(str);
        return Size{cx > s.width() ? cx : s.width(), s.height()};
    }

    CaretState MakeCaretState(uint paragraph_id, uint row_id, uint string_id, uint string_pos)
    {
        CaretState r;
        ElementId id{0, 0};
        id.push_back(paragraph_id);
        id.push_back(row_id);
        id.push_back(string_id);
        id.push_back(string_pos);
        r.id = id;
        return r;
    }

    CaretState MakeCaretState(uint paragraph_id, uint row_id, uint string_id, uint string_pos, uint selection_start, uint selection_size)
    {
        CaretState r;
        ElementId id{0, 0};
        id.push_back(paragraph_id);
        id.push_back(row_id);
        id.push_back(string_id);
        r.selections.AddSelection(id, selection_start, selection_size);
        id.push_back(string_pos);
        r.id = id;
        return r;
    }

    CaretState MakeCaretState(uint paragraph_id, uint row_id, uint string_id, uint string_pos, uint selection1_start, uint selection1_size,
        uint string2_id, uint selection2_start, uint selection2_size)
    {
        CaretState r;
        ElementId id{0, 0};
        id.push_back(paragraph_id);
        id.push_back(row_id);
        id.push_back(string_id);
        r.selections.AddSelection(id, selection1_start, selection1_size);

        ElementId id2{0, 0};
        id2.push_back(paragraph_id);
        id2.push_back(row_id);
        id2.push_back(string2_id);
        r.selections.AddSelection(id2, selection2_start, selection2_size);

        id.push_back(string_pos);
        r.id = id;
        return r;
    }

    int argc = 0;
    char** argv = nullptr;
    QApplication app;
    MainWindow main_window;
    ::testing::NiceMock<WindowMock> window_mock;
    Document document;
};

}

#endif
