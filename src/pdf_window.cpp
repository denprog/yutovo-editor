#include "pdf_window.h"
#include <setjmp.h>

namespace yutovo
{

void PdfErrorHandler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
    printf("ERROR: error_no=%04X, detail_no=%d\n", (unsigned int)error_no, (int)detail_no);
}

//PdfWindow

PdfWindow::PdfWindow(const Size& _page_size) :
    page_size(_page_size)
{
    pdf = HPDF_New(PdfErrorHandler, nullptr);
    if (!pdf)
        return;

    jmp_buf env;
    if (setjmp(env))
    {
        HPDF_Free(pdf);
        return;
    }

    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, "UTF-8");
    AddPage();
}

PdfWindow::~PdfWindow()
{
    HPDF_Free(pdf);
}

void PdfWindow::Init(Document* document)
{
}

void PdfWindow::DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color, const Color bg_color)
{
    Rect r(rect);
    int b = view_port.height + y_filled;
    if (rect.top > b || rect.GetBottom() > b)
    {
        y_filled += last_y_filled;
        AddPage();
        y_diff = y_top - r.top;
    }
    r.top += y_diff - (pages - 1) * page_size.height;

    if (top_y_filled == 0)
        top_y_filled = rect.top;
    last_y_filled = rect.GetBottom() - top_y_filled;

    HPDF_Font font = GetFont(format);
    ClipDraw clip_draw(*this);
    HPDF_Page_BeginText(page);
    HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, page_size.height);
    HPDF_Page_SetFontAndSize(page, font, format->size);
    HPDF_Page_SetRGBFill(page, color.r / 255, color.g / 255, color.b / 255);
    int ascent = (int)HPDF_Font_GetAscent(font) / 1000.0 * format->size;
    HPDF_Page_TextOut(page, r.left, r.top + ascent, text.c_str());
    HPDF_Page_EndText(page);

    if (format->underline || format->strikethrough)
    {
        int descent = (int)(HPDF_Font_GetDescent(font) / 1000.0 * format->size);
        float t = format->size / 20.0;
        if (t < 0.5f)
            t = 0.5f;
        HPDF_Page_SetLineWidth(page, t);
        HPDF_Page_SetRGBStroke(page, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);

        if (format->underline)
        {
            float y = r.GetBottom() + descent * 0.6;
            HPDF_Page_MoveTo(page, r.left, y);
            HPDF_Page_LineTo(page, r.GetRight(), y);
            HPDF_Page_Stroke(page);
        }

        if (format->strikethrough)
        {
            float y = r.top + ascent - format->size * 0.25;
            HPDF_Page_MoveTo(page, r.left, y);
            HPDF_Page_LineTo(page, r.GetRight(), y);
            HPDF_Page_Stroke(page);
        }
    }
}

void PdfWindow::DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color)
{
    int b = pages * view_port.height;
    int _y1 = y1, _y2 = y2;
    if (y1 > b || y2 > b)
    {
        AddPage();
        if (_y1 < _y2)
            y_diff = y_top - y1;
        else
            y_diff = y_top - y2;
    }
    _y1 += y_diff - (pages - 1) * page_size.height;
    _y2 += y_diff - (pages - 1) * page_size.height;

    ClipDraw clip_draw(*this);
    HPDF_Page_SetRGBStroke(page, color.r / 255, color.g / 255, color.b / 255);
    HPDF_Page_SetLineWidth(page, 1);
    HPDF_Page_MoveTo(page, x1, _y1);
    HPDF_Page_LineTo(page, x2, _y2);
    HPDF_Page_Stroke(page);
}

void PdfWindow::DrawRect(const int x1, const int y1, const int width, const int height, const Color color)
{
    int b = view_port.height + y_filled;
    int _y1 = y1;
    if (y1 > b || y1 + height > b)
    {
        y_filled += last_y_filled;
        AddPage();
        y_diff = y_top - y1;
    }
    _y1 += y_diff - (pages - 1) * page_size.height;

    if (top_y_filled == 0)
        top_y_filled = y1;
    last_y_filled = y1 + height - top_y_filled;

    ClipDraw clip_draw(*this);
    HPDF_Page_SetRGBStroke(page, color.r / 255, color.g / 255, color.b / 255);
    HPDF_Page_SetLineWidth(page, 1);
    HPDF_Page_Rectangle(page, x1, _y1, width, height);
    HPDF_Page_Stroke(page);    
}

void PdfWindow::DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color)
{
}

void PdfWindow::DrawFillEllipse(const int x1, const int y1, const int width, const int height, const Color color)
{
}

void PdfWindow::DrawFillPath(const std::list<Point>& path, const Color color)
{
    if (path.empty())
        return;

    HPDF_Page_SetRGBFill(page, color.r / 255, color.g / 255, color.b / 255);
    auto p = path;
    auto it = p.begin();
    int b = pages * view_port.height;
    if (it->y > b)
    {
        AddPage();
        y_diff = y_top - it->y;
    }
    it->y += y_diff - (pages - 1) * page_size.height;

    ClipDraw clip_draw(*this);
    HPDF_Page_MoveTo(page, it->x, it->y);
    ++it;
    for (; it != p.end(); ++it)
    {
        it->y += y_diff - (pages - 1) * page_size.height;
        HPDF_Page_LineTo(page, it->x, it->y);
    }
    HPDF_Page_ClosePathFillStroke(page);    
}

void PdfWindow::DrawBezierPath(const std::list<Point>& path, const Color color)
{
}

void PdfWindow::DrawWavyLine(const int x1, const int y1, const int width, const int radius, const Color color)
{
    if (width <= 0 || radius <= 0)
        return;

    HPDF_Page_SetRGBStroke(page, color.r / 255.0, color.g / 255.0, color.b / 255.0f);
    HPDF_Page_SetLineWidth(page, 1.0f);

    int x = x1;

    ClipDraw clip_draw(*this);
    while (x < x1 + width)
    {
        HPDF_Page_Arc(page, x, y1, -radius, 90, 270);
        HPDF_Page_Stroke(page);
        x += radius * 2;
        if (x >= x1 + width)
            break;
        HPDF_Page_Arc(page, x, y1, radius, 90, 270);
        x += radius * 2;
        HPDF_Page_Stroke(page);
    }
}

void PdfWindow::DrawImage(const int x1, const int y1, const int width, const int height, const std::vector<unsigned char>& image)
{
    int _y1 = y1;
    int b = view_port.height + y_filled;
    if (y1 > b || y1 + height > b)
    {
        y_filled += last_y_filled;
        AddPage();
        y_diff = y_top - y1;
    }
    _y1 += y_diff - (pages - 1) * page_size.height;

    if (top_y_filled == 0)
        top_y_filled = y1;
    last_y_filled = y1 + height - top_y_filled;

    std::vector<unsigned char> rgba;
    int w, h;
    if (IsPng(image))
    {
        if (!PngToRgba(image, rgba, w, h))
            return;
    }
    else
    {
        rgba = image;
        w = width;
        h = height;
    }

    FlipImageVertically(rgba, w, h);
    std::vector<unsigned char> png;
    RgbaToPng(rgba, w, h, png);

    ClipDraw clip_draw(*this);
    HPDF_Image picture = HPDF_LoadPngImageFromMem(pdf, png.data(), png.size());
    HPDF_Page_DrawImage(page, picture, x1, _y1, w, h);
}

int PdfWindow::GetSymbolSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline)
{
    Size s{0, 0};
    int font_size = 1;
    auto _symbol = std::u32string(1, symbol);
    baseline = 0;
    StringFormatPtr format{new StringFormat(family_name, font_size, false, false, false, false, false, false, Color::Black(), Color::Black(), Color::Black())};
    while (s.height < height)
    {
        HPDF_Font font = GetFont(format);
        s = GetTextSize(_symbol, format);
        baseline = GetFontAscent(format);
        format->size = ++font_size;
    }
    size = s;
    return font_size - 1;
}

void PdfWindow::PrepareSymbolsSizes(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes)
{
}

void PdfWindow::ClearRect(const int x1, const int y1, const int width, const int height)
{
}

void PdfWindow::ClearSurface()
{
}

void PdfWindow::StoreRect(const Rect& rect)
{
}

void PdfWindow::RestoreRect()
{
}

Size PdfWindow::GetTextSize(const std::u32string& text, const StringFormatPtr format)
{
    HPDF_Font font = GetFont(format);
    HPDF_Page_BeginText(page);
    HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, HPDF_Page_GetHeight(page));
    HPDF_Page_SetFontAndSize(page, font, format->size);
    double w = HPDF_Page_TextWidth(page, yutovo::ToBasicString(text).c_str());
    double ascent = HPDF_Font_GetAscent(font) / 1000.0 * format->size;
    double descent = HPDF_Font_GetDescent(font) / 1000.0 * format->size;
    HPDF_Page_EndText(page);
    double h = ascent - descent;
    return Size{(int)w, (int)h};
}

int PdfWindow::GetCharPos(const std::u32string& text, const StringFormatPtr format, int pos)
{
    return 0;
}

int PdfWindow::GetFontAscent(const StringFormatPtr format)
{
    HPDF_Font font = GetFont(format);
    return (int)(HPDF_Font_GetAscent(font) / 1000.0 * format->size);
}

Size PdfWindow::GetImageSize(const std::vector<unsigned char>& image)
{
    HPDF_Image picture = HPDF_LoadPngImageFromMem(pdf, image.data(), image.size());
    float w = HPDF_Image_GetWidth(picture);
    float h = HPDF_Image_GetHeight(picture);
    return Size{(int)w, (int)h};
}

void PdfWindow::SetViewPort(const Rect _view_port)
{
    view_port = _view_port;
    y_top = 0;
}

void PdfWindow::AddViewPort(const Rect _view_port)
{
}

Rect PdfWindow::GetViewPort(const int pos)
{
    return view_port;
}

void PdfWindow::Update(const Rect& rect)
{
    //redraw has finished - pdf is ready
    HPDF_SaveToStream(pdf);
    HPDF_UINT size = HPDF_GetStreamSize(pdf);
    result.resize(size);
    HPDF_ResetStream(pdf);
    HPDF_ReadFromStream(pdf, result.data(), &size);
    OnPdfExportResult(result, PdfResult::Success);
}

void PdfWindow::Resize(uint width, uint height)
{
}

Rect PdfWindow::GetRect()
{
    return Rect{0, 0, page_size.width, page_size.height};
}

void PdfWindow::MoveDocument(const int left, const int top)
{
}

std::string PdfWindow::Translate(ElementId id, const std::string& str)
{
    return "";
}

std::u32string PdfWindow::Translate(ElementId id, const std::u32string& str)
{
    return U"";
}

int PdfWindow::ConvertToPixels(const int mm)
{
    return 0;
}

void PdfWindow::OnSaveResult(const uint task_id, IOResult result, const int document_id)
{
}

void PdfWindow::OnLoadResult(const uint task_id, IOResult result, const int document_id)
{
}

void PdfWindow::OnLoadInclude(const std::string& file_name, const int document_id)
{
}

void PdfWindow::AddPage()
{
    page = HPDF_AddPage(pdf);
    HPDF_Page_SetWidth(page, page_size.width);
    HPDF_Page_SetHeight(page, page_size.height);
    HPDF_Page_Concat(page, 1, 0, 0, -1, 0, HPDF_Page_GetHeight(page));
    ++pages;
    y_top = (pages - 1) * page_size.height + view_port.top;
    top_y_filled = 0;
}

HPDF_Font PdfWindow::GetFont(const StringFormatPtr format)
{
    auto it = fonts_map.find(format->id);
    if (it != fonts_map.end())
        return HPDF_GetFont(pdf, it->second.c_str(), "UTF-8");
    std::string path;
    if (!GetFontPath(format, path))
        return nullptr;
    auto font = HPDF_LoadTTFontFromFile(pdf, path.c_str(), HPDF_TRUE);
    if (!font)
        return nullptr;
    fonts_map[format->id] = font;
    return HPDF_GetFont(pdf, font, "UTF-8");
}

}
