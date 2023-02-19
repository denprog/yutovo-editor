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

    last_pos = false;
    element = el;
    if (document->IsString(element->id))
        str_pos = caret_state.GetPos();
    else
    {
        str_pos = -1;
        int p = caret_state.GetPos();
        if (p == element->elements->Count())
        {
            element = element->elements->Get(p - 1);
            last_pos = true;
        }
        else
        {
            element = element->elements->Get(p);
        }
    }
    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

void Caret::SetState(const ElementId id, const uint pos, bool update_x_pos)
{
    element = document->GetElement(id);
    if (!element)
    {
        Reset();
        return;
    }

    last_pos = false;
    if (document->IsString(element->id))
        str_pos = pos;
    else
    {
        str_pos = -1;
        if (pos == element->elements->Count())
        {
            element = element->elements->Get(pos - 1);
            last_pos = true;
        }
        else
        {
            element = element->elements->Get(pos);
            if (!element->HasCaretState())
            {
                CaretState c;
                if (element->GetFirstCaretState(c, nullptr))
                {
                    SetState(c, update_x_pos);
                    return;
                }
            }
        }
    }
    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

void Caret::SetState(const ElementId id, bool update_x_pos)
{
    element = document->GetElement(id);
    if (!element)
    {
        Reset();
        return;
    }
    last_pos = false;
    if (document->IsString(element->id))
        str_pos = GetChildPos(id);
    else
        str_pos = -1;

    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

void Caret::SetPos(const uint pos, bool update_x_pos)
{
    last_pos = false;
    str_pos = -1;
    if (document->IsString(element->id))
        str_pos = pos;
    else
    {
        if (pos == element->elements->Count())
        {
            element = element->elements->Get(pos - 1);
            last_pos = true;
        }
        else
        {
            element = element->elements->Get(pos);
        }
    }

    if (update_x_pos)
        UpdateXPos();
    
    window->OnCaretMoved(document->GetEditorState());
    document->UpdateFormats();
}

Element* Caret::GetElement() const
{
    if (str_pos >= 0)
        return element.get();
    if (!element)
        return nullptr;
    return element->parent;
}

int Caret::GetPos() const
{
    if (str_pos >= 0)
        return str_pos;
    if (!element)
        return -1;
    return last_pos ? GetChildPos(element->id) + 1 : GetChildPos(element->id);
}

CaretState Caret::GetCaretState()
{
    return CaretState(GetElement(), GetPos());
}

void Caret::SetVisible(bool _visible)
{
    if (!_visible)
        Hide();
    visible = _visible;
}

void Caret::Show()
{
    auto el = GetElement();
    if (!visible || !el)
        return;
    
    caret_rect = el->GetAbsoluteRect(el->GetCaretRect(GetPos()));
    if (!show)
        window->StoreRect(caret_rect);
    el->DrawCaret(GetPos());
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
    if (document->text->GetFirstCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveToDocumentEnd(Selection* selection)
{
    CaretState c;
    if (document->text->GetLastCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveHome(Selection* selection)
{
    CaretState c = GetCaretState();
    if (GetElement()->GetBeginCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveEnd(Selection* selection)
{
    CaretState c = GetCaretState();
    if (GetElement()->GetEndCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveLeft(Selection* selection)
{
    CaretState c = GetCaretState();
    if (GetElement()->GetLeftCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveRight(Selection* selection)
{
    CaretState c = GetCaretState();
    if (GetElement()->GetRightCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveUp(Selection* selection)
{
    Rect r = GetElement()->GetAbsoluteRect(GetElement()->GetCaretRect(GetPos()));
    if (last_x_element)
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        r.left = x_rect.left;
    }
    CaretState c;
    if (GetElement()->GetTopCaretState(r.left, r.top + 2, c, selection))
        SetState(c, false);
}

void Caret::MoveDown(Selection* selection)
{
    Rect r = GetElement()->GetAbsoluteRect(GetElement()->GetCaretRect(GetPos()));
    if (last_x_element)
    {
        //fix x position
        Rect x_rect = last_x_element->GetAbsoluteRect(last_x_element->GetCaretRect(last_x_pos));
        r.left = x_rect.left;
    }
    CaretState c;
    if (GetElement()->GetBottomCaretState(r.left, r.GetBottom() - 2, c, selection))
        SetState(c, false);
}

void Caret::MoveWordLeft(Selection* selection)
{
    CaretState c = GetCaretState();
    if (GetElement()->GetWordLeftCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

void Caret::MoveWordRight(Selection* selection)
{
    CaretState c = GetCaretState();
    if (GetElement()->GetWordRightCaretState(c, selection))
    {
        SetState(c);
        UpdateXPos();
    }
}

bool Caret::IsInsideElement(const ElementId id)
{
    auto el = GetElement();
    if (!el)
        return false;
    if (document->IsString(el->id))
        return id == el->id || IsChild(id, GetParent(el->id));
    return IsChild(id, el->id);
}

bool Caret::IsOnElement(const ElementId id)
{
    if (GetCaretState() == id)
        return true;
    if (last_pos && GetPrevPos(GetCaretState().id) == id)
        return true;
    return false;
}

void Caret::UpdateXPos()
{
    if (!element)
        return;
    if (document->IsString(element->id))
    {
        last_x_element = element.get();
        last_x_pos = str_pos;
    }
    else
    {
        last_x_element = element->parent;
        last_x_pos = GetChildPos(element->id);
    }
}

void Caret::Reset()
{
    element = nullptr;
    str_pos = -1;
}

}
