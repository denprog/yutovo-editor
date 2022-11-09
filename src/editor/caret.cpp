#include "caret.h"
#include "text.h"
#include "document.h"
#include <chrono>

namespace yutovo
{

//Caret

using namespace std::chrono_literals;

Caret::Caret(Window* _window, Text* _text) :
    window(_window),
    text(_text)
{
}

void Caret::SetState(const CaretState& caret_state, bool update_x_pos)
{
    if (!text->document->GetParent(caret_state.id))
        return;
    current_element = text->document->GetParent(caret_state.id).get();
    current_pos = caret_state.GetPos();
    selections = caret_state.selections;
    if (update_x_pos)
        UpdateXPos();
}

CaretState Caret::GetCaretState()
{
    return CaretState(current_element, current_pos, selections);
}

void Caret::SetVisible(bool _visible)
{
    if (!_visible)
        Hide();
    visible = _visible;
}

void Caret::Show()
{
    if (!visible)
        return;
    
    caret_rect = current_element->GetAbsoluteRect(current_element->GetCaretRect(current_pos));
    if (!show)
        window->StoreRect(caret_rect);
    current_element->DrawCaret(current_pos);
    window->Update(caret_rect);
    show = true;
}

void Caret::Hide()
{
    if (!visible)
        return;
    
    window->RestoreRect();
    window->Update(caret_rect);
    show = false;
}

void Caret::Blink()
{
    if (show)
        Hide();
    else
        Show();
}

void Caret::MoveToDocumentBegin(bool select)
{
    CaretState c;
    text->GetFirstCaretState(c);
    SetState(c);
    UpdateXPos();
}

void Caret::MoveToDocumentEnd(bool select)
{
    CaretState c;
    text->GetLastCaretState(c);
    SetState(c);
    UpdateXPos();
}

void Caret::MoveHome(bool select)
{
    CaretState cur = GetCaretState();
    if (!select)
        cur.selections.ClearSelection();

    CaretState res;
    if (current_element->GetBeginCaretState(cur, res, select))
    {
        SetState(res);
        UpdateXPos();
    }
}

void Caret::MoveEnd(bool select)
{
    CaretState cur = GetCaretState();
    if (!select)
        cur.selections.ClearSelection();

    CaretState res;
    if (current_element->GetEndCaretState(cur, res, select))
    {
        SetState(res);
        UpdateXPos();
    }
}

void Caret::MoveLeft(bool select)
{
    CaretState cur = GetCaretState();
    if (cur.selections.HasSelection() && !select)
    {
        uint start, size;
        if (cur.selections.HasSelection(text->document->GetParent(cur.id)->id, start, size))
        {
            selections.ClearSelection();
            SetState(CaretState(text->document->GetParent(cur.id)->id, start));
            return;
        }
    }
    
    CaretState res;
    if (current_element->GetLeftCaretState(cur, res, select))
    {
        SetState(res);
        UpdateXPos();
    }
}

void Caret::MoveRight(bool select)
{
    CaretState cur = GetCaretState();
    if (cur.selections.HasSelection() && !select)
    {
        uint start, size;
        if (cur.selections.HasSelection(text->document->GetParent(cur.id)->id, start, size))
        {
            selections.ClearSelection();
            SetState(CaretState(text->document->GetParent(cur.id)->id, start + size));
            return;
        }
    }
    
    CaretState res;
    if (current_element->GetRightCaretState(cur, res, select))
    {
        SetState(res);
        UpdateXPos();
    }
}

void Caret::MoveUp(bool select)
{
    CaretState res;
    Rect r = current_element->GetAbsoluteRect(current_element->GetCaretRect(current_pos));
    //if (!x_caret_state.IsEmpty())
    if (last_x_element)
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        r.left = x_rect.left;
    }
    if (current_element->GetTopCaretState(r.left, r.top, res, select))
    {
        if (!select)
            res.selections.ClearSelection();
        SetState(res);
    }
}

void Caret::MoveDown(bool select)
{
    CaretState res;
    Rect r = current_element->GetAbsoluteRect(current_element->GetCaretRect(current_pos));
    //if (!x_caret_state.IsEmpty())
    if (last_x_element)
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        r.left = x_rect.left;
    }
    if (current_element->GetBottomCaretState(r.left, r.GetBottom(), res, select))
    {
        if (!select)
            res.selections.ClearSelection();
        SetState(res);
    }
}

void Caret::UpdateXPos()
{
    //x_caret_state = CaretState(current_element, current_pos);
    last_x_element = current_element;
    last_x_pos = current_pos;
}

}
