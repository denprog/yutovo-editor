#ifndef __CARET_STATE_H__
#define __CARET_STATE_H__

#include <vector>
#include <memory>

namespace yutovo
{

class Element;
class Caret;

typedef std::vector<uint> ElementId;
typedef std::shared_ptr<Element> ElementPtr;
typedef std::shared_ptr<Caret> CaretPtr;

struct CaretState
{
    CaretState() = default;
    CaretState(const std::vector<ElementPtr>& elements);
    CaretState(const ElementId _id);
    CaretState(const ElementId _id, const uint pos);
    CaretState(const Element* element, const uint pos);

    bool operator==(const CaretState& c);
    bool operator!=(const CaretState& c);

    void SetState(ElementPtr element);
    void SetState(const ElementId _id, const ElementId tail_id);
    void SetState(const ElementId _id, const uint pos);
    void SetState(const ElementId _id);

    void SetPos(const uint pos);

    uint GetPos() const;
    ElementId GetElement() const;
    int GetPos(const ElementId& _id) const;
    int GetElementPos(const ElementId& _id) const;
    int GetPosInElement(const ElementId& _id) const;
    int GetStatePos(const ElementId& _id) const;
    ElementId GetElementAtPos(const uint pos) const;
    ElementId GetTailId(const uint pos) const;
    bool IsInsideElement(const ElementId& _id) const;

    bool IsEmpty() const;

#ifdef DEBUG
    std::string ToString() const;
#endif

    ElementId id;
};

}

#endif
