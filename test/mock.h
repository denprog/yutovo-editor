/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __MOCK_H__
#define __MOCK_H__

#include <QMainWindow>
#include <QApplication>
#include <QFontDatabase>
#include <gmock/gmock.h>
#include "document.h"
#include "editor_utils.h"
#include "window.h"
#include "pdf_window.h"

namespace yutovo_test
{

typedef unsigned int uint;

using namespace yutovo;
using yutovo::ToBasicString;
using ElementId = yutovo::ElementId;

extern int argc;
extern char** argv;

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

typedef std::tuple<char32_t, std::string, int> SymbolsSizes;

class WindowMock : public Window
{
public:
    MOCK_METHOD(void, Init, (Document* document), (override));
    
    MOCK_METHOD(void, DrawText, (const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color, const Color bg_color), (override));
    MOCK_METHOD(void, DrawLine, (const int x1, const int y1, const int x2, const int y2, const Color color), (override));
    MOCK_METHOD(void, DrawRect, (const int x1, const int y1, const int width, const int height, const Color color), (override));
    MOCK_METHOD(void, DrawFillRect, (const int x1, const int y1, const int width, const int height, const Color color), (override));
    MOCK_METHOD(void, DrawFillEllipse, (const int x1, const int y1, const int width, const int height, const Color color), (override));
    MOCK_METHOD(void, DrawFillPath, (const std::list<Point>& path, const Color color), (override));
    MOCK_METHOD(void, DrawBezierPath, (const std::list<Point>& path, const Color color), (override));
    MOCK_METHOD(void, DrawWavyLine, (const int x1, const int y1, const int width, const int radius, const Color color), (override));
    MOCK_METHOD(void, DrawImage, (const int x1, const int y1, const int width, const int height, const std::vector<unsigned char>& image), (override));
    MOCK_METHOD(int, GetSymbolSize, (const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline), (override));
    MOCK_METHOD(void, PrepareSymbolsSizes, (const std::vector<SymbolsSizes>& symbols_sizes), (override));

    MOCK_METHOD(void, ClearRect, (const int x1, const int y1, const int width, const int height, const Color color), (override));
    MOCK_METHOD(int, GetFontAscent, (const StringFormatPtr), (override));
    MOCK_METHOD(void, ClearSurface, (), (override));

    MOCK_METHOD(void, StoreRect, (const Rect& rect), (override));
    MOCK_METHOD(void, RestoreRect, (), (override));

    MOCK_METHOD(Size, GetTextSize, (const std::u32string& text, const StringFormatPtr format), (override));
    MOCK_METHOD(int, GetCharPos, (const std::u32string& text, const StringFormatPtr format, int pos), (override));
    MOCK_METHOD(Size, GetImageSize, (const std::vector<unsigned char>& image), (override));

    MOCK_METHOD(void, Update, (const Rect& rect), (override));

    MOCK_METHOD(void, SetViewPort, (const Rect), (override));
    MOCK_METHOD(void, AddViewPort, (const Rect), (override));
    MOCK_METHOD(Rect, GetViewPort, (const int), (override));

    MOCK_METHOD(void, Resize, (uint width, uint height), (override));

    MOCK_METHOD(Rect, GetRect, (), (override));

    MOCK_METHOD(std::u32string, Translate, (ElementId id, const std::u32string& str));

    MOCK_METHOD(void, OnSaveResult, (const uint task_id, IOResult result, const int document_id), (override));
    MOCK_METHOD(void, OnLoadResult, (const uint task_id, IOResult result, const int document_id), (override));
    MOCK_METHOD(void, OnLoadInclude, (const std::string& file_name, const int document_id), (override));

    MOCK_METHOD(void, OnCopyResult, (CopyResult result), (override));
    MOCK_METHOD(void, OnPasteResult, (PasteResult result), (override));

    MOCK_METHOD(void, OnLinkClicked, (const ElementId& id, const std::u32string& url), (override));
};

class PdfWindowMock : public PdfWindow
{
public:
    PdfWindowMock(const Size& _page_size, bool draw_footer) :
        PdfWindow(_page_size, draw_footer)
    {
    }

    MOCK_METHOD(void, OnPdfExportResult, (const std::vector<uint8_t>& pdf, const PdfResult result), (override));
    MOCK_METHOD(bool, GetFontPath, (const StringFormatPtr format, std::string& path), (override));
};

struct DocumentTest : public testing::Test
{
    DocumentTest() :
        app(argc, argv),
        document(&window_mock, config)
    {
    }

    void Start(int width)
    {
        EXPECT_CALL(window_mock, GetRect).WillRepeatedly(
            [width]()
            {
                return Rect{0, 0, width, 400};
            });
            
        EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
            [&](const std::u32string& text, const StringFormatPtr format)
            {
                return GetTextSizeMock(text, format);
            });
        
        document.config.solve_delay = 0;
        document.config.pretty_json = true;
        document.Start();
    }

    void Start(int width, int height)
    {
        EXPECT_CALL(window_mock, GetRect).WillRepeatedly(
            [width, height]()
            {
                return Rect{0, 0, width, height};
            });
            
        EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
            [&](const std::u32string& text, const StringFormatPtr format)
            {
                return GetTextSizeMock(text, format);
            });
        
        document.config.solve_delay = 0;
        document.config.pretty_json = true;
        document.Start();
    }

    Size GetTextSizeMock(const std::u32string& text, const StringFormatPtr format);

    Size GetImageSizeMock(const std::vector<unsigned char>& image);

    EditorState MakeEditorState(uint paragraph_id, uint row_id, uint string_id, uint string_pos)
    {
        CaretState r;
        ElementId id{0};
        id.push_back(paragraph_id);
        id.push_back(row_id);
        id.push_back(string_id);
        id.push_back(string_pos);
        r.id = id;
        EditorState s;
        s.caret_state = r;
        return s;
    }

    EditorState MakeEditorState(uint paragraph_id, uint row_id, uint string_id, uint string_pos, uint selection_start, uint selection_size)
    {
        CaretState r;
        ElementId id{0};
        id.push_back(paragraph_id);
        id.push_back(row_id);
        id.push_back(string_id);

        SelectionState s;
        s.Add(id, selection_start, selection_size);
        id.push_back(string_pos);
        r.id = id;
        return EditorState{r, s};
    }

    EditorState MakeEditorState(uint paragraph_id, uint row_id, uint string_id, uint string_pos, uint selection1_start, uint selection1_size,
        uint string2_id, uint selection2_start, uint selection2_size)
    {
        EditorState res = MakeEditorState(paragraph_id, row_id, string_id, string_pos, selection1_start, selection1_size);

        ElementId id2{0};
        id2.push_back(paragraph_id);
        id2.push_back(row_id);
        id2.push_back(string2_id);
        res.selection_state.Add(id2, selection2_start, selection2_size);
        return res;
    }

    EditorState MakeEditorState(uint paragraph_id, uint row_id, uint string_id, uint string_pos, uint selection1_start, uint selection1_size,
        uint string2_id, uint selection2_start, uint selection2_size, uint string3_id, uint selection3_start, uint selection3_size)
    {
        EditorState res = MakeEditorState(paragraph_id, row_id, string_id, string_pos, selection1_start, selection1_size,
            string2_id, selection2_start, selection2_size);

        ElementId id3{0};
        id3.push_back(paragraph_id);
        id3.push_back(row_id);
        id3.push_back(string3_id);
        res.selection_state.Add(id3, selection3_start, selection3_size);
        return res;
    }

    EditorState MakeEditorState(ElementId id, ElementSelectionState selection)
    {
        CaretState c(id);
        SelectionState s;
        s.Add(selection);
        return EditorState{c, s};
    }

    EditorState MakeEditorState(ElementId id, ElementSelectionState selection1, ElementSelectionState selection2)
    {
        CaretState c(id);
        SelectionState s;
        s.Add(selection1);
        s.Add(selection2);
        return EditorState{c, s};
    }

    EditorState MakeEditorState(ElementId id, ElementSelectionState selection1, ElementSelectionState selection2, ElementSelectionState selection3)
    {
        CaretState c(id);
        SelectionState s;
        s.Add(selection1);
        s.Add(selection2);
        s.Add(selection3);
        return EditorState{c, s};
    }

    EditorState MakeEditorState(ElementId id, ElementSelectionState selection1, ElementSelectionState selection2, ElementSelectionState selection3, 
        ElementSelectionState selection4)
    {
        CaretState c(id);
        SelectionState s;
        s.Add(selection1);
        s.Add(selection2);
        s.Add(selection3);
        s.Add(selection4);
        return EditorState{c, s};
    }

    EditorState MakeEditorState(ElementId id, ElementSelectionState selection1, ElementSelectionState selection2, ElementSelectionState selection3, 
        ElementSelectionState selection4, ElementSelectionState selection5)
    {
        CaretState c(id);
        SelectionState s;
        s.Add(selection1);
        s.Add(selection2);
        s.Add(selection3);
        s.Add(selection4);
        s.Add(selection5);
        return EditorState{c, s};
    }

    EditorState MakeEditorState(ElementId id, ElementSelectionState selection1, ElementSelectionState selection2, ElementSelectionState selection3, 
        ElementSelectionState selection4, ElementSelectionState selection5, ElementSelectionState selection6)
    {
        CaretState c(id);
        SelectionState s;
        s.Add(selection1);
        s.Add(selection2);
        s.Add(selection3);
        s.Add(selection4);
        s.Add(selection5);
        s.Add(selection6);
        return EditorState{c, s};
    }

    EditorState MakeEditorState(ElementId id)
    {
        return EditorState{CaretState{id}, SelectionState{}};
    }

    std::string ErrorMarks()
    {
        std::string res = "[";
        std::function<void (ElementPtr, std::string&)> add_error_marks = 
            [&](ElementPtr el, std::string& str)
            {
                if (document.IsString(el))
                {
                    if (el->error_mark)
                    {
                        if (str.length() > 1)
                            str += ",";
                        str += "{{" + ElementIdToString(el->id) + "}," + std::to_string(0) + "," + std::to_string(el->elements->Count()) + "}";
                    }
                    return;
                }

                int start = 0, size = 0;
                for (int i = 0; i < el->elements->Count(); ++i)
                {
                    auto ch = el->elements->Get(i);
                    if (ch->error_mark)
                    {
                        start = i;
                        size = 1;
                        for (int j = i + 1; j < el->elements->Count(); ++j)
                        {
                            if (el->elements->Get(j)->error_mark)
                                ++size;
                        }
                        
                        if (str.length() > 1)
                            str += ",";
                        str += "{{" + ElementIdToString(el->id) + "}," + std::to_string(start) + "," + std::to_string(size) + "}";
                    }
                    add_error_marks(ch, str);
                }
            };
        add_error_marks(document.GetElement({0}), res);
        res += "]";
        return res;
    }

    std::string Base64Encode(std::vector<unsigned char>& arr);

    void GetImageData(QImage& image, std::vector<unsigned char>& data);

    QApplication app;
    MainWindow main_window;
    ::testing::NiceMock<WindowMock> window_mock;
    yutovo::Config config;
    Document document;

    std::u32string clipboard_json;
    std::u32string clipboard_text;
};

struct FormulaTest : DocumentTest
{
    FormulaTest()
    {
        EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
            {
                return Rect{0, 0, 600, 400};
            });
    }
};

struct FormulaTestCustom : DocumentTest
{
};

struct SolverTest : DocumentTest
{
};

struct SolverAutoTest : SolverTest
{
};

struct SolverRealTest : SolverTest
{
};

struct SolverIntegerTest : SolverTest
{
};

struct SolverRationalTest : SolverTest
{
};

struct SolverComplexTest : SolverTest
{
};

struct SolverArrayRealTest : SolverTest
{
};

struct AssignmentTest : SolverTest
{
};

struct UnitTest : SolverTest
{
};

struct ArrayTest : SolverTest
{
};

struct VariablesTest : SolverTest
{
    void Start(int width)
    {
        SolverTest::Start(width);

        EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
            {
                return str;
            });
    }
};

struct TwoDocumentsTest : DocumentTest
{
    TwoDocumentsTest() :
        document2(&window_mock2, config2)
    {
    }

    void Start(int width)
    {
        DocumentTest::Start(width);

        EXPECT_CALL(window_mock2, GetRect).WillRepeatedly(
            [width]()
            {
                return Rect{0, 0, width, 400};
            });
            
        EXPECT_CALL(window_mock2, GetTextSize).WillRepeatedly(
            [&](const std::u32string& text, const StringFormatPtr format)
            {
                return GetTextSizeMock(text, format);
            });
        
        document2.config.solve_delay = 0;
        document2.Start();
    }

    MainWindow main_window2;
    ::testing::NiceMock<WindowMock> window_mock2;
    yutovo::Config config2;
    Document document2;
};

struct IncludeDocumentsTest : DocumentTest
{
    IncludeDocumentsTest() : 
        document2(&window_mock2, config2)
    {
    }

    void Start(int width)
    {
        DocumentTest::Start(width);

        EXPECT_CALL(window_mock2, GetRect).WillRepeatedly(
            [width]()
            {
                return Rect{0, 0, width, 400};
            });
            
        EXPECT_CALL(window_mock2, GetTextSize).WillRepeatedly(
            [&](const std::u32string& text, const StringFormatPtr format)
            {
                return GetTextSizeMock(text, format);
            });
        
        document2.config.solve_delay = 0;
        document2.Start();
    }

    ::testing::NiceMock<WindowMock> window_mock2;
    yutovo::Config config2;
    Document document2;
};

struct PdfTest : DocumentTest
{
    void Start(int width, const Size& page_size, bool draw_footer)
    {
        pdf_window_mock.reset(new ::testing::NiceMock<PdfWindowMock>(page_size, draw_footer));

        EXPECT_CALL(*pdf_window_mock, GetFontPath).WillRepeatedly([&](const StringFormatPtr format, std::string& path)
            {
                QString p = ResolveFontPath(format);
                if (p.isEmpty())
                    return false;
                path = p.toUtf8().data();
                return true;
            });

        DocumentTest::Start(width);
    }

    QString ResolveFontPath(const StringFormatPtr format);

    std::unique_ptr<::testing::NiceMock<PdfWindowMock>> pdf_window_mock;
    QFontDatabase database;
};

}

#endif
