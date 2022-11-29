#include "row.h"
#include "str.h"
#include "document.h"

namespace yutovo
{

//Row

Row::Row(Element* parent) :
    Element(parent)
{
    type = ElementType::ROW;

    ElementPtr str(new String(this));
    AddElement(str);
}

Element* Row::Clone()
{
    return new Row(*this);
}

Element* Row::Create(Element* parent)
{
    return new Row(parent);
}

void Row::Remake(bool with_elements)
{
    if (elements->Count() > 1)
    {
        for (size_t i = 0; i < elements->Count();)
        {
            auto el = (*elements)[i];
            if (el->type == ElementType::STRING)
            {
                if (el->elements->Count() == 0 && elements->Count() > 1)
                {
                    elements->RemoveAt(i, 1); //remove empty strings
                    continue;
                }
                if (i < elements->Count() - 1)
                {
                    if (el->Merge(elements->Get(i + 1)))
                    {
                        el->Remake(true);
                        continue;
                    }
                }
            }
            ++i;
        }
    }
    else if (elements->Count() == 0)
    {
        AddElement(ElementPtr(new String(this))); //insert empty string
    }

    if (with_elements)
        elements->Remake();
    
    int cx = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto element = elements->Get(i);
        element->rect.Move(cx, 1);
        cx += element->rect.width;
    }
    UpdateRect();

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto element = elements->Get(i);
        if (element->baseline > baseline)
            baseline = element->baseline;
    }

    for (uint i = 0; i < elements->Count(); ++i)
    {
        const auto& el = elements->Get(i);
        el->rect.Move(el->rect.left, baseline - el->baseline);
    }

    UpdateRect();

    document->Remake(parent->id, false);
}

bool Row::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    CaretState caret_state = caret->GetCaretState();
    if (!caret_state.IsInsideElement(id))
        return parent->InsertElements(_elements, with_undo);
    
    for (auto& el : _elements)
    {
        if (el->type != ElementType::STRING) //here can be inserted only strings for a while
            return parent->InsertElements(_elements, with_undo);
    }

    CaretState c;
    ElementPtr el = document->GetParent(caret_state.id);
    if (el->id == id)
    {
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            elements->Insert(_elements[i], i);
            if (elements->Get(i)->GetLastCaretState(c, nullptr))
                caret->SetState(c);
        }
    }
    else
    {
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            uint p = caret_state.GetElementPos(el->id);
            if (el->GetFirstCaretState(c, nullptr) && c == caret_state)
            {
                elements->Insert(_elements[i], p + i);
                if (elements->Get(p + i)->GetLastCaretState(c, nullptr))
                    caret->SetState(c);
                if (with_undo)
                {
                    document->DeleteElements(false, false, true);
                    document->PushEditorState(SelectionState(id, p + i, 1), true);
                }
            }
            else if (el->GetLastCaretState(c, nullptr) && c == caret_state)
            {
                elements->Insert(_elements[i], p + i + 1);
                if (elements->Get(p + i + 1)->GetLastCaretState(c, nullptr))
                    caret->SetState(c);
                if (elements->Count() > p + i + 2)
                {
                    ElementPtr el1 = elements->Get(p + i + 1);
                    ElementPtr el2 = elements->Get(p + i + 2);
                    el1->Merge(el2);
                }
                if (with_undo)
                {
                    document->DeleteElements(false, false, true);
                    document->PushEditorState(SelectionState(id, p + i + 1, 1), true);
                }
            }
            else
            {
                if (el->SplitAt(caret_state.GetPos()))
                {
                    elements->Insert(_elements[i], p + i + 1);
                    if (elements->Get(p + i + 1)->GetLastCaretState(c, nullptr))
                        caret->SetState(c);
                }
                if (with_undo)
                {
                    document->DeleteElements(false, false, true);
                    document->PushEditorState(SelectionState(id, p + i + 1, 1), true);
                }
            }
        }
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Row::DeleteElements(bool left, bool with_undo)
{
    CaretState before_state = caret->GetCaretState();
    if (selection->IsEmpty())
    {
        if (!left)
        {
            if (before_state.GetPos() == 0 && elements->Count() == 1 && elements->Get(0)->type == ElementType::STRING && 
                elements->Get(0)->elements->Count() == 0)
                return parent->DeleteElements(left, with_undo);
        }

        CaretState first_state, last_state;
        GetFirstCaretState(first_state, nullptr);
        GetLastCaretState(last_state, nullptr);
        if ((left && before_state == first_state) || (!left && before_state == last_state))
            return parent->DeleteElements(left, with_undo);

        int pos = before_state.GetElementPos(id);
        if (left)
        {
            if (pos == 0)
            {
                int p = elements->GetElementPos(document->GetParent(before_state.id)->id);
                if (p > 0)
                {
                    auto el = elements->Get(p - 1);
                    if (el->CanContinueSelection())
                    {
                        CaretState c;
                        if (el->GetLastCaretState(c, nullptr))
                        {
                            caret->SetState(c);
                            if (!el->DeleteElements(left, with_undo))
                                return parent->DeleteElements(left, with_undo);
#ifdef DEBUG
                            to_str = ToText();
#endif
                            return true;
                        }
                    }
                }
                else
                    return parent->DeleteElements(left, with_undo);
            }
            else                
                elements->RemoveAt(pos - 1, 1);
        }
        else
        {
            if (pos == elements->Count())
                return parent->DeleteElements(left, with_undo);
            else
                elements->RemoveAt(pos, 1);
        }

        document->Remake(id, false);

#ifdef DEBUG
        to_str = ToText();
#endif
        return true;
    }
    else
    {
        uint start, size;
        if (selection->Has(id, start, size))
        {
            elements->RemoveAt(start, size);
            document->Remake(id, false);
#ifdef DEBUG
            to_str = ToText();
#endif
            return true;
        }
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return false;
}

bool Row::ChangeStringFormat(const StringFormatPtr format, bool with_undo)
{
    SelectionState s = selection->GetState();
    for (auto& t : s.state)
    {
        if (!IsChild(id, t.id))
            continue;
        ElementPtr el = document->GetElement(t.id);
        if (el->SplitAt(t.start))
            el = elements->Get(elements->GetElementPos(el->id) + 1);
        el->SplitAt(t.size);
        el->ChangeStringFormat(format, with_undo);
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Row::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    return GetFirstCaretState(caret_state, select);
}

bool Row::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    return GetLastCaretState(caret_state, select);
}

bool Row::CanContinueSelection()
{
    return true;
}

}
