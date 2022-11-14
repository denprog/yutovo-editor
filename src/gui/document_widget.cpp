#include "document_widget.h"
#include <QResizeEvent>
#include <QPainter>

//DocumentWidget

DocumentWidget::DocumentWidget(QWidget *parent) :
    QWidget(parent),
    window(this),
    document(&window)
{
    connect(&window, &QtWindow::DocumentUpdated, this, &DocumentWidget::OnDocumentUpdated);
    connect(&window, &QtWindow::WindowUpdated, this, &DocumentWidget::OnWindowUpdated);

    setFocusPolicy(Qt::StrongFocus);
}

void DocumentWidget::InsertText(const std::string& str, const StringFormatPtr string_format)
{
    document.InsertText(str, string_format, true);
}

void DocumentWidget::OnDocumentUpdated(const Rect rect)
{
    update(rect.left, rect.top, rect.width, rect.height);
}

void DocumentWidget::OnWindowUpdated()
{
    update(rect());
    document.Redraw();
}

void DocumentWidget::paintEvent(QPaintEvent *event)
{
    const QRect& rect = event->rect();
    QPixmap pixmap;
    window.GetPixmap(pixmap, rect);

    QPainter p(this);
    p.drawPixmap(QPoint(rect.left(), rect.top()), pixmap);
}

void DocumentWidget::resizeEvent(QResizeEvent *event)
{
    document.Resize(event->size().width(), event->size().height());
}

void DocumentWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        if (event->modifiers() & Qt::ControlModifier)
            document.MoveCaretWordLeft(event->modifiers() & Qt::ShiftModifier);
        else
            document.MoveCaretLeft(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Right:
        if (event->modifiers() & Qt::ControlModifier)
            document.MoveCaretWordRight(event->modifiers() & Qt::ShiftModifier);
        else
            document.MoveCaretRight(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Up:
        document.MoveCaretUp(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Down:
        document.MoveCaretDown(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Home:
        document.MoveCaretHome(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_End:
        document.MoveCaretEnd(event->modifiers() & Qt::ShiftModifier);
        break;
    default:
        QString str = event->text();
        if (!str.isEmpty())
            document.InsertText(str.toUtf8().data(), true);
        break;
    }
}

void DocumentWidget::mousePressEvent(QMouseEvent *event)
{
    
}
