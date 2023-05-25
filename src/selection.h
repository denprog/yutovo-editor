#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "caret_state.h"

namespace yutovo
{

class Document;

struct ElementSelection
{
    bool operator==(const ElementSelection& s) const;
    bool operator!=(const ElementSelection& s) const;
    bool operator<(const ElementSelection& s) const;

    bool IsEmpty() const;

    ElementPtr element;
    uint start = 0;
    uint size = 0;
};

struct ElementSelectionState
{
    bool operator==(const ElementSelectionState& compare) const;
    bool operator!=(const ElementSelectionState& compare) const;

    ElementId id;
    uint start = 0;
    uint size = 0;
};

struct SelectionState
{
    SelectionState() = default;
    SelectionState(const ElementId id, uint start, uint size);

    bool operator==(const SelectionState& s) const;
    bool operator!=(const SelectionState& s) const;

    void Add(const ElementId id, uint start, uint size);
    void Add(const ElementSelectionState& s);
    void Merge(const SelectionState& s);
    
    ElementId GetCommonElement() const;
    ElementId GetCommonElement(uint& start, uint& size) const;

    bool IsEmpty() const;

#ifdef DEBUG
    std::string ToString() const;
#endif

    std::vector<ElementSelectionState> state;
};

class Selection
{
public:
    Selection(Document* _document);
    
    bool operator==(const Selection& s) const;
    bool operator!=(const Selection& s) const;

public:
    void Set(SelectionState& state);
    void Add(const ElementPtr element, uint start, uint size);
    void Add(const ElementId id, uint start, uint size);
    void Remove(const ElementId id, uint start, uint size);
    void InsertElement(const ElementId id);
    void RemoveElement(const ElementId id);

    bool Has(const ElementPtr element, uint& start, uint& size) const;
    bool Has(const ElementId id, uint& start, uint& size) const;
    bool Has(const ElementId id, ElementSelection& s) const;
    bool HasChild(const ElementId id, ElementSelection& s) const;
    bool IsSelected(const ElementId id) const;

    void Clear();

    bool IsEmpty() const;

    SelectionState GetState() const;

private:
    Document* document;

public:
    std::vector<ElementSelection> selection;

    bool can_optimize = true;
};

}

#endif
