#include "selection.h"
#include "document.h"
#include "util.h"
#include <functional>

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

ElementId SelectionState::GetCommonElement(uint& start, uint& size) const
{
    ElementId parent_id = GetCommonElement();
    auto& first = state[0];
    auto& last = state[state.size() - 1];
    if (first.id == parent_id)
        start = first.start;
    else
        start = yutovo::GetChildPos(parent_id, first.id);
    
    if (last.id == parent_id)
    {
        if (state.size() == 1)
            size = last.size;
        else
            size = last.start + last.size - start;
    }
    else
    {
        int p = yutovo::GetChildPos(parent_id, last.id);
        size = p - start + 1;
    }

    if (parent_id.size() == 2) //for paragraph take its parent
    {
        start = yutovo::GetChildPos(parent_id);
        size = 1;
        parent_id = yutovo::GetParent(parent_id);
    }

    return parent_id;
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

void Selection::Set(LogicalSelectionState& state)
{
    selection.clear();
    for (ElementLogicalSelectionState& s : state.state)
    {
        std::vector<ElementPtr> elements;
        document->GetElements(s.id, elements);
        int p = 0;
        int start = s.start;
        int size = s.size;
        for (size_t i = 0; i < elements.size(); ++i)
        {
            ElementPtr _el = elements[i];
            if (_el->type == ElementType::PARAGRAPH)
            {
                for (int j = 0; j < _el->elements->Count(); ++j)
                {
                    auto r = _el->elements->Get(j);
                    if (s.start - p + s.size <= r->elements->Count())
                    {
                        Add(r->id, s.start - p, s.size);
                        break;
                    }
                    else if (s.start - p < r->elements->Count())
                    {
                        Add(r->id, s.start - p, r->elements->Count() - p);
                    }
                    p += r->elements->Count();
                }
            }
            else
            {
                if (start + size <= p + (int)_el->elements->Count())
                {
                    Add(_el->id, start - p, size);
                    break;
                }
                else if (start <= p)
                {
                    Add(_el->id, start - p, _el->elements->Count());
                    start += _el->elements->Count();
                    size -= _el->elements->Count();
                }
            }
            p += _el->elements->Count();
        }
    }
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
        //remove selections which will be included in the new item
        for (int i = start; i < start + size; ++i)
        {
            ElementSelection s;
            auto el = element->elements->Get(i);
            if (el && HasChild(el->id, s))
                Remove(s.element->id, s.start, s.size);
        }

        bool decomposed = false;
        for (size_t i = 0; i < selection.size(); ++i)
        {
            if (Decompose(selection[i], element->id))
            {
                selection.erase(selection.begin() + i);

                auto it = std::find_if(selection.begin(), selection.end(), 
                    [element](const ElementSelection& s)
                    {
                        return s.element->id == element->id;
                    });
                ElementSelection& cur = *it;

                //substract or add selection
                if (start == cur.start && start + size <= cur.start + cur.size)
                {
                    cur.start += size;
                    cur.size -= size;
                }
                else if (start >= cur.start && start + size == cur.start + cur.size)
                {
                    cur.size -= size;
                }
                else
                {
                    if (start < cur.start)
                        cur.start = start;
                    if (start + size > cur.start + cur.size)
                        cur.size = start + size - cur.start;
                }

                decomposed = true;
                break;
            }
        }

        if (!decomposed)
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

    if (!document->pasting)
        Optimize();
}

void Selection::Add(const ElementId id, uint start, uint size)
{
    Add(document->GetElement(id), start, size);
}

void Selection::Remove(const ElementId id, uint start, uint size)
{
    for (size_t i = 0; i < selection.size(); ++i)
    {
        ElementSelection& s = selection[i];
        if (s.element->id == id)
            break;
        if (Decompose(s, id))
        {
            selection.erase(selection.begin() + i);
            break;
        }
    }

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
    else if (s.start == start && s.size >= size)
    {
        s.start = start + size;
        s.size = s.size - size;
    }

    Optimize();
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
    for (auto& s : selection)
    {
        if (s.element->id == p_id && s.start >= GetChildPos(id))
            --s.start;
    }

    Optimize();
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
                ElementId child_id = GetChild(s.element->id, i);
                if (child_id == id || IsChild(child_id, id))
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

void Selection::Optimize()
{
    bool optimize = true;
    while (optimize)
    {
        optimize = false;

        //make parent selections if child selections have full size
        for (size_t i = 0; i < selection.size(); ++i)
        {
            ElementSelection& s1 = selection[i];
            if (s1.start == 0 && s1.size == s1.element->elements->Count())
            {
                if (!s1.element->parent)
                    continue;
                int pos = s1.element->parent->elements->GetChildPos(s1.element->id);
                auto p = document->GetElement(s1.element->parent->id);
                selection.erase(selection.begin() + i);
                auto it = std::find_if(selection.begin(), selection.end(), 
                    [p, pos](ElementSelection& s)
                    {
                        return s.element == p && s.start <= pos && s.start + s.size > pos;
                    });
                if (it == selection.end())
                {
                    ElementSelection s{p, (uint)pos, 1};
                    selection.emplace_back(s);
                }
                optimize = true;
                break;
            }
        }

        if (optimize)
        {
            std::sort(selection.begin(), selection.end());
            continue;
        }
        
        //try to merge selections
        for (size_t i = 0; i < selection.size(); ++i)
        {
            ElementSelection& s1 = selection[i];
            for (size_t j = i + 1; j < selection.size(); ++j)
            {
                ElementSelection& s2 = selection[j];
                if (s1.element->id == s2.element->id && s1.start + s1.size == s2.start)
                {
                    s1.size += s2.size;
                    selection.erase(selection.begin() + j);
                    optimize = true;
                    break;
                }
            }
            if (optimize)
                break;
        }

        if (!optimize)
        {
            //remove selections which are inside another selections
            for (size_t i = 0; i < selection.size(); ++i)
            {
                ElementSelection& s1 = selection[i];
                for (size_t j = 0; j < selection.size(); ++j)
                {
                    if (j == i)
                        continue;
                    ElementSelection& s2 = selection[j];
                    if (IsChild(s1.element->id, s2.element->id))
                    {
                        for (int k = s1.start; k < s1.start + s1.size; ++k)
                        {
                            auto child_id = GetChild(s1.element->id, k);
                            if (IsChild(child_id, s2.element->id) || child_id == s2.element->id)
                            {
                                selection.erase(selection.begin() + j);
                                optimize = true;
                                break;
                            }
                        }
                        if (optimize)
                            break;
                    }
                }
                if (optimize)
                    break;
            }
        }

        selection.erase(std::unique(selection.begin(), selection.end()), selection.end()); //remove dublicates

        if (selection.size() == 1)
        {
            auto& s = selection[0];
            if (s.IsEmpty() || (s.element->elements->Count() > 0 && s.size == 0))
                selection.clear();
        }
    }
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

LogicalSelectionState Selection::GetLogicalState() const
{
    LogicalSelectionState state;
    for (auto& s : selection)
    {
        ElementId _id = s.element->id;
        if (_id.size() == 1) //paragraphs of text
        {
            LogicalId logical_id = document->GetLogicalId(_id);
            state.state.push_back(ElementLogicalSelectionState{logical_id, s.start, s.size});
        }
        else if (_id.size() == 2) //rows of paragraph
        {
            auto row = document->GetElement(GetChild(_id, s.start));
            CaretState c1, c2;
            row->GetFirstCaretState(c1, nullptr);
            LogicalId p1 = document->GetLogicalId(c1.id);
            uint size = 0;

            for (int i = s.start; i < s.start + s.size; ++i)
            {
                auto row = document->GetElement(GetChild(_id, i));
                for (int j = 0; j < row->elements->Count(); ++j)
                {
                    auto _el = row->elements->Get(j);
                    _el->GetLastCaretState(c2, nullptr);
                    LogicalId p2 = document->GetLogicalId(c2.id);
                    if (GetParent(p1) == GetParent(p2))
                    {
                        size = GetChildPos(p2) - GetChildPos(p1);
                    }
                    else
                    {
                        state.state.push_back(ElementLogicalSelectionState{yutovo::GetParent(p1), (uint)yutovo::GetChildPos(p1), size});
                        _el->GetFirstCaretState(c1, nullptr);
                        p1 = document->GetLogicalId(c1.id);
                    }
                }
            }

            state.state.push_back(ElementLogicalSelectionState{yutovo::GetParent(p1), (uint)yutovo::GetChildPos(p1), size});
        }
        else //others
        {
            _id.push_back(s.start);
            LogicalId logical_id = document->GetLogicalId(_id);
            state.state.push_back(ElementLogicalSelectionState{yutovo::GetParent(logical_id), (uint)yutovo::GetChildPos(logical_id), s.size});
        }
    }
    return state;
}

bool Selection::Decompose(ElementSelection s, ElementId until_id)
{
    if (s.element->id == until_id)
    {
        selection.emplace_back(ElementSelection{s.element, 0, s.element->elements->Count()});
        return true;
    }
    if (!IsChild(s.element->id, until_id))
        return false;
    for (int i = s.start; i < s.start + s.size; ++i)
    {
        auto el = s.element->elements->Get(i);
        if (IsChild(el->id, until_id) || el->id == until_id)
        {
            if (Decompose(ElementSelection{el, 0, el->elements->Count()}, until_id))
            {
                for (int j = i + 1; j < s.start + s.size; ++j)
                {
                    auto _el = s.element->elements->Get(j);
                    selection.emplace_back(ElementSelection{_el, 0, _el->elements->Count()});
                }
                return true;
            }
            return false;
        }
        else
            selection.emplace_back(ElementSelection{el, 0, el->elements->Count()});
    }
    return false;
}

}
