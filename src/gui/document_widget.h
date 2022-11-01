#ifndef __DOCUMENT_WIDGET_H__
#define __DOCUMENT_WIDGET_H__

#include <QWidget>
#include "editor/document.h"
#include "qt_window.h"

using namespace yutovo;

class DocumentWidget : public QWidget
{
public:
    DocumentWidget(QWidget *parent);

    void InsertText(const std::string& str, const StringFormatPtr string_format);

public slots:
    void OnDocumentUpdated(const Rect rect);
    void OnWindowUpdated();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    QtWindow window;
    Document document;
};

#endif
