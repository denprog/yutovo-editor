#include "element.h"
#include "document.h"
#include <assert.h>
#include <limits>

namespace yutovo
{

//Element

Element::Element(Element* _parent) :
    parent(_parent),
    elements(new Elements(this))
{
    if (parent)
    {
        document = parent->document;
        window = document->window;
    }
}

Element::Element(const Element& source) :
    document(source.document),
    window(source.window),
    type(source.type),
    parent(source.parent),
    id(source.id),
    editable(source.editable)
{
    elements.reset(new Elements(*source.elements)); //deep copy
}

Element::~Element()
{
}

Element* Element::Clone()
{
    return new Element(this);
}

void Element::Draw(const Selections& selections) const
{
    elements->Draw(selections);
}

void Element::Remake(CaretState& caret_state, bool with_elements)
{
    if (with_elements)
        elements->Remake(caret_state);
    UpdateRect();
}

bool Element::InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    return false;
}

bool Element::DeleteElements(const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    return false;
}

bool Element::CanSplit(const uint max_left_width)
{
    return false;
}

bool Element::Split(const uint max_left_width, CaretState& caret_state)
{
    return false;
}

bool Element::CanMerge(const ElementPtr with_element)
{
    return false;
}

bool Element::Merge(const ElementPtr with_element, CaretState& caret_state)
{
    return false;
}

bool Element::GetFirstCaretState(CaretState& res)
{
    return elements->GetFirstCaretState(res);
}

bool Element::GetLastCaretState(CaretState& res)
{
    return elements->GetLastCaretState(res);
}

bool Element::GetLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    if (before_state.IsInsideElement(id))
    {
        if (elements->GetLeftCaretState(before_state, after_state, selection))
            return true;
        if (parent)
            return parent->GetLeftCaretState(before_state, after_state, selection);
    }
    return false;
}

bool Element::GetRightCaretState(const CaretState& caret_state, CaretState& res, bool selection)
{
    if (elements->GetRightCaretState(caret_state, res, selection))
        return true;
    if (parent)
        return parent->GetRightCaretState(caret_state, res, selection);
    return false;
}

bool Element::GetTopCaretState(const int x, const int y, CaretState& res, bool selection)
{
    if (y < GetAbsoluteRect().GetBottom())
    {
        if (parent)
            return parent->GetTopCaretState(x, y, res, selection);
        return false;
    }

    CaretState next, last;
    if (!GetFirstCaretState(next))
        return false;
    if (!GetLastCaretState(last))
        return false;
    
    res = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        CaretState c = next;
        ElementPtr el = document->GetElement(c.id);
        if (!el->GetRightCaretState(c, next, selection))
            break;
        
        r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            res = next;
        }
    }

    return true;
}

bool Element::GetBottomCaretState(const int x, const int y, CaretState& res, bool selection)
{
    if (y > GetAbsoluteRect().top)
    {
        if (parent)
            return parent->GetBottomCaretState(x, y, res, selection);
        return false;
    }

    CaretState next, last;
    if (!GetFirstCaretState(next))
        return false;
    if (!GetLastCaretState(last))
        return false;
    
    res = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        CaretState c = next;
        ElementPtr el = document->GetElement(c.id);
        if (!el->GetRightCaretState(c, next, selection))
            break;
        
        Rect r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            res = next;
        }
    }

    return true;
}

bool Element::GetBeginCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    if (parent)
        return parent->GetBeginCaretState(before_state, after_state, selection);
    return false;
}

bool Element::GetEndCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    if (parent)
        return parent->GetEndCaretState(before_state, after_state, selection);
    return false;
}

bool Element::HasCaretState()
{
    return false;
}

bool Element::CanContinueSelection()
{
    return false;
}

Rect Element::GetCaretRect(const uint pos) const
{
    return elements->GetCaretRect(pos);
}

void Element::DrawCaret(const uint pos) const
{
    elements->DrawCaret(pos);
}

std::string Element::ToHtml()
{
    return elements->ToHtml();
}

std::string Element::ToText()
{
    return elements->ToText();
}

void Element::UpdateRect()
{
    //get rect relatively to the parent
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto element = elements->Get(i);
        if (element->rect.GetRight() > right)
            right = element->rect.GetRight();
        if (element->rect.GetBottom() > bottom)
            bottom = element->rect.GetBottom();
    }
    rect.SetRect(0, 0, right, bottom);
}

Element* Element::GetElementInPos(const ElementId& _id, const uint pos)
{
    if (pos == 0)
        return this;
    
    ElementId i(_id);
    i.erase(i.begin());
    return elements->Get(i[0])->GetElementInPos(i, pos - 1);
}

// ElementPtr Element::GetElement(const ElementId& _id)
// {
//     if (_id.size() == 2)
//         return elements->Get(_id[1]);
    
//     ElementId i(_id);
//     i.erase(i.begin());
//     return elements->Get(i[0])->GetElement(i);
// }

// Element* Element::GetParent(const ElementId& _id)
// {
//     if (_id.size() == 2)
//         return this;
    
//     ElementId i(_id);
//     i.erase(i.begin());
//     return elements->Get(i[0])->GetParent(i);
// }

void Element::AddElement(ElementPtr element)
{
    elements->Add(element);
}

// void Element::RemoveElement(uint pos, CaretState& caret_state)
// {
//     if (caret_state.IsInsideElement(elements->Get(pos)->id))
//     {
//     }
//     else
//     {
//         for (int i = pos + 1; i < elements->Count(); ++i)
//         {
//             ElementPtr el = elements->Get(i);
//             if (caret_state.IsInsideElement(el->id))
//             {
//                 int p = caret_state.GetStatePos(el->id);
//                 --caret_state.id[p];
//             }
//         }
//     }
//     elements->RemoveAt(pos, 1);
// }

// void Element::MoveElement(ElementPtr from, uint from_pos, ElementPtr to, uint to_pos, CaretState& caret_state)
// {
//     ElementPtr el = from->elements->Get(from_pos);
//     ElementId tail_id;
//     if (caret_state.IsInsideElement(el->id))
//         tail_id = caret_state.GetTailId(el->id.size());
//     else
//     {
//         for (int i = from_pos + 1; i < elements->Count(); ++i)
//         {
//             ElementPtr el = elements->Get(i);
//             if (caret_state.IsInsideElement(el->id))
//             {
//                 int p = caret_state.GetStatePos(el->id);
//                 ++caret_state.id[p];
//             }
//         }
//     }

//     to->elements->Insert(el, to_pos);
//     from->elements->RemoveAt(from_pos, 1);

//     if (!tail_id.empty())
//         caret_state.SetState(el->id, tail_id);
// }

uint Element::GetChildPos(const Element* element)
{
    for (size_t i = 0; i < elements->Count(); ++i)
    {
        if (elements->Get(i)->id == element->id)
            return i;
    }
    assert(false);
}

Rect Element::GetAbsoluteRect(const Rect& _rect) const
{
    Rect r = _rect;
    r.left += rect.left;
    r.top += rect.top;

    Element* p = parent;
    while (p)
    {
        r.left += p->rect.left;
        r.top += p->rect.top;
        p = p->parent;
    }
    return r;
}

Rect Element::GetAbsoluteRect() const
{
    Rect r = rect;

    Element* p = parent;
    while (p)
    {
        r.left += p->rect.left;
        r.top += p->rect.top;
        p = p->parent;
    }
    return r;
}

Point Element::GetAbsolutePoint(const Point& point) const
{
    if (parent)
    {
        Point parent_point = parent->GetAbsolutePoint(point);
        Point p = point;
        p.x += parent_point.x + rect.left;
        p.y += parent_point.y + rect.top;
        return p;
    }
    return point;
}

//Elements

Elements::Elements(Element* _parent) :
    parent(_parent)
{
}

Elements::Elements(const Elements& source) :
    parent(source.parent)
{
    for (auto& el : source.elements)
        elements.push_back(ElementPtr(el->Clone())); //deep copy
}

ElementPtr Elements::operator[](const int pos)
{
    return elements[pos];
}

void Elements::Draw(const Selections& selections) const
{
    for (auto& element : elements)
        element->Draw(selections);
}

void Elements::Remake(CaretState& caret_state)
{
    for (auto& element : elements)
        element->Remake(caret_state, true);
}

ElementPtr Elements::Get(uint pos)
{
    assert(elements.size() > pos);
    return elements[pos];
}

ElementId Elements::GetElementId(uint pos)
{
    assert(elements.size() > pos);
    return elements[pos]->id;
}

int Elements::GetElementPos(ElementId id)
{
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (GetElementId(i) == id)
            return i;
    }
    return -1;
}

void Elements::Add(ElementPtr element)
{
    elements.push_back(ElementPtr(element));

    element->parent = parent;
    element->document = parent->document;
    element->window = parent->window;
    //set id
    UpdateIds();
}

void Elements::Insert(ElementPtr element, const uint pos)
{
    elements.insert(elements.begin() + pos, element);
    element->parent = parent;
    element->document = parent->document;
    element->window = parent->window;
    UpdateIds(); //set id
}

void Elements::Insert(ElementPtr element, const uint pos, CaretState& caret_state)
{
    int p = -1;
    if (caret_state.IsInsideElement(element->id))
        p = caret_state.GetPos();
    int cs_pos = -1;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (caret_state.IsInsideElement(elements[i]->id))
        {
            cs_pos = i;
            break;
        }
    }
    
    elements.insert(elements.begin() + pos, element);

    element->parent = parent;
    element->document = parent->document;
    element->window = parent->window;
    UpdateIds(); //set id

    if (p != -1)
        caret_state.SetState(element->id, p);
    if (cs_pos != -1)
        caret_state.SetState(elements[cs_pos + 1]->id, caret_state.GetPos());
}

void Elements::Remove(const ElementPtr element)
{
    for (auto it = elements.begin(); it != elements.end(); ++it)
    {
        if ((*it)->id == element->id)
        {
            elements.erase(it);
            UpdateIds();
            return;
        }
    }    
}

void Elements::RemoveAt(const uint pos, const int size, CaretState& caret_state)
{
    int cs_pos = -1;
    for (size_t i = pos + size; i < elements.size(); ++i)
    {
        if (caret_state.IsInsideElement(elements[i]->id))
        {
            cs_pos = i;
            break;
        }
    }

    elements.erase(elements.begin() + pos, elements.begin() + pos + size);
    UpdateIds();

    if (cs_pos != -1)
        caret_state.SetState(elements[cs_pos - size]->id, caret_state.GetPos());
}

void Elements::RemoveAt(const uint pos, const int size)
{
    elements.erase(elements.begin() + pos, elements.begin() + pos + size);
    UpdateIds();
}

void Elements::Clear()
{
    elements.clear();
}

uint Elements::Count()
{
    return elements.size();
}

Rect Elements::GetCaretRect(const uint pos) const
{
    return Rect();
}

void Elements::DrawCaret(const uint pos) const
{
}

Rect Elements::GetRect()
{
    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();
    for (auto& element : elements)
    {
        if (element->rect.left < left)
            left = element->rect.left;
        if (element->rect.top < top)
            top = element->rect.top;
        if (element->rect.GetRight() > right)
            right = element->rect.GetRight();
        if (element->rect.GetBottom() > bottom)
            bottom = element->rect.GetBottom();
    }
    return Rect{left, top, right - left, bottom - top};
}

bool Elements::GetFirstCaretState(CaretState& res)
{
    if (elements.empty())
        return false;
    if (elements[0]->HasCaretState())
    {
        res.id = GetElementId(0);
        return true;
    }
    return elements[0]->GetFirstCaretState(res);
}

bool Elements::GetLastCaretState(CaretState& res)
{
    if (elements.empty())
        return false;
    return elements[elements.size() - 1]->GetLastCaretState(res);
}

bool Elements::GetLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    uint p = before_state.GetElementPos(parent->id);
    while (p-- > 0)
    {
        if (selection)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetLastCaretState(after_state))
                {
                    after_state.selections = before_state.selections;
                    CaretState c = after_state;
                    ElementPtr el = parent->document->GetElement(c.id);
                    if (el->GetLeftCaretState(c, after_state, selection))
                        return true;
                }
            }
        }
        else if (elements[p]->GetLastCaretState(after_state))
            return true;
        if (elements[p]->HasCaretState())
        {
            after_state.SetState(GetElementId(p), before_state.selections);
            if (selection)
                after_state.selections.AddSelection(parent->id, p, 1);
            return true;
        }
    }
    return false;
}

bool Elements::GetRightCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    uint p = before_state.GetElementPos(parent->id);
    while (++p < Count())
    {
        if (elements[p]->HasCaretState())
        {
            after_state.SetState(GetElementId(p), before_state.selections);
            if (selection)
                after_state.selections.AddSelection(parent->id, p, 1);
            return true;
        }
        if (selection)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetFirstCaretState(after_state))
                {
                    after_state.selections = before_state.selections;
                    CaretState c = after_state;
                    ElementPtr el = parent->document->GetElement(c.id);
                    if (el->GetRightCaretState(c, after_state, selection))
                        return true;
                }
            }
        }
        else if (elements[p]->GetFirstCaretState(after_state))
        {
            if (selection)
                after_state.selections.AddSelection(parent->id, p, 1);
            return true;
        }
    }
    return false;
}

bool Elements::HasLastCaretState()
{
    return false;
}

std::string Elements::ToHtml()
{
    std::string html;
    for (auto it = elements.begin(); it != elements.end(); ++it)
        html += (*it)->ToHtml();
    return html;
}

std::string Elements::ToText()
{
    std::string t;
    for (auto it = elements.begin(); it != elements.end(); ++it)
        t += (*it)->ToText();
    return t;
}

void Elements::UpdateIds()
{
    for (size_t i = 0; i < elements.size(); ++i)
    {
        auto& el = elements[i];
        el->id = el->parent->id;
        el->id.push_back(i);
        el->elements->UpdateIds();
    }
}

}
