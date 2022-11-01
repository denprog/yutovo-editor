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

void Row::Remake(CaretState& caret_state, bool with_elements)
{
    if (elements->Count() > 1)
    {
        for (size_t i = 0; i < elements->Count();)
        {
            auto el = (*elements)[i];
            if (el->type == ElementType::STRING)
            {
                if (el->elements->Count() == 0 && parent->elements->Count() > 1)
                {
                    elements->RemoveAt(i, 1); //remove empty strings
                    continue;
                }
                if (i < elements->Count() - 1)
                {
                    if (el->Merge(elements->Get(i + 1), caret_state))
                    {
                        el->Remake(caret_state, true);
                        continue;
                    }
                }
            }
            ++i;
        }
    }
    else if (elements->Count() == 0)
        AddElement(ElementPtr(new String(this))); //insert empty string

    if (with_elements)
        elements->Remake(caret_state);
    
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

bool Row::InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    for (auto& el : _elements)
    {
        if (el->type != ElementType::STRING) //here can be inserted only strings for a while
            return parent->InsertElements(_elements, before_state, after_state, with_undo);
    }
    for (size_t i = 0; i < _elements.size(); ++i)
    {
        elements->Insert(_elements[i], before_state.GetElementPos(id) + i + 1);
        CaretState c;
        elements->Get(before_state.GetElementPos(id) + i + 1)->GetLastCaretState(c);
        after_state = c;
    }
    if (with_undo)
        document->DeleteElements(CaretState(_elements), false, true);
    return true;
}

bool Row::GetBeginCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    return GetFirstCaretState(after_state);
}

bool Row::GetEndCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    return GetLastCaretState(after_state);
}

bool Row::CanContinueSelection()
{
    return true;
}

// bool Row::GetTopCaretState(const int x, const int y, CaretState& res)
// {
//     return false;
// }

// bool Row::GetBottomCaretState(const int x, const int y, CaretState& res)
// {
//     if (y > rect.top)
//         return parent->GetBottomCaretState(x, y, res);

//     CaretState next, last;
//     if (!GetFirstCaretState(next))
//         return false;
//     if (!GetLastCaretState(last))
//         return false;
    
// 	int min_dist = std::numeric_limits<int>::max();

//     do
//     {
//         Rect r = document->GetCaretRect(next);
//         int dist = r.DistToPoint(x, y);
//         if (dist < min_dist)
//             res = next;
//         CaretState c = next;

//         if (!GetRightCaretState(c, next))
//             break;
//     }
//     while (next != last);

//     return true;
// }

}
