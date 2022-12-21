#include "caret.h"
#include "text.h"
#include "document.h"
#include "util.h"
#include <chrono>

namespace yutovo
{

//Caret

using namespace std::chrono_literals;

Caret::Caret(Document* _document) :
    document(_document),
    window(document->window)
{
}

void Caret::SetState(const CaretState& caret_state, bool update_x_pos)
{
    auto el = document->GetParent(caret_state.id);
    if (!el)
    {
        Reset();
        return;
    }
    current_element = el.get();
    current_pos = caret_state.GetPos();
    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

void Caret::SetState(const ElementId id, const uint pos, bool update_x_pos)
{
    auto el = document->GetElement(id);
    if (!el)
    {
        Reset();
        return;
    }
    current_element = el.get();
    current_pos = pos;
    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

void Caret::SetState(const ElementId id, bool update_x_pos)
{
    auto el = document->GetParent(id);
    if (!el)
    {
        Reset();
        return;
    }
    current_element = el.get();
    current_pos = id[id.size() - 1];
    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

void Caret::SetPos(const uint pos, bool update_x_pos)
{
    current_pos = pos;
    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

CaretState Caret::GetCaretState()
{
    return CaretState(current_element, current_pos);
}

void Caret::SetVisible(bool _visible)
{
    if (!_visible)
        Hide();
    visible = _visible;
}

void Caret::Show()
{
    if (!visible || !current_element)
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

void Caret::MoveToDocumentBegin(Selection* selection)
{
    CaretState c;
    if (text->GetFirstCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveToDocumentEnd(Selection* selection)
{
    CaretState c;
    if (text->GetLastCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveHome(Selection* selection)
{
    CaretState c = GetCaretState();
    if (current_element->GetBeginCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveEnd(Selection* selection)
{
    CaretState c = GetCaretState();
    if (current_element->GetEndCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveLeft(Selection* selection)
{
    CaretState c = GetCaretState();
    if (current_element->GetLeftCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveRight(Selection* selection)
{
    CaretState c = GetCaretState();
    if (current_element->GetRightCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveUp(Selection* selection)
{
    Rect r = current_element->GetAbsoluteRect(current_element->GetCaretRect(current_pos));
    if (last_x_element)
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        r.left = x_rect.left;
    }
    CaretState c;
    if (current_element->GetTopCaretState(r.left, r.top + 2, c, selection))
        SetState(c, false);
}

void Caret::MoveDown(Selection* selection)
{
    Rect r = current_element->GetAbsoluteRect(current_element->GetCaretRect(current_pos));
    if (last_x_element)
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        r.left = x_rect.left;
    }
    CaretState c;
    if (current_element->GetBottomCaretState(r.left, r.GetBottom() - 2, c, selection))
        SetState(c, false);
}

void Caret::MoveWordLeft(Selection* selection)
{
    CaretState c = GetCaretState();
    if (current_element->GetWordLeftCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveWordRight(Selection* selection)
{
    CaretState c = GetCaretState();
    if (current_element->GetWordRightCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

bool Caret::IsInsideElement(const ElementId id)
{
    return current_element && IsChild(id, current_element->id);
}

bool Caret::IsOnElement(const ElementId id)
{
    return current_element && GetCaretState() == id;
}

void Caret::UpdateXPos()
{
    last_x_element = current_element;
    last_x_pos = current_pos;
}

void Caret::Reset()
{
    current_element = nullptr;
    current_pos = 0;
}

}
