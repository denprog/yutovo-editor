#include "selection.h"
#include "document.h"
#include "util.h"

namespace yutovo
{

//ElementSelection

bool ElementSelection::operator==(const ElementSelection& s) const
{
    return element == s.element && start == s.start && size == s.size;
}

bool ElementSelection::operator!=(const ElementSelection& s) const
{
    return element != s.element || start != s.start || size != s.size;
}

bool ElementSelection::operator<(const ElementSelection& s) const
{
    std::vector<ElementId> ids;
    ElementId s_id1 = element->id;
    s_id1.push_back(start);
    ids.push_back(s_id1);
    ElementId s_id2 = s.element->id;
    s_id2.push_back(s.start);
    ids.push_back(s_id2);
    ElementId _id = GetCommonParent(ids);
    return s_id1[_id.size()] < s_id2[_id.size()]; //get position in common parent
}

bool ElementSelection::IsEmpty() const
{
    return element == nullptr;
}

//ElementSelectionState

bool ElementSelectionState::operator==(const ElementSelectionState& s) const
{
    return id == s.id && start == s.start && size == s.size;
}

bool ElementSelectionState::operator!=(const ElementSelectionState& s) const
{
    return id != s.id || start != s.start || size != s.size;
}

//SelectionState

SelectionState::SelectionState(const ElementId id, uint start, uint size)
{
    Add(id, start, size);
}

bool SelectionState::operator==(const SelectionState& compare) const
{
    if (state.size() != compare.state.size())
        return false;
    for (const ElementSelectionState& s : compare.state)
    {
        auto it = std::find_if(state.begin(), state.end(), 
            [s](auto& t)
            {
                return t.id == s.id && t.start == s.start && t.size == s.size;
            });
        if (it == state.end())
            return false;
    }
    return true;
}

bool SelectionState::operator!=(const SelectionState& compare) const
{
    return !(state == compare.state);
}

void SelectionState::Add(const ElementId id, uint start, uint size)
{
    state.push_back(ElementSelectionState{id, start, size});
}

void SelectionState::Add(const ElementSelectionState& s)
{
    state.push_back(s);
}

void SelectionState::Merge(const SelectionState& s)
{
    for (auto& t : s.state)
        Add(t);
}

ElementId SelectionState::GetCommonElement() const
{
    std::vector<ElementId> ids;
    for (int i = 0; i < state.size(); ++i)
        ids.push_back(state[i].id);
    return GetCommonParent(ids);
}

bool SelectionState::IsEmpty() const
{
    return state.empty();
}

#ifdef DEBUG
std::string SelectionState::ToString() const
{
    std::string res;
    for (size_t i = 0; i < state.size(); ++i)
    {
        const ElementSelectionState& s = state[i];
        res += "[{" + IdToString(s.id) + "}," + std::to_string(s.start) + "," + std::to_string(s.size) + "]";
        if (i < state.size() - 1)
            res += ",";
    }
    return res;
}
#endif

//Selection

Selection::Selection(Document* _document) :
    document(_document)
{
}

bool Selection::operator==(const Selection& s) const
{
    if (selection.size() != s.selection.size())
        return false;
    for (const ElementSelection& s : s.selection)
    {
        auto it = std::find_if(selection.begin(), selection.end(), 
            [s](auto& t)
            {
                return t.element->id == s.element->id;
            });
        if (it == selection.end())
            return false;
        if (*it != s)
            return false;
    }
    return true;
}

bool Selection::operator!=(const Selection& s) const
{
    return !(selection == s.selection);
}

void Selection::Set(SelectionState& state)
{
    selection.clear();
    for (ElementSelectionState& s : state.state)
        Add(s.id, s.start, s.size);
}

void Selection::Add(const ElementPtr element, uint start, uint size)
{
    if (!element)
        return;
    auto it = std::find_if(selection.begin(), selection.end(), 
        [element](ElementSelection& s)
        {
            return s.element == element;
        });
    if (it == selection.end())
    {
        selection.emplace_back(ElementSelection{element, start, size}); //add new selection
    }
    else
    {
        //append or substract the selection or add a new one
        if (start < it->start && start + size >= it->start)
        {
            it->size = it->start - start + it->size;
            it->start = start;
        }
        else if (start < it->start && start + size > it->start + it->size)
        {
            it->start = start;
            it->size = size;
        }
        else if (start == it->start && size <= it->size)
        {
            it->start = start + size;
            it->size -= size;
        }
        else if (start > it->start && start < it->start + it->size && start + size == it->start + it->size)
        {
            it->size -= size;
        }
        else if (start > it->start && start + size < it->start + it->size)
        {
            it->size = start - it->start;
            selection.emplace_back(ElementSelection{element, start + size, it->start + it->size - start - size});
        }
        else if (start >= it->start && start <= it->start + it->size && start + size > it->start + it->size)
        {
            it->size = start - it->start + size;
        }
        else
        {
            selection.emplace_back(ElementSelection{element, start, size});
        }

        if (it->size == 0)
            selection.erase(it);
    }
    std::sort(selection.begin(), selection.end());
}

void Selection::Add(const ElementId id, uint start, uint size)
{
    Add(document->GetElement(id), start, size);
}

void Selection::Remove(const ElementId id, uint start, uint size)
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](auto& s)
        {
            return s.element->id == id;
        });
    if (it == selection.end())
        return;
    
    ElementSelection& s = *it;
    if (s.start >= start && s.start + s.size <= start + size)
        selection.erase(it);
    else if (s.start < start && s.start + s.size > start)
        s.size = start - s.start;
    else if (s.start > start && s.start < start + size)
        s.start = start + size;
}

void Selection::InsertElement(const ElementId id)
{
    if (selection.empty() || id.empty())
        return;
    ElementId p_id = GetParent(id);
    auto it = std::find_if(selection.begin(), selection.end(), 
        [p_id](auto& s)
        {
            return s.element->id == p_id;
        });
    if (it == selection.end())
        return;
    
    auto& s = *it;
    if (s.start >= GetChildPos(id))
        ++s.start;
}

void Selection::RemoveElement(const ElementId id)
{
    if (selection.empty() || id.empty())
        return;
    ElementId p_id = GetParent(id);
    auto it = std::find_if(selection.begin(), selection.end(), 
        [p_id](auto& s)
        {
            return s.element->id == p_id;
        });
    if (it == selection.end())
        return;
    
    auto& s = *it;
    if (s.start >= GetChildPos(id))
        --s.start;
}

bool Selection::Has(const ElementPtr element, uint& start, uint& size) const
{
    return Has(element->id, start, size);
}

bool Selection::Has(const ElementId id, uint& start, uint& size) const
{
    ElementSelection s;
    if (!Has(id, s))
        return false;
    start = s.start;
    size = s.size;
    return true;
}

bool Selection::Has(const ElementId id, ElementSelection& s) const
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](auto& s)
        {
            if (s.element->id == id)
                return true;
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                if (IsChild(GetChild(s.element->id, i), id))
                    return true;
            }
            return false;
        });
    if (it == selection.end())
        return false;
    if (it->element->id == id)
        s = *it;
    else
    {
        //parent element is selected, so select the whole this element
        auto el = document->GetElement(id);
        if (!el)
            return false;
        s.start = 0;
        s.size = el->elements->Count();
    }
    return true;
}

bool Selection::HasChild(const ElementId id, ElementSelection& s) const
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](auto& s)
        {
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                if (IsChild(id, s.element->id) || id == s.element->id)
                    return true;
            }
            return false;
        });
    if (it == selection.end())
        return false;
    s = *it;
    return true;
}

bool Selection::IsSelected(const ElementId id) const
{
    auto it = std::find_if(selection.begin(), selection.end(), 
        [id](const ElementSelection& s)
        {
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                ElementId ch = GetChild(s.element->id, i);
                if (ch == id || IsChild(ch, id))
                    return true;
            }
            return false;
        });
    return it != selection.end();
}

void Selection::Clear()
{
    selection.clear();
}

bool Selection::IsEmpty() const
{
    return selection.empty();
}

SelectionState Selection::GetState() const
{
    SelectionState state;
    for (auto& s : selection)
        state.state.push_back(ElementSelectionState{s.element->id, s.start, s.size});
    return state;
}

}
