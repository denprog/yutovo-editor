#include "qt_window.h"
#include <QPainter>
#include "document_widget.h"

//QtWindow

QtWindow::QtWindow(DocumentWidget* document_widget) :
    surface(new QImage(document_widget->size().width(), document_widget->size().height(), QImage::Format_RGB32))
{
}

void QtWindow::DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    
    QFont font(format->family.c_str(), format->size);
    font.setItalic(format->italic);
    font.setBold(format->bold);
    font.setUnderline(format->underline);
    p.setFont(font);
    if (draw_doc)
        p.setClipRegion(clip_region);
    bool b = p.hasClipping();
    p.drawText(QRect(rect.left - document_point.x, rect.top, rect.width, rect.height), text.c_str());
    p.end();
}

void QtWindow::DrawLine(const int x1, const int y1, const int x2, const int y2)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setPen(QPen(QBrush(Qt::SolidPattern), 1));
    if (draw_doc)
        p.setClipRegion(clip_region);
    p.drawLine(x1 - document_point.x, y1, x2 - document_point.x, y2);
    p.end();
}

void QtWindow::DrawRect(const int x1, const int y1, const int width, const int height, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setPen(QColor::fromRgba(color.ToInt()));
    if (draw_doc)
    {
        p.setClipRegion(clip_region);
        p.drawRect(x1 - document_point.x, y1, width, height);
    }
    else
        p.drawRect(x1, y1, width, height);
    p.end();
}

void QtWindow::DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    if (draw_doc)
    {
        p.setClipRegion(clip_region);
        p.fillRect(x1 - document_point.x, y1, width, height, QColor::fromRgba(color.ToInt()));
    }
    else
        p.fillRect(x1, y1, width, height, QColor::fromRgba(color.ToInt()));
    p.end();
}

void QtWindow::ClearSurface()
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    QRect rect = surface->rect();
    p.fillRect(rect, QColor::fromRgb(255, 255, 255));
    p.end();
}

void QtWindow::StoreRect(const Rect& rect)
{
    Rect r(rect);
    r.left -= document_point.x;
    store_image = surface->copy(r.left, r.top, r.width, r.height);
    store_rect = r;
}

void QtWindow::RestoreRect()
{
    if (store_rect.IsEmpty())
        return;
    
    QPainter p(surface.get());
    p.drawImage(QPoint(store_rect.left, store_rect.top), store_image);
    p.end();

    emit DocumentUpdated(store_rect);
}

Size QtWindow::GetTextSize(const std::string& text, const StringFormatPtr format)
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

int QtWindow::GetFontAscent(const StringFormatPtr format)
{
    QFont font(format->family.c_str(), format->size);
    font.setBold(format->bold);
    font.setItalic(format->italic);
    font.setUnderline(format->underline);
    QFontMetrics m(font);
    return m.ascent();
}

void QtWindow::Update(const Rect& rect)
{
    {
        std::lock_guard<std::mutex> lock(pixmap_mutex);
        pixmap.convertFromImage(*surface);
    }

    //emit DocumentUpdated(rect);
    Rect r(rect);
    r.left -= document_point.x;
    emit DocumentUpdated(r);
}

void QtWindow::SetViewPort(const Rect view_port)
{
    clip_region = QRegion(QRect(view_port.left, view_port.top, view_port.width, view_port.height));
}

void QtWindow::AddViewPort(const Rect view_port)
{
}

Rect QtWindow::GetViewPort(const int pos)
{
    QRegion::const_iterator iter;
    int p = 0;
    for (iter = clip_region.begin(); p < pos && iter != clip_region.end(); ++p && ++iter);
    const QRect& rect = *iter;
    return Rect{rect.left(), rect.top(), rect.width(), rect.height()};
}

void QtWindow::Resize(uint width, uint height)
{
    surface.reset(new QImage(width, height, QImage::Format_RGB32));
    ClearSurface();
}

void QtWindow::MoveDocument(const int left, const int top)
{
    Window::MoveDocument(left, top);
    ClearSurface();

    // {
    //     std::lock_guard<std::mutex> lock(pixmap_mutex);
    //     pixmap.convertFromImage(*surface);
    // }

    // QRect rect = surface->rect();
    // Rect r(rect.left(), rect.top(), rect.width(), rect.height());
    // emit DocumentUpdated(r);

    //emit WindowUpdated();
    //store_rect.Reset();
}

Rect QtWindow::GetRect()
{
    QRect rect = surface->rect();
    return Rect{rect.left(), rect.top(), rect.width(), rect.height()};
}

void QtWindow::GetPixmap(QPixmap& out, const QRect& rect)
{
    std::lock_guard<std::mutex> lock(pixmap_mutex);
    out = pixmap.copy(rect);
}
