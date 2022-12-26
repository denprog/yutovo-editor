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
    caret(document->caret),
    selection(&document->selection),
    elements(new Elements(this))
{
}

Element::Element(Element* _parent) :
    parent(_parent),
    document(parent ? parent->document : nullptr), //parent == null when pasting from clipboard
    window(document ? document->window : nullptr),
    caret(document ? document->caret : nullptr),
    selection(document ? &document->selection : nullptr),
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
    dont_normalize(source.dont_normalize),
    caret(document->caret),
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

bool Element::Copy(std::vector<ElementPtr>& copy)
{
    uint start, size;
    if (!selection->Has(id, start, size))
        return false;
    if (start == 0 && size == elements->Count())
    {
        copy.push_back(ElementPtr(Clone()));
        return true;
    }
    for (uint i = start; i < start + size; ++i)
        copy.push_back(ElementPtr(elements->Get(i)->Clone()));
    return true;
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

void Element::Normalize(bool with_undo)
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->Normalize(with_undo);
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

bool Element::ChangeStringFormat(const StringFormatPtr format, bool with_undo)
{
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (!el->ChangeStringFormat(format, with_undo))
            return false;
    }
    return true;
}

bool Element::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo)
{
    if (!parent)
        return false;
    return parent->ChangeParagraphFormat(format, with_undo);
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

void Element::UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format)
{
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->UpdateStringFormat(base_format, new_format);
}

bool Element::AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo)
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
        next.SetState(id);
    if (!GetLastCaretState(last, select))
        last.SetState(id);
    
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
        next.SetState(id);
    if (!GetLastCaretState(last, select))
        last.SetState(id);
    
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

bool Element::HasLastCaretState()
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

void Element::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = top = right = bottom = 0;
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

bool Element::GetElementAtCoords(const int x, const int y, ElementId& _id)
{
    Rect r = GetAbsoluteRect();
    if (!r.IsPointInside(x, y))
        return false;
    
    //look in the child elements
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (el->GetElementAtCoords(x, y, _id))
            return true;
    }
    return false;
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

void Element::ResetDontNormalize()
{
    dont_normalize = false;
    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->dont_normalize = false;
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

FormulaFormatPtr Element::GetFormulaFormat() const
{
    assert(parent);
    return parent->GetFormulaFormat();
}

//Elements

Elements::Elements(Element* _parent) :
    parent(_parent),
    caret(parent->document ? parent->document->caret : nullptr),
    selection(parent->document ? &parent->document->selection : nullptr)
{
}

Elements::Elements(const Elements& source) :
    parent(source.parent),
    caret(parent->document->caret),
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

void Elements::Clone(std::vector<ElementPtr>& _elements, const uint start, const uint size)
{
    for (int i = start; i < start + size; ++i)
        _elements.push_back(elements[i]);
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

ElementPtr Elements::Get(ElementId id)
{
    auto it = std::find_if(elements.begin(), elements.end(), 
        [id](ElementPtr& el)
        {
            return el->id == id;
        });
    if (it == elements.end())
        return nullptr;
    return *it;
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

//Find child in the id and get its position
int Elements::GetChildPos(ElementId id)
{
    if (parent->id.size() > id.size())
        return -1;
    ElementId _id(id);
    _id.erase(_id.begin() + parent->id.size() + 1, _id.end());

    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (GetElementId(i) == _id)
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
        caret->SetState(element->id, p, true);
    if (!s.IsEmpty())
        selection->Add(element, s.start, s.size);
}

void Elements::Remove(const ElementPtr element)
{
    parent->elements->RemoveAt(element->parent->elements->GetElementPos(element->id), 1);
}

void Elements::Remove(const ElementId id)
{
    RemoveAt(GetElementPos(id), 1);
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
        if (pos == Count() && elements[pos - 1]->HasLastCaretState())
        {
            caret->SetState(parent->id, pos);
        }
        else
        {
            ElementPtr el = Get(pos < Count() ? pos : pos - 1);
            if (el->HasCaretState())
            {
                caret->SetState(el->id);
            }
            else
            {
                CaretState s;
                if (pos < Count())
                    el->GetFirstCaretState(s, nullptr);
                else
                    el->GetLastCaretState(s, nullptr);
                caret->SetState(s);
            }
        }
    }
}

void Elements::Move(const ElementPtr element, const uint pos)
{
    Insert(ElementPtr(element->Clone()), pos);
    element->parent->elements->Remove(element);

#ifdef DEBUG
    parent->to_str = parent->ToText();
    element->parent->to_str = element->parent->ToText();
#endif
}

void Elements::Move(const Elements& _elements, const uint pos)
{
    for (int i = 0; i < _elements.Count(); ++i)
        Move(_elements.elements[i], pos + i);
}

void Elements::Clear()
{
    elements.clear();
}

uint Elements::Count() const
{
    return elements.size();
}

Rect Elements::GetCaretRect(const uint pos) const
{
    Rect r;
    if (pos == Count())
    {
        Rect& rect = elements[pos - 1]->rect;
        r = Rect{rect.GetRight() - 1, rect.top - 1, 2, rect.height + 2}; //for last caret state draw one line
    }
    else
    {
        r = elements[pos]->rect; //draw caret of two lines
        r.left -= 3;
        r.width += 6;
        r.top -= 1;
        r.height += 6;
    }
    return r;
}

void Elements::DrawCaret(const uint pos) const
{
    if (pos == Count())
    {
        Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
        parent->window->DrawLine(r.GetRight() - 1, r.top + 1, r.GetRight() - 1, r.GetBottom() - 2, Color::Black());
    }
    else
    {
        Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
        parent->window->DrawLine(r.left + 1, r.top + 1, r.left + 1, r.GetBottom() - 2, Color::Black());
        parent->window->DrawLine(r.left + 1, r.GetBottom() - 2, r.GetRight() - 2, r.GetBottom() - 2, Color::Black());
    }
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
    if (elements[elements.size() - 1]->HasLastCaretState())
    {
        caret_state.SetState(parent->id, Count());
        return true;
    }
    return elements[elements.size() - 1]->GetLastCaretState(caret_state, select);
}

bool Elements::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
    if (p < 0)
    {
        CaretState c;
        if (GetLastCaretState(c, nullptr) && caret_state == c)
            p = Count();
        else
            return false;
    }
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
        else if (Count() > p + 1 && elements[p + 1]->id != caret_state.id && elements[p + 1]->HasCaretState())
        {
            caret_state.SetState(Get(p + 1));
            if (select)
                select->Add(parent->id, p + 1, 1);
            return true;
        }
        else if (elements[p]->GetLastCaretState(caret_state, select))
            return true;
    }

    if (Count() > 0 && elements[0]->HasCaretState())
    {
        if (caret_state != CaretState(Get(0)->id))
        {
            caret_state.SetState(Get(0));
            if (select)
                select->Add(parent->id, 0, 1);
            return true;
        }
    }
    return false;
}

bool Elements::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
    if (p < 0)
        return false;
    
    if (parent->document->GetParent(caret_state.id)->id == parent->id) //try to enter into this element
    {
        if (elements[p]->GetFirstCaretState(caret_state, select))
        {
            if (select)
                select->Add(parent->id, p, 1);
            return true;
        }
    }

    while (++p < Count()) //find next nearest caret state
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

    if (elements[p - 1]->HasLastCaretState())
    {
        caret_state.SetState(parent->id, p);
        return true;
    }
    return false;
}

bool Elements::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    int p = GetChildPos(caret_state.id);
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
    int p = GetChildPos(caret_state.id);
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
