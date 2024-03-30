#include "row.h"
#include "str.h"
#include "document.h"
#include "paragraph.h"

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

Element* Row::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Row(parent, false);
    return new Row(document);
}

bool Row::Remake(bool with_elements)
{
    if (document->break_remake)
        return false;

    bool changed = Element::Remake(with_elements);
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
        last_rect = rect;
        return true;
    }
    return changed;
}

void Row::Normalize()
{
    Element::Normalize();

    if (elements->Count() == 0)
    {
        AddEmptyElement(); //insert empty string
        CaretState c;
        if (GetFirstCaretState(c, nullptr))
            caret->SetState(c);
    }
    else
    {
        for (size_t i = 0; i < elements->Count();)
        {
            auto el = (*elements)[i];
            if (document->IsString(el) && el->can_merge)
            {
                if (el->elements->Count() == 0 && elements->Count() > 1)
                {
                    elements->RemoveAt(i, 1); //remove empty strings
                    window->OnCaretMoved(document->MakeEditorState());
                    if (i > 0)
                        --i;
                    continue;
                }
                if (i < elements->Count() - 1)
                {
                    if (el->Merge(elements->Get(i + 1)))
                    {
                        el->UpdateRect(true);
                        window->OnCaretMoved(document->MakeEditorState());
                        continue;
                    }
                }
            }
            ++i;
        }
    }
}

bool Row::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    CaretState caret_state = caret->GetCaretState();
    if (!caret_state.IsInsideElement(id) && caret_state.id != id)
        return parent->InsertElements(_elements, with_undo, changed_element);

    for (auto& el : _elements)
    {
        if (document->IsRow(el))
        {
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                std::vector<ElementPtr> v;
                v.push_back(el->elements->Get(i));
                if (!InsertElements(v, with_undo, changed_element))
                    return false;
            }
            changed_element = id;
            return true;
        }
        else if (document->IsParagraph(el) || el->type == ElementType::TEXT) //paragraphs can be inserted above
            return parent->InsertElements(_elements, with_undo, changed_element);
    }

    CaretState c;
    ElementPtr el = document->GetParent(caret_state.id);
    if (caret_state.id == id)
    {
        if (with_undo)
            document->StoreUndo(id);
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            auto ins = _elements[i];
            elements->Insert(ins, i);
            if (i == 0 && elements->Get(i)->AfterInsert(with_undo))
            {
                parent->Normalize();
                continue;
            }
            if (elements->Get(i)->GetLastCaretState(c, nullptr))
                caret->SetState(c);
            parent->Normalize();
        }
    }
    else if (el->id == id)
    {
        if (with_undo)
            document->StoreUndo(id);
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            auto ins = _elements[i];
            uint p = caret_state.GetPosInElement(id);
            elements->Insert(ins, p + i);
            if (document->pasting)
            {
                if (ins->HasLastCaretState())
                    caret->SetState(id, p + i + 1, true);
                else if (ins->GetLastCaretState(c, nullptr))
                    caret->SetState(c);
                parent->Normalize();
                continue;
            }
            if (i == 0 && elements->Get(p + i)->AfterInsert(with_undo))
            {
                parent->Normalize();
                continue;
            }
            if (elements->Get(p + i)->GetLastCaretState(c, nullptr))
                caret->SetState(c);
            parent->Normalize();
        }
    }
    else
    {
        if (with_undo)
            document->StoreUndo(parent->id);
        for (size_t i = 0; i < _elements.size(); ++i)
        {
            ElementPtr ins(_elements[i]);
            bool b = false;
            uint p = caret_state.GetElementPos(el->id);
            if (el->GetLastCaretState(c, nullptr) && c == caret_state)
            {
                elements->Insert(ins, p + i + 1);
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
                if (!b)
                {
                    if (document->pasting && ins->GetLastCaretState(c, nullptr))
                        caret->SetState(c);
                    else if (elements->Get(elements->Count() > p + i + 1 ? p + i + 1 : p + i)->GetLastCaretState(c, nullptr))
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
            }
        }
        Normalize();
    }

    changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Row::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    CaretState before_state = caret->GetCaretState();
    if (selection->IsEmpty())
    {
        if (!left)
        {
            if (before_state.GetPos() == 0 && elements->Count() == 1 && document->IsString(elements->Get(0)) && elements->Get(0)->elements->Count() == 0)
            {
                return parent->DeleteElements(left, with_undo, changed_element);
            }
        }

        CaretState first_state, last_state;
        GetFirstCaretState(first_state, nullptr);
        GetLastCaretState(last_state, nullptr);
        if ((left && before_state == first_state) || (!left && before_state == last_state))
            return parent->DeleteElements(left, with_undo, changed_element);

        if (left)
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
                        el->DeleteElements(left, with_undo, changed_element);
                        changed_element = id;
#ifdef DEBUG
                        to_str = ToText();
#endif
                        return true;
                    }
                }
                else
                {
                    if (with_undo)
                        document->StoreUndo(id);
                    elements->RemoveAt(p - 1, 1);
                    Normalize();
                    changed_element = id;
#ifdef DEBUG
                    to_str = ToText();
#endif
                    return true;
                }
            }
            else
            {
                p = elements->GetElementPos(before_state.id);
                if (p > 0 || before_state == last_state)
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
                        return el->DeleteElements(left, with_undo, changed_element);
                    }
                    if (with_undo)
                        document->StoreUndo(id);
                    elements->RemoveAt(p, 1);

                    if (elements->Count() == 0)
                        Normalize();
                    changed_element = id;
#ifdef DEBUG
                    to_str = ToText();
#endif
                    return true;
                }
                return parent->DeleteElements(left, with_undo, changed_element);
            }
        }
        else
        {
            int p = elements->GetElementPos(before_state.id);
            if (p == -1)
                p = elements->GetElementPos(document->GetParent(before_state.id)->id);
            if (p < elements->Count())
            {
                if (before_state.GetParent() != id)
                    ++p;
                auto el = elements->Get(p);
                if (el && el->CanContinueSelection())
                {
                    CaretState c;
                    if (el->GetFirstCaretState(c, nullptr))
                    {
                        caret->SetState(c);
                        el->DeleteElements(left, with_undo, changed_element);
                        changed_element = id;
#ifdef DEBUG
                        to_str = ToText();
#endif
                        return true;
                    }
                }
                else if (p < elements->Count())
                {
                    if (with_undo)
                        document->StoreUndo(id);
                    elements->RemoveAt(p, 1);
                }
            }
            else if (p >= 0)
            {
                if (with_undo)
                    document->StoreUndo(id);
                elements->RemoveAt(p, 1);
            }

            Normalize();
        }

        changed_element = id;

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
                document->StoreUndo(parent->id);
            elements->RemoveAt(start, size);
            Normalize();
            changed_element = id;
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

bool Row::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    Element::ChangeStringFormat(format, with_undo, changed_element);
    
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
            el->ChangeStringFormat(format, with_undo, changed_element);
        }
    }

    changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Row::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    if (!select)
        return GetFirstCaretState(caret_state, nullptr);
    if (!GetFirstCaretState(caret_state, nullptr))
        return false;
    CaretState c = caret->GetCaretState();
    int p = caret_state.GetPosInElement(id);
    select->Add(id, p, c.GetPosInElement(id));
    return true;
}

bool Row::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    if (!select)
        return GetLastCaretState(caret_state, nullptr);
    if (!GetLastCaretState(caret_state, nullptr))
        return false;
    CaretState c = caret->GetCaretState();
    int p1 = c.GetPosInElement(id);
    int p2 = caret_state.GetPosInElement(id);
    ElementSelection s;
    if (!select->Has(GetChild(id, p1), s))
        select->Add(id, p1, 1);
    if (caret_state.last_pos && p2 - p1 > 1 && p2 <= elements->Count())
        select->Add(id, p1 + 1, p2 - p1 - 1);
    else if (p2 < elements->Count() && p2 - p1 > 0)
        select->Add(id, p1 + 1, p2 - p1);
    return true;
}

bool Row::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (!select)
        return Element::GetTopCaretState(x, y, caret_state, select);
    
    CaretState next, last;
    if (y < GetAbsoluteRect().GetBottom())
    {
        if (select)
        {
            //select this row from start until caret pos
            if (GetFirstCaretState(next, nullptr))
            {
                last = caret->GetCaretState();
                while (next < last)
                {
                    ElementPtr el = document->GetElement(next.id);
                    if (!el || !el->GetRightCaretState(next, select))
                        break;
                }
                GetFirstCaretState(next, nullptr);
                caret_state = next;
                caret->SetState(next, false);
            }
        }
        return parent->GetTopCaretState(x, y, caret_state, select);
    }

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

    GetLastCaretState(next, nullptr);
    while (caret_state < next)
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

    CaretState next, last;
    if (y > GetAbsoluteRect().top)
    {
        if (select)
        {
            //select this row from caret pos until the end
            if (GetLastCaretState(last, nullptr))
            {
                next = caret->GetCaretState();
                while (next < last)
                {
                    ElementPtr el = document->GetElement(next.id);
                    if (!el || !el->GetRightCaretState(next, select))
                        break;
                }
                caret_state = next;
                caret->SetState(next, false);
            }
        }
        return parent->GetBottomCaretState(x, y, caret_state, select);
    }

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

    GetFirstCaretState(next, nullptr);
    while (next < caret_state)
    {
        ElementPtr el = document->GetElement(next.id);
        if (!el || !el->GetRightCaretState(next, select))
            break;
    }

    caret_state = next;

    return true;
}

bool Row::GetNearestCaretState(const int x, const int y, CaretState& caret_state)
{
    int min_dist = std::numeric_limits<int>::max();
    int dist;
    ElementPtr el;
    CaretState next, last;
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto _el = elements->Get(i);
        if (!_el->GetFirstCaretState(next, nullptr) || !_el->GetLastCaretState(last, nullptr))
        {
            if (_el->HasCaretState())
            {
                Rect r = _el->GetAbsoluteRect();
                if (r.IsPointInside(x, y))
                {
                    caret_state.SetState(_el->id);
                    return true;
                }
            }
            continue;
        }
        Rect r1 = document->GetCaretRect(next);
        Rect r2 = document->GetCaretRect(last);
        Rect r{r1.left, r1.top, r2.left - r1.left + r2.width, r2.top - r1.top + r2.height};
        if (r.IsPointInside(x, y))
        {
            min_dist = 0;
            el = _el;
            break;
        }
        else
        {
            dist = r1.DistToPoint(x, y);
            if (dist < min_dist)
            {
                min_dist = dist;
                el = _el;
            }
            dist = r2.DistToPoint(x, y);
            if (dist < min_dist)
            {
                min_dist = dist;
                el = _el;
            }
        }
    }

    if (min_dist > 0)
    {
        int dist1 = std::numeric_limits<int>::max(), dist2 = std::numeric_limits<int>::max();
        if (GetFirstCaretState(next, nullptr))
        {
            Rect r = document->GetCaretRect(next);
            dist1 = r.DistToPoint(x, y);
        }
        if (GetLastCaretState(last, nullptr))
        {
            Rect r = document->GetCaretRect(last);
            dist2 = r.DistToPoint(x, y);
        }
        if (dist1 < min_dist && dist1 < dist2)
        {
            caret_state = next;
            return true;
        }
        if (dist2 < min_dist)
        {
            caret_state = last;
            return true;
        }
    }

    if (!el)
    {
        if (!GetFirstCaretState(next, nullptr) || !GetLastCaretState(last, nullptr))
            return false;
        Rect r = document->GetCaretRect(next);
        min_dist = r.DistToPoint(x, y);
        r = document->GetCaretRect(last);
        dist = r.DistToPoint(x, y);
        if (dist < min_dist)
            caret_state = last;
        else
            caret_state = next;
        return true;
    }
    return el->GetNearestCaretState(x, y, caret_state);
}

Rect Row::GetCaretRect(const uint pos) const
{
    if (pos > 0 && pos == elements->Count() && elements->Get(pos - 1)->type == ElementType::CODE_BLOCK)
    {
        Rect& rect = elements->Get(pos - 1)->rect;
        return Rect{rect.GetRight() + 1, rect.top - 1, 2, rect.height + 2};
    }
    return Element::GetCaretRect(pos);
}

void Row::DrawCaret(const uint pos) const
{
    if (pos > 0 && pos == elements->Count() && elements->Get(pos - 1)->type == ElementType::CODE_BLOCK)
    {
        Rect r = GetAbsoluteRect(GetCaretRect(pos));
        window->DrawLine(r.GetRight() - 1, r.top + 1, r.GetRight() - 1, r.GetBottom() - 2, Color::Black());
        return;
    }
    return Element::DrawCaret(pos);
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

void Row::Align(ParagraphFormat::Alignment alignment)
{
    int cx = 0;
    int left_m, top_m, right_m, bottom_m;
    ParagraphFormatPtr format = ((Paragraph*)parent)->format;
    int page_width = ((Text*)parent->parent)->page_width;
    int line_width = page_width - format->indent_before - format->indent_after;

    switch (alignment)
    {
    case ParagraphFormat::Alignment::Left:
        break;
    case ParagraphFormat::Alignment::Right:
        {
            int pos = line_width;
            for (int i = elements->Count() - 1; i >= 0; --i)
            {
                auto el = elements->Get(i);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                el->rect.Move(line_width - el->rect.width - cx - left_m, 0);
                cx += el->rect.width + left_m + right_m;
            }
        }
        break;
    case ParagraphFormat::Alignment::Center:
        {
            int w = 0;
            for (int i = 0; i < elements->Count(); ++i)
            {
                auto el = elements->Get(i);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                w += el->rect.width + left_m + right_m;
            }
            cx = line_width / 2 - w / 2;
            for (int i = 0; i < elements->Count(); ++i)
            {
                auto el = elements->Get(i);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                el->rect.Move(cx + left_m, 0);
                cx += el->rect.width + left_m + right_m;
            }
        }
        break;
    case ParagraphFormat::Alignment::Justify:
        if (yutovo::GetChildPos(id) != parent->elements->Count() - 1)
        {
            std::map<int, int> elements_spaces;
            int all_spaces = 0;
            int w = 0;
            for (int i = 0; i < elements->Count(); ++i)
            {
                auto el = elements->Get(i);
                el->GetMargin(left_m, top_m, right_m, bottom_m);
                if (el->type == ElementType::STRING) //only strings can be stretched
                {
                    std::u32string s = el->ToText();
                    int spaces = std::count_if(s.begin(), s.end(),
                        [](char32_t c)
                        {
                            return std::isspace(c);
                        });
                    all_spaces += spaces;
                    elements_spaces[i] = spaces;
                    w += el->rect.width + left_m + right_m;
                }
                else
                    w += el->rect.width + left_m + right_m;
            }

            if (elements_spaces.empty() || all_spaces == 0)
                return;
            else
            {
                float k = (line_width - w) / all_spaces;
                if (k > 0)
                {
                    for (auto s : elements_spaces)
                    {
                        auto el = elements->Get(s.first);
                        ((String*)el.get())->SetStretchWidth(k * s.second); //stretch the string proportionaly
                    }
                    UpdateRect(true);
                }
                for (int i = 0; i < elements->Count(); ++i)
                {
                    auto el = elements->Get(i);
                    el->GetMargin(left_m, top_m, right_m, bottom_m);
                    el->rect.Move(cx + left_m, 0);
                    cx += el->rect.width + left_m + right_m;
                }
            }
        }
        break;
    }
}

}
