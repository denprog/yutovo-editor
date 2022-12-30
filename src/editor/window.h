#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <string>
#include <vector>
#include "util.h"
#include "style.h"
#include "editor_state.h"
#include "result_codes.h"

namespace yutovo
{

//Base class for output windows
class Window
{
public:
    Window();

    virtual void DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color) = 0;
    virtual void DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color) = 0;
    void DrawRect(const Rect& rect, const Color color);
    virtual void DrawRect(const int x1, const int y1, const int width, const int height, const Color color) = 0;
    void DrawFillRect(const Rect& rect, const Color color);
    virtual void DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color) = 0;
    void ClearRect(const Rect& rect);
    virtual void ClearRect(const int x1, const int y1, const int width, const int height) = 0;

    virtual void ClearSurface() = 0;

    virtual void StoreRect(const Rect& rect) = 0;
    virtual void RestoreRect() = 0;

    virtual Size GetTextSize(const std::string& text, const StringFormatPtr format) = 0;
    virtual int GetCharPos(const std::string& text, const StringFormatPtr format, int pos) = 0;
    virtual int GetFontAscent(const StringFormatPtr format) = 0;

    virtual void SetViewPort(const Rect view_port) = 0;
    virtual void AddViewPort(const Rect view_port) = 0;
    virtual Rect GetViewPort(const int pos) = 0;

    virtual void Update(const Rect& rect) = 0;

    virtual void Resize(uint width, uint height) = 0;

    virtual Rect GetRect() = 0;

    virtual void MoveDocument(const int left, const int top);
    Point GetDocumentPoint();

    void BeginDrawOutside();
    void EndDrawOutside();

    virtual void OnCaretMoved(const EditorState editor_state);

    virtual void OnSaveResult(const uint task_id, IOResult result);
    virtual void OnLoadResult(const uint task_id, IOResult result);

    virtual void OnCopyResult(CopyResult result);
    virtual void OnPasteResult(PasteResult result);

protected:
    Point document_point;
    bool draw_doc = false; //drawing inside the clipping regions
};

}

#endif
