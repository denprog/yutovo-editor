#ifndef __CARET_STATE_H__
#define __CARET_STATE_H__

#include <vector>
#include <memory>

namespace yutovo
{

class Element;

typedef std::vector<uint> ElementId;
typedef std::shared_ptr<Element> ElementPtr;

struct Selection
{
    bool operator==(const Selection& s) const
    {
        return id == s.id && start == s.start && size == s.size;
    }

    ElementId id;
    uint start;
    uint size;
};

struct Selections
{
    bool operator==(const Selections& s);
    bool operator!=(const Selections& s);
    
    void AddSelection(const Selection& selection);
    void AddSelection(const ElementId& id, const uint pos, const uint count);
    void ClearSelection();
    void ClearSelection(const ElementId& id);
    bool HasSelection(const ElementId& id, uint& start, uint& size) const;
    bool IsEmpty() const;

#ifdef DEBUG
    std::string ToString() const;
#endif

    std::vector<Selection> selections;
};

struct CaretState
{
    CaretState() = default;
    CaretState(const std::vector<ElementPtr>& elements);
    CaretState(const ElementId _id);
    CaretState(const ElementId _id, const uint pos);
    CaretState(const ElementId _id, const uint pos, const uint count);
    CaretState(const Element* element, const uint pos);
    CaretState(const Element* element, const uint pos, const Selections& _selections);

    bool operator==(const CaretState& c);
    bool operator!=(const CaretState& c);

    void SetState(ElementPtr element);
    void SetState(const ElementId _id, const Selections& _selections);
    void SetState(const ElementId _id, const ElementId tail_id);
    void SetState(const ElementId _id, const uint pos);

    void SetPos(const uint pos);

    uint GetPos() const;
    uint GetElementPos(const ElementId& _id) const;
    int GetStatePos(const ElementId& _id) const;
    ElementId GetElementAtPos(const uint pos) const;
    ElementId GetTailId(const uint pos) const;
    bool IsInsideElement(const ElementId& _id) const;

    bool IsEmpty() const;

#ifdef DEBUG
    std::string ToString() const;
#endif

    ElementId id;
    Selections selections;
};

}

#endif
