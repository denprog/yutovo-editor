#include "row.h"
#include "str.h"
#include "document.h"

namespace yutovo
{

//Row

Row::Row(Document* _document) :
    Element(_document)
{
    type = ElementType::ROW;
}

Row::Row(Document* _document, Element* _parent) :
    Element(_document)
{
    type = ElementType::ROW;
    parent = _parent;
}

Row::Row(Element* _parent, bool with_string) :
    Element(_parent)
{
    type = ElementType::ROW;
    if (with_string)
        AddEmptyElement();
}

Element* Row::Clone()
{
    return new Row(*this);
}

Element* Row::Create(Element* parent)
{
    return new Row(parent);
}

void Row::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    Element::Remake(with_elements, with_parent, with_undo);

    int cx = 0;
    int left_m, top_m, right_m, bottom_m;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->GetMargin(left_m, top_m, right_m, bottom_m);
        el->rect.Move(cx + left_m, 0);
        cx += el->rect.width + left_m + right_m;
    }
    UpdateRect();

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    for (uint i = 0; i < elements->Count(); ++i)
    {
        const auto& el = elements->Get(i);
        el->rect.Move(el->rect.left, baseline - el->baseline);
    }

    UpdateRect();

    rect.width += right_m;

    //align top and bottom of the line by the margins of the elements
    int max_top_m = 0, max_bottom_m = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->GetMargin(left_m, top_m, right_m, bottom_m);
        if (top_m > 0 && el->rect.top - top_m < 0 && max_top_m < top_m - el->rect.top)
            max_top_m = top_m - el->rect.top;
        if (bottom_m > 0 && el->rect.GetBottom() + bottom_m > rect.height && max_bottom_m < bottom_m - (rect.height - el->rect.GetBottom()))
            max_bottom_m = bottom_m - (rect.height - el->rect.GetBottom());
    }

    rect.height += max_top_m + max_bottom_m;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        el->rect.top += max_top_m;
    }

    baseline += max_top_m;

    if (rect != last_rect)
    {
        if (with_parent)
            document->Remake(parent->id, false, with_undo, false);
        document->Redraw(id, false);
    }
    last_rect = rect;
}

void Row::Normalize(bool with_undo)
{
    if (!document->can_normalize)
        return;
    
    Element::Normalize(with_undo);

    if (elements->Count() > 1)
    {
        for (size_t i = 0; i < elements->Count();)
        {
            auto el = (*elements)[i];
            if (document->IsString(el))
            {
                if (el->elements->Count() == 0 && elements->Count() > 1)
                {
                    if (with_undo)
                    {
                        auto t = elements->Get(i)->Clone();
                        document->CallFunc(ElementId{}, 
                            [d = document](const ElementId id)
                            {
                                d->can_normalize = true;
                            },
                            true);
                        document->InsertElement(t);
                        document->PushEditorState(CaretState(id, i), true);
                        document->CallFunc(ElementId{}, 
                            [d = document](const ElementId id)
                            {
                                d->can_normalize = false;
                            },
                            true);
                    }
                    elements->RemoveAt(i, 1); //remove empty strings
                    window->OnCaretMoved(document->GetEditorState());
                    if (i > 0)
                        --i;
                    continue;
                }
                if (i < elements->Count() - 1)
                {
                    ElementPtr el1, el2;
                    if (with_undo)
                    {
                        el1.reset(el->Clone());
                        el2.reset(elements->Get(i + 1)->Clone());
                    }
                    if (el->Merge(elements->Get(i + 1)))
                    {
                        el->UpdateRect(true);
                        if (with_undo)
                        {
                            document->CallFunc(ElementId{},
                                [d = document, _id = id](const ElementId id)
                                {
                                    d->can_normalize = true;
                                },
                                true);
                            document->InsertElement(el1);
                            document->PushEditorState(CaretState(id, elements->GetElementPos(el->id)), true);
                            document->InsertElement(el2);
                            document->PushEditorState(CaretState(id, elements->GetElementPos(el->id)), true);
                            document->CallFunc(ElementId{},
                                [d = document, _id = id](const ElementId id)
                                {
                                    d->can_normalize = true;
                                    d->GetElement(_id)->Normalize(false);
                                    d->can_normalize = false;
                                },
                                true);
                            document->DeleteElements(false, false, true);
                            document->CallFunc(ElementId{},
                                [d = document, _id = id](const ElementId id)
                                {
                                    d->can_normalize = false;
                                },
                                true);
                            document->PushEditorState(SelectionState(id, elements->GetElementPos(el->id), 1), true);
                        }
                        window->OnCaretMoved(document->GetEditorState());
                        continue;
                    }
                }
            }
            ++i;
        }
    }
    else if (elements->Count() == 0)
    {
        if (with_undo)
        {
            document->DeleteElements(false, false, true);
            document->PushEditorState(CaretState(id), SelectionState(parent->id, parent->elements->GetElementPos(id), 1), true);
        }
        AddEmptyElement(); //insert empty string
        CaretState c;
        if (GetFirstCaretState(c, nullptr))
            caret->SetState(c);
    }
}

bool Row::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    CaretState caret_state = caret->GetCaretState();
    if (!caret_state.IsInsideElement(id))
        return parent->InsertElements(_elements, with_undo);

    for (auto& el : _elements)
    {
        if (document->IsRow(el))
        {
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                std::vector<ElementPtr> v;
                v.push_back(el->elements->Get(i));
                if (!InsertElements(v, with_undo))
                    return false;
            }
            return true;
        }
        else if (document->IsParagraph(el)) //paragraphs can be inserted above
            return parent->InsertElements(_elements, with_undo);
    }

    CaretState c;
    ElementPtr el = document->GetParent(caret_state.id);
    if (el->id == id)
    {
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            auto ins = _elements[i];
            uint p = caret_state.GetPosInElement(id);
            elements->Insert(ins, p + i);
            if (with_undo)
            {
                document->DeleteElements(false, false, true);
                document->PushEditorState(SelectionState(id, p + i, 1), true);
            }
            if (document->pasting)
            {
                if (ins->HasLastCaretState())
                    caret->SetState(id, p + i + 1, true);
                else if (ins->GetLastCaretState(c, nullptr))
                    caret->SetState(c);
                parent->Normalize(with_undo);
                continue;
            }
            if (i == 0 && elements->Get(p + i)->AfterInsert(with_undo))
            {
                parent->Normalize(with_undo);
                continue;
            }
            if (elements->Get(p + i)->GetLastCaretState(c, nullptr))
                caret->SetState(c);
            parent->Normalize(with_undo);
        }
    }
    else
    {
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            ElementPtr ins(_elements[i]);
            bool b = false;
            uint p = caret_state.GetElementPos(el->id);
            if (el->GetLastCaretState(c, nullptr) && c == caret_state)
            {
                elements->Insert(ins, p + i + 1);
                if (with_undo)
                {
                    document->DeleteElements(false, false, true);
                    document->PushEditorState(SelectionState(id, p + i + 1, 1), true);
                }
                if (i == 0 && !document->pasting)
                    b = ins->AfterInsert(with_undo);
                if (!b)
                {
                    if (ins->HasLastCaretState())
                    {
                        caret->SetState(id, p + i + 2, true);
                    }
                    else
                    {
                        if (elements->Get(p + i + 1)->GetLastCaretState(c, nullptr))
                            caret->SetState(c);
                        else if (i > 0 && _elements[i - 1]->parent->GetLastCaretState(c, nullptr))
                            caret->SetState(c);
                    }
                }
                if (elements->Count() > p + i + 2)
                {
                    ElementPtr el1 = elements->Get(p + i + 1);
                    ElementPtr el2 = elements->Get(p + i + 2);
                    el1->Merge(el2);
                }
            }
            else if (el->GetFirstCaretState(c, nullptr) && c == caret_state)
            {
                elements->Insert(ins, p + i);
                if (i == 0 && !document->pasting)
                    b = ins->AfterInsert(with_undo);
                if (with_undo)
                {
                    document->DeleteElements(false, false, true);
                    document->PushEditorState(SelectionState(id, p + i, 1), true);
                }
                if (!b)
                {
                    if (elements->Get(elements->Count() > p + i + 1 ? p + i + 1 : p + i)->GetLastCaretState(c, nullptr))
                        caret->SetState(c);
                }
            }
            else
            {
                if (el->SplitAt(caret_state.GetPos()))
                {
                    elements->Insert(ins, p + i + 1);
                    if (i == 0 && !document->pasting)
                        b = ins->AfterInsert(with_undo);
                    if (!b && elements->Get(p + i + 1)->GetLastCaretState(c, nullptr))
                        caret->SetState(c);
                }
                if (with_undo)
                {
                    document->DeleteElements(false, false, true);
                    document->PushEditorState(SelectionState(id, elements->GetElementPos(ins->id), 1), true);
                }
            }
        }
        Normalize(with_undo);
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
            if (before_state.GetPos() == 0 && elements->Count() == 1 && document->IsString(elements->Get(0)) && elements->Get(0)->elements->Count() == 0)
            {
                return parent->DeleteElements(left, with_undo);
            }
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
                            el->DeleteElements(left, with_undo);
#ifdef DEBUG
                            to_str = ToText();
#endif
                            return true;
                        }
                    }
                    else
                    {
                        if (with_undo)
                        {
                            document->InsertElement(elements->Get(p - 1)->Clone(), false, true);
                            document->PushEditorState(CaretState(id, p - 1), true);
                        }
                        elements->RemoveAt(p - 1, 1);
                        Normalize(with_undo);
                        parent->Remake(true, true, with_undo);
#ifdef DEBUG
                        to_str = ToText();
#endif
                        return true;
                    }
                }
                else
                {
                    p = elements->GetElementPos(before_state.id);
                    if (p > 0)
                    {
                        if (before_state != last_state)
                            --p;
                        auto el = elements->Get(p);
                        if (el->CanContinueSelection())
                        {
                            CaretState c;
                            if (!el->GetLastCaretState(c, nullptr))
                                return false;
                            caret->SetState(c);
                            return el->DeleteElements(left, with_undo);
                        }
                        if (with_undo)
                        {
                            document->InsertElement(el->Clone(), false, true);
                            document->PushEditorState(CaretState(id, p), true);
                        }
                        elements->RemoveAt(p, 1);
                        parent->Remake(true, true, with_undo);
#ifdef DEBUG
                        to_str = ToText();
#endif
                        return true;
                    }
                    return parent->DeleteElements(left, with_undo);
                }
            }
            else
            {
                if (with_undo)
                {
                    document->InsertElement(elements->Get(pos - 1)->Clone(), false, true);
                    document->PushEditorState(CaretState(id, pos - 1), true);
                }
                elements->RemoveAt(pos - 1, 1);
            }
        }
        else
        {
            if (pos == elements->Count())
                return parent->DeleteElements(left, with_undo);
            else
            {
                int p = elements->GetElementPos(before_state.id);
                if (p == -1)
                    p = elements->GetElementPos(document->GetParent(before_state.id)->id);
                if (p < elements->Count())
                {
                    if (before_state.GetElement() != id)
                        ++p;
                    auto el = elements->Get(p);
                    if (el && el->CanContinueSelection())
                    {
                        CaretState c;
                        if (el->GetFirstCaretState(c, nullptr))
                        {
                            caret->SetState(c);
                            el->DeleteElements(left, with_undo);
#ifdef DEBUG
                            to_str = ToText();
#endif
                            return true;
                        }
                    }
                    else if (p < elements->Count())
                    {
                        if (with_undo)
                        {
                            document->InsertElement(elements->Get(p)->Clone(), false, true);
                            document->PushEditorState(CaretState(id, p), true);
                        }
                        elements->RemoveAt(p, 1);
                    }
                }
                else if (p >= 0)
                {
                    if (with_undo)
                    {
                        document->InsertElement(elements->Get(p)->Clone(), false, true);
                        document->PushEditorState(CaretState(id, p), true);
                    }
                    elements->RemoveAt(p, 1);
                }

                Normalize(with_undo);
            }
        }

        parent->Remake(true, true, with_undo);
        document->Redraw(parent->id, true);

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
            if (with_undo)
            {
                std::vector<ElementPtr> clone;
                elements->Clone(clone, start, size);
                document->InsertElements(clone, false, true);
                document->PushEditorState(CaretState(id, start), true);
            }
            elements->RemoveAt(start, size);
            Normalize(with_undo);
            parent->Remake(true, true, with_undo);
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
    Element::ChangeStringFormat(format, with_undo);
    
    SelectionState s = selection->GetState();
    for (auto& t : s.state)
    {
        if (!IsChild(id, t.id))
            continue;
        ElementPtr el = document->GetElement(t.id);
        if (el->SplitAt(t.start))
            el = elements->Get(elements->GetElementPos(el->id) + 1);
        if (el)
        {
            el->SplitAt(t.size);
            el->ChangeStringFormat(format, with_undo);
        }
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

bool Row::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!select)
        return Element::GetTopCaretState(x, y, caret_state, select);
    
    if (y < GetAbsoluteRect().GetBottom())
        return parent->GetTopCaretState(x, y, caret_state, select);

    CaretState next, last;
    if (!GetFirstCaretState(next, nullptr))
        next.SetState(id);
    if (!GetLastCaretState(last, nullptr))
        last.SetState(id);

    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, nullptr))
            break;
        
        r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    CaretState prev = caret->GetCaretState();
    ElementId prev_id = prev.GetElement();
    if (!IsChild(id, prev_id) && id != prev_id)
    {
        //select the row below
        auto el = document->GetElement(prev_id);
        if (!el->GetFirstCaretState(prev, select))
            return false;
    }

    GetLastCaretState(next, nullptr);
    while (next != caret_state)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetLeftCaretState(next, select))
            break;
    }

    return true;
}

bool Row::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!select)
        return Element::GetBottomCaretState(x, y, caret_state, select);

    if (y > GetAbsoluteRect().top)
        return parent->GetBottomCaretState(x, y, caret_state, select);

    CaretState next, last;
    if (!GetFirstCaretState(next, nullptr))
        next.SetState(id);
    if (!GetLastCaretState(last, nullptr))
        last.SetState(id);

    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, nullptr))
            break;
        
        Rect r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    CaretState prev = caret->GetCaretState();
    ElementId prev_id = prev.GetElement();
    if (!IsChild(id, prev_id) && id != prev_id)
    {
        //select the row above
        auto el = document->GetElement(prev_id);
        if (!el->GetLastCaretState(prev, select))
            return false;
    }

    GetFirstCaretState(next, nullptr);
    while (next != caret_state)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, select))
            break;
    }

    return true;
}

bool Row::CanContinueSelection()
{
    return true;
}

void Row::AddEmptyElement()
{
    AddElement(ElementPtr(new String(this)));
}

bool Row::IsEmpty()
{
    if (elements->Count() != 1)
        return false;
    if (!document->IsString(elements->Get(0)))
        return false;
    return elements->Get(0)->elements->Count() == 0;
}

}
