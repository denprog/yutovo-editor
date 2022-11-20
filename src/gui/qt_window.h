#ifndef __QT_WINDOW_H__
#define __QT_WINDOW_H__

#include <QImage>
#include <QPixmap>
#include <memory>
#include <mutex>
#include "editor/window.h"

class DocumentWidget;

using namespace yutovo;

//Qt implementation of the abstract window
class QtWindow : public QObject, public Window
{
    Q_OBJECT

public:
    QtWindow(DocumentWidget* document_widget);

    virtual void DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect);
    virtual void DrawLine(const int x1, const int y1, const int x2, const int y2);
    virtual void DrawRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void ClearSurface();

    virtual void StoreRect(const Rect& rect);
    virtual void RestoreRect();

    virtual Size GetTextSize(const std::string& text, const StringFormatPtr format);
    virtual int GetFontAscent(const StringFormatPtr format);

    virtual void Update(const Rect& rect);

    virtual void SetViewPort(const Rect view_port);
    virtual void AddViewPort(const Rect view_port);
    virtual Rect GetViewPort(const int pos);

    virtual void Resize(uint width, uint height);

    virtual void MoveDocument(const int left, const int top);

    virtual void OnCaretMoved(const CaretState& caret_state);

    virtual Rect GetRect();

public:
    void GetPixmap(QPixmap& out, const QRect& rect);

signals:
    void DocumentUpdated(const Rect rect);
    void WindowUpdated();
    void CaretMoved(const CaretState& caret_state);

private:
    std::unique_ptr<QImage> surface;
    QPixmap pixmap;
    std::mutex pixmap_mutex;
    Rect store_rect;
    QImage store_image;

    QRegion clip_region;
};

#endif
