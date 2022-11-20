#include "caret_state.h"
#include "element.h"
#include "util.h"
#include <algorithm>

namespace yutovo
{

//Selections

bool Selections::operator==(const Selections& _selections)
{
    if (selections.size() != _selections.selections.size())
        return false;
    for (const Selection& s : _selections.selections)
    {
        auto it = std::find_if(selections.begin(), selections.end(), 
            [s](auto& selection)
            {
                return selection.id == s.id;
            });
        if (it == selections.end())
            return false;
        if (*it != s)
            return false;
    }
    return true;
}

bool Selections::operator!=(const Selections& _selections)
{
    return !(selections == _selections.selections);
}

void Selections::AddSelection(const Selection& selection)
{
    selections.emplace_back(selection);
    std::sort(selections.begin(), selections.end());
}

void Selections::AddSelection(const ElementId& id, const uint pos, const uint count)
{
    auto it = std::find_if(selections.begin(), selections.end(), 
        [id](auto& selection)
        {
            return selection.id == id;
        });
    if (it == selections.end())
    {
        selections.emplace_back(Selection{id, pos, count}); //add new selection
    }
    else
    {
        //append or substract the selection or add a new one
        if (pos < it->start && pos + count <= it->start + it->size)
        {
            it->size = it->start - pos + it->size;
            it->start = pos;
        }
        else if (pos < it->start && pos + count > it->start + it->size)
        {
            it->start = pos;
            it->size = count;
        }
        else if (pos == it->start && count <= it->size)
        {
            it->start = pos + count;
            it->size -= count;
        }
        else if (pos > it->start && pos < it->start + it->size && pos + count == it->start + it->size)
        {
            it->size -= count;
        }
        else if (pos > it->start && pos + count < it->start + it->size)
        {
            it->size = pos - it->start;
            selections.emplace_back(Selection{id, pos + count, it->start + it->size - pos - count});
        }
        else if (pos >= it->start && pos <= it->start + it->size && pos + count > it->start + it->size)
        {
            it->size = pos - it->start + count;
        }
        else
        {
            selections.emplace_back(Selection{id, pos, count});
        }

        if (it->size == 0)
            selections.erase(it);
    }
    std::sort(selections.begin(), selections.end());
}

void Selections::RemoveSelection(const ElementId& id, const uint start)
{
    auto it = std::find_if(selections.begin(), selections.end(), 
        [id, start](auto& selection)
        {
            return selection.id == id && selection.start == start;
        });
    if (it != selections.end())
        selections.erase(it);
}

void Selections::ClearSelection()
{
    selections.clear();
}

void Selections::ClearSelection(const ElementId& id)
{
    auto it = std::find_if(selections.begin(), selections.end(), 
        [id](auto& selection)
        {
            return selection.id == id;
        });
    if (it != selections.end())
        selections.erase(it);
}

bool Selections::HasSelection() const
{
    return !selections.empty();
}

bool Selections::HasSelection(const ElementId& id, Selection& selection) const
{
    auto it = std::find_if(selections.begin(), selections.end(), 
        [id](auto& selection)
        {
            return selection.id == id;
        });
    if (it == selections.end())
        return false;
    
    selection = *it;
    return true;
}

bool Selections::HasSelection(const ElementId& id, uint& start, uint& size) const
{
    Selection s;
    if (!HasSelection(id, s))
        return false;
    start = s.start;
    size = s.size;
    return true;
}

bool Selections::IsEmpty() const
{
    return selections.empty();
}

#ifdef DEBUG
std::string Selections::ToString() const
{
    std::string res;
    for (size_t i = 0; i < selections.size(); ++i)
    {
        const Selection& s = selections[i];
        res += "[" + IdToString(s.id) + "," + std::to_string(s.start) + "," + std::to_string(s.size) + "]";
        if (i < selections.size() - 1)
            res += ",";
    }
    return res;
}
#endif

//CaretState

CaretState::CaretState(const std::vector<ElementPtr>& elements) :
    id(elements[0]->id)
{
    selections.AddSelection(Selection{id, 0, (uint)elements.size()});
}

CaretState::CaretState(const ElementId _id) :
    id(_id)
{
}

CaretState::CaretState(const Element* element, const uint pos)
{
    id = element->id;
    id.push_back(pos);
}

CaretState::CaretState(const ElementId _id, const uint pos) :
    id(_id)
{
    id.push_back(pos);
}

CaretState::CaretState(const ElementId _id, const uint pos, const uint count) :
    id(_id)
{
    selections.AddSelection(id, pos, count);
    id.push_back(pos);
}

CaretState::CaretState(const Element* element, const uint pos, const Selections& _selections) :
    CaretState(element, pos)
{
    selections = _selections;
}

bool CaretState::operator==(const CaretState& c)
{
    return id == c.id && selections == c.selections;
}

bool CaretState::operator!=(const CaretState& c)
{
    return id != c.id || selections != c.selections;
}

void CaretState::SetState(ElementPtr element)
{
    id = element->id;
    //selections.ClearSelection();
}

void CaretState::SetState(const ElementId _id, const Selections& _selections)
{
    id = _id;
    selections = _selections;
}

void CaretState::SetState(const ElementId _id, const ElementId tail_id)
{
    id = _id;
    for (auto i : tail_id)
        id.push_back(i);
}

void CaretState::SetState(const ElementId _id, const uint pos)
{
    id = _id;
    id.push_back(pos);
}

void CaretState::MergeState(const CaretState& caret_state)
{
    if (!caret_state.id.empty())
        id = caret_state.id;
    for (const Selection& s : caret_state.selections.selections)
        selections.AddSelection(s.id, s.start, s.size);
}

void CaretState::SetPos(const uint pos)
{
    id[id.size() - 1] = pos;
}

uint CaretState::GetPos() const
{
    return id[id.size() - 1];
}

ElementId CaretState::GetElement() const
{
    ElementId _id(id);
    _id.erase(_id.end() - 1);
    return _id;
}

int CaretState::GetPos(const ElementId& _id) const
{
    if (IsInsideElement(_id))
        return GetPos();
    uint start, size;
    if (selections.HasSelection(_id, start, size))
        return start;
    return -1;
}

uint CaretState::GetElementPos(const ElementId& _id) const
{
    return id[_id.size()];
}

int CaretState::GetStatePos(const ElementId& _id) const
{
    if (id.size() < _id.size())
        return -1;
    size_t i = 0;
    for (; i < _id.size(); ++i)
    {
        if (_id[i] != id[i])
            return -1;
    }
    return i - 1;
}

ElementId CaretState::GetElementAtPos(const uint pos) const
{
    ElementId _id(id);
    _id.erase(_id.begin() + pos);
    return _id;
}

ElementId CaretState::GetTailId(const uint pos) const
{
    ElementId _id(id);
    _id.erase(_id.begin(), _id.begin() + pos);
    return _id;
}

bool CaretState::IsInsideElement(const ElementId& _id) const
{
    if (id.size() < _id.size() || _id.empty())
        return false;
    for (size_t i = 0; i < _id.size(); ++i)
    {
        if (_id[i] != id[i])
            return false;
    }
    return true;
}

bool CaretState::IsEmpty() const
{
    return id.empty() && selections.IsEmpty();
}

std::string CaretState::ToString() const
{
    std::string res = "id:" + IdToString(id) + "; ";
    res += "selections: " + selections.ToString();
    return res;
}

}
