#include "window.h"

namespace yutovo
{

//Window

Window::Window()
{
}

void Window::DrawFillRect(const Rect& rect, const Color color)
{
    DrawFillRect(rect.left, rect.top, rect.width, rect.height, color);
}

void Window::DrawRect(const Rect& rect, const Color color)
{
    DrawRect(rect.left, rect.top, rect.width, rect.height, color);
}

void Window::ClearRect(const Rect& rect)
{
    ClearRect(rect.left, rect.top, rect.width, rect.height);
}

void Window::MoveDocument(const int left, const int top)
{
    document_point.x = left > 0 ? left : 0;
    document_point.y = top > 0 ? top : 0;
}

void Window::SetDocumentSize(const Size size)
{
    document_size = size;
}

Point Window::GetDocumentPoint()
{
    return document_point;
}

void Window::BeginDrawOutside()
{
    draw_doc = false;
}

void Window::EndDrawOutside()
{
    draw_doc = true;
}

void Window::OnCaretMoved(const EditorState editor_state)
{
}

void Window::OnSaveResult(const uint task_id, IOResult result)
{
}

void Window::OnLoadResult(const uint task_id, IOResult result)
{
}

void Window::OnCopyResult(CopyResult result)
{
}

void Window::OnPasteResult(PasteResult result)
{
}

}
