#include "element.h"
#include "document.h"
#include "caret.h"
#include "selection.h"
#include <assert.h>
#include <limits>
#include <map>

namespace yutovo
{

//Element

Element::Element(Document* _document) :
    parent(nullptr),
    document(_document),
    window(_document->window),
    caret(&document->caret),
    selection(&document->selection),
    elements(new Elements(this))
{
}

Element::Element(Element* _parent) :
    parent(_parent),
    document(_parent->document),
    window(document->window),
    caret(&document->caret),
    selection(&document->selection),
    elements(new Elements(this))
{
}

Element::Element(const Element& source) :
    parent(source.parent),
    document(source.document),
    window(source.window),
    type(source.type),
    id(source.id),
    editable(source.editable),
    caret(&document->caret),
    selection(&document->selection)
{
    elements.reset(source.elements->Clone(this)); //deep copy

#ifdef DEBUG
    to_str = ToText();
#endif
}

Element::~Element()
{
}

void Element::Draw() const
{
    elements->Draw();
}

void Element::Remake(bool with_elements)
{
    if (with_elements)
        elements->Remake();
    UpdateRect();
}

bool Element::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    return false;
}

bool Element::DeleteElements(bool left, bool with_undo)
{
    if (selection->IsEmpty())
    {
        if ((left && caret->current_pos == 0) || (!left && caret->current_pos == elements->Count()))
            return parent->DeleteElements(left, with_undo);
        elements->RemoveAt(left ? caret->current_pos - 1 : caret->current_pos, 1);

#ifdef DEBUG
        to_str = ToText();
#endif
        return true;
    }

#ifdef DEBUG
    to_str = ToText();
#endif
    return false;
}

bool Element::Split(const uint max_left_width)
{
    return false;
}

bool Element::SplitAt(const uint pos)
{
    if (elements->Count() <= pos)
        return false;
    ElementPtr part(Create(parent));
    for (int i = pos; i < elements->Count();)
    {
        part->elements->Insert(elements->Get(i), part->elements->Count());
        elements->RemoveAt(i, 1);
    }
    return true;
}

bool Element::Merge(const ElementPtr with_element)
{
    return false;
}

bool Element::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    return elements->GetFirstCaretState(caret_state, select);
}

bool Element::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    return elements->GetLastCaretState(caret_state, select);
}

bool Element::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetLeftCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetLeftCaretState(caret_state, select);
    return false;
}

bool Element::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetRightCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetRightCaretState(caret_state, select);
    return false;
}

bool Element::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (y < GetAbsoluteRect().GetBottom())
    {
        if (parent)
            return parent->GetTopCaretState(x, y, caret_state, select);
        return false;
    }

    CaretState next, last;
    if (!GetFirstCaretState(next, select))
        return false;
    if (!GetLastCaretState(last, select))
        return false;
    
    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, select))
            break;
        
        r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    return true;
}

bool Element::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (y > GetAbsoluteRect().top)
    {
        if (parent)
            return parent->GetBottomCaretState(x, y, caret_state, select);
        return false;
    }

    CaretState next, last;
    if (!GetFirstCaretState(next, select))
        return false;
    if (!GetLastCaretState(last, select))
        return false;
    
    caret_state = next;
    Rect r = document->GetCaretRect(next);
	int min_dist = r.DistToPoint(x, y);

    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, select))
            break;
        
        Rect r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            caret_state = next;
        }
    }

    return true;
}

bool Element::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    if (parent)
        return parent->GetBeginCaretState(caret_state, select);
    return false;
}

bool Element::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    if (parent)
        return parent->GetEndCaretState(caret_state, select);
    return false;
}

bool Element::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetWordLeftCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetWordLeftCaretState(caret_state, select);
    return false;
}

bool Element::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    if (caret_state.IsInsideElement(id))
    {
        if (elements->GetWordRightCaretState(caret_state, select))
            return true;
    }
    if (parent)
        return parent->GetWordRightCaretState(caret_state, select);
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

void Element::AddElement(ElementPtr element)
{
    elements->Add(element);
}

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

ParagraphFormatPtr Element::GetParagraphFormat()
{
    assert(parent);
    return parent->GetParagraphFormat();
}

StringFormatPtr Element::GetStringFormat()
{
    assert(parent); //anybody must return string format
    return parent->GetStringFormat();
}

//Elements

Elements::Elements(Element* _parent) :
    parent(_parent),
    caret(&parent->document->caret),
    selection(&parent->document->selection)
{
}

Elements::Elements(const Elements& source) :
    parent(source.parent),
    caret(&parent->document->caret),
    selection(&parent->document->selection)
{
    for (auto& el : source.elements)
        elements.push_back(ElementPtr(el->Clone())); //deep copy
}

ElementPtr Elements::operator[](const int pos)
{
    return elements[pos];
}

Elements* Elements::Clone(Element* _parent)
{
    Elements* res = new Elements(_parent);
    for (auto el : elements)
        res->Add(ElementPtr(el->Clone()));
    return res;
}

void Elements::Draw() const
{
    for (auto& element : elements)
        element->Draw();
}

void Elements::Remake()
{
    for (auto& element : elements)
        element->Remake(true);
}

ElementPtr Elements::Get(uint pos)
{
    if (pos >= elements.size())
        return nullptr;
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
    int p = -1;
    if (caret->IsInsideElement(element->id))
        p = caret->current_pos;
    
    ElementSelection s;
    if (selection->Has(element->id, s))
        selection->Remove(element->id, s.start, s.size);
    
    elements.insert(elements.begin() + pos, element);

    element->parent = parent;
    element->document = parent->document;
    element->window = parent->window;
    UpdateIds(); //set id

    //update caret state on the new position of the element
    if (p != -1)
        caret->SetState(element->id, p);
    if (!s.IsEmpty())
        selection->Add(element, s.start, s.size);
}

void Elements::Remove(const ElementPtr element)
{
    parent->elements->RemoveAt(element->parent->elements->GetElementPos(element->id), 1);
}

void Elements::RemoveAt(const uint pos, const int size)
{
    selection->Remove(parent->id, pos, size);
    int cs_pos = -1;
    if (caret->IsInsideElement(elements[pos]->id))
        cs_pos = pos;

    elements.erase(elements.begin() + pos, elements.begin() + pos + size);
    UpdateIds();

    if (cs_pos != -1 && Count() > 0)
    {
        ElementPtr el = Get(pos < Count() ? pos : pos - 1);
        if (el->HasCaretState())
        {
            caret->SetState(el->id);
        }
        else
        {
            CaretState s;
            el->GetFirstCaretState(s, nullptr);
            caret->SetState(s);
        }
    }
}

void Elements::Move(const ElementPtr element, const uint pos)
{
    if (caret->IsInsideElement(element->id))
    {
        int cs_pos = caret->current_pos;
        element->parent->elements->Remove(element);
        Insert(element, pos);
        caret->SetState(element->id, cs_pos);
    }
    else if (element->parent->elements->Count() > 1)
    {
        int cs_pos = -1;
        auto p = element->parent;
        if (caret->IsInsideElement(p->elements->Get(1)->id))
            cs_pos = caret->current_pos;
        element->parent->elements->Remove(element);
        Insert(element, pos);
        if (cs_pos != -1)
            caret->SetState(p->elements->Get(0)->id, cs_pos);
    }
    else
    {
        element->parent->elements->Remove(element);
        Insert(element, pos);
    }

#ifdef DEBUG
    parent->to_str = parent->ToText();
    element->parent->to_str = element->parent->ToText();
#endif
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

bool Elements::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    if (elements.empty())
        return false;
    if (elements[0]->HasCaretState())
    {
        caret_state.id = GetElementId(0);
        return true;
    }
    return elements[0]->GetFirstCaretState(caret_state, select);
}

bool Elements::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    if (elements.empty())
        return false;
    return elements[elements.size() - 1]->GetLastCaretState(caret_state, select);
}

bool Elements::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetElementPos(parent->document->GetElement(caret_state.id)->id);
    if (p < 0)
        return false;
    while (p-- > 0)
    {
        if (select)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetLastCaretState(caret_state, nullptr))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetLeftCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else if (elements[p]->GetLastCaretState(caret_state, select))
            return true;
        if (elements[p]->HasCaretState())
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
    }
    return false;
}

bool Elements::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetElementPos(parent->document->GetElement(caret_state.id)->id);
    if (p < 0)
        return false;
    while (++p < Count())
    {
        if (elements[p]->HasCaretState())
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
        if (select)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetFirstCaretState(caret_state, nullptr))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetRightCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else if (elements[p]->GetFirstCaretState(caret_state, select))
        {
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
    }
    return false;
}

bool Elements::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetElementPos(parent->document->GetElement(caret_state.id)->id);
    if (p < 0)
        return false;
    while (p-- > 0)
    {
        if (select)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetLastCaretState(caret_state, select))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetWordLeftCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else
        {
            if (elements[p]->GetLastCaretState(caret_state, select))
            {
                if (elements[p]->GetWordLeftCaretState(caret_state, select))
                    return true;
            }
        }
        if (elements[p]->HasCaretState())
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
    }
    return false;
}

bool Elements::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetElementPos(parent->document->GetElement(caret_state.id)->id);
    if (p < 0)
        return false;
    while (++p < Count())
    {
        if (elements[p]->HasCaretState())
        {
            caret_state.SetState(Get(p));
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
        if (select)
        {
            if (elements[p]->CanContinueSelection())
            {
                if (elements[p]->GetFirstCaretState(caret_state, select))
                {
                    ElementPtr el = parent->document->GetParent(caret_state.id);
                    if (el->GetWordRightCaretState(caret_state, select))
                        return true;
                }
            }
        }
        else
        {
            if (elements[p]->GetFirstCaretState(caret_state, select))
            {
                if (elements[p]->GetWordRightCaretState(caret_state, select))
                {
                    if (select)
                        select->Add(parent->id, p, 1);
                    return true;
                }
            }
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
