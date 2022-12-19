#include "document_widget.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QGuiApplication>
#include <QCursor>

//DocumentWidget

DocumentWidget::DocumentWidget(QWidget *parent) :
    QWidget(parent),
    window(this)
{
    connect(&window, &QtWindow::DocumentUpdated, this, &DocumentWidget::OnDocumentUpdated);
    connect(&window, &QtWindow::WindowUpdated, this, &DocumentWidget::OnWindowUpdated);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

DocumentPtr DocumentWidget::CreateDocument()
{
    document.reset(new Document(&window));
    return document;
}

void DocumentWidget::InsertText(const std::string& str, const StringFormatPtr string_format)
{
    document->InsertText(str, string_format, true);
}

bool DocumentWidget::GetElementAtCoords(const int x, const int y, ElementId& id)
{
    auto p = window.GetDocumentPoint();
    return document->GetElementAtCoords(x + p.x, y + p.y, id);
}

void DocumentWidget::OnDocumentUpdated(const Rect rect)
{
    update(rect.left, rect.top, rect.width, rect.height);
}

void DocumentWidget::OnWindowUpdated()
{
    update(rect());
    document->Redraw();
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
    document->Resize(event->size().width(), event->size().height());
}

void DocumentWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        if (event->modifiers() & Qt::ControlModifier)
            document->MoveCaretWordLeft(event->modifiers() & Qt::ShiftModifier);
        else
            document->MoveCaretLeft(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Right:
        if (event->modifiers() & Qt::ControlModifier)
            document->MoveCaretWordRight(event->modifiers() & Qt::ShiftModifier);
        else
            document->MoveCaretRight(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Up:
        document->MoveCaretUp(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Down:
        document->MoveCaretDown(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Home:
        document->MoveCaretHome(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_End:
        document->MoveCaretEnd(event->modifiers() & Qt::ShiftModifier);
        break;
    case Qt::Key_Backspace:
        document->DeleteElements(true, true, false);
        break;
    case Qt::Key_Delete:
        document->DeleteElements(false, true, false);
        break;
    case Qt::Key_Return:
        document->InsertParagraph(true);
        break;
    default:
        QString str = event->text();
        for (auto ch : str)
        {
            if (!ch.isPrint())
                return;
        }
        if (!str.isEmpty())
            document->InsertText(str.toUtf8().data(), true);
        break;
    }
}

void DocumentWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        document->MoveCaret((int)event->pos().x(), (int)event->pos().y());
}

void DocumentWidget::mouseMoveEvent(QMouseEvent *event)
{
    ElementId id;
    if (!GetElementAtCoords((int)event->pos().x(), (int)event->pos().y(), id))
    {
        setCursor(Qt::ArrowCursor);
        return;
    }
    if (document->GetElementType(id) == ElementType::STRING || document->GetElementType(id) == ElementType::CODE_STRING)
        setCursor(Qt::IBeamCursor);
    else
        setCursor(Qt::ArrowCursor);
}
