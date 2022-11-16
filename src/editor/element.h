#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include <memory>
#include <vector>
#include "window.h"
#include "util.h"
#include "caret_state.h"

namespace yutovo
{

class Document;
class Elements;

enum class ElementType
{
    NONE = 0,
    TEXT,
    PAGE,
    PARAGRAPH,
    ROW,
    STRING
};

class Element;

//Base class for all the elements of the document
class Element
{
public:
    Element(Element* _parent);
    Element(const Element& source);
    virtual ~Element();

    virtual Element* Clone() = 0;

    virtual Element* Create(Element* parent) = 0;

    virtual void Draw(const Selections& selections) const;
    virtual void Remake(CaretState& caret_state, bool with_elements);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo);
    virtual bool DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo);

    virtual bool Split(const uint max_left_width, CaretState& caret_state);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element, CaretState& caret_state);

    virtual bool GetFirstCaretState(CaretState& caret_state, bool selection);
    virtual bool GetLastCaretState(CaretState& caret_state, bool selection);
    virtual bool GetLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetRightCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& res, bool selection);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& res, bool selection);
    virtual bool GetBeginCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetEndCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetWordLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetWordRightCaretState(const CaretState& before_state, CaretState& after_state, bool selection);

    virtual bool HasCaretState();
    virtual bool CanContinueSelection();

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual std::string ToHtml();
    virtual std::string ToText();

    virtual void UpdateRect();

    Element* GetElementInPos(const ElementId& _id, const uint pos);
    //ElementPtr GetElement(const ElementId& _id);
    //Element* GetParent(const ElementId& _id);

    void AddElement(ElementPtr element);
    // void RemoveElement(uint pos, CaretState& caret_state);
    // void MoveElement(ElementPtr from, uint from_pos, ElementPtr to, uint to_pos, CaretState& caret_state);

    uint GetChildPos(const Element* element);

    Rect GetAbsoluteRect(const Rect& _rect) const;
    virtual Rect GetAbsoluteRect() const;
    Point GetAbsolutePoint(const Point& point) const;

public:
    Document* document = nullptr;
    Window* window = nullptr;

    ElementType type = ElementType::NONE;
    Element* parent;
    ElementId id; //hierarchic unique id
    Rect rect; //relative bounding rect
    int baseline = 0;
    
    bool modified = false;
    
    bool editable = true;
    //bool mergeable = true;

    std::unique_ptr<Elements> elements; //child nodes

#ifdef DEBUG
    std::string to_str;
#endif
};

class Elements
{
public:
    Elements(Element* _parent);
    Elements(const Elements& source);

    ElementPtr operator[](const int pos);
    
    virtual void Draw(const Selections& selections) const;
    virtual void Remake(CaretState& caret_state);

    virtual ElementPtr Get(uint pos);
    virtual ElementId GetElementId(uint pos);
    virtual int GetElementPos(ElementId id);

    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Insert(ElementPtr element, const uint pos, CaretState& caret_state);
    virtual void Remove(const ElementPtr element);
    virtual void Remove(const ElementPtr element, CaretState& caret_state);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void RemoveAt(const uint pos, const int size, CaretState& caret_state);
    virtual void Move(const ElementPtr element, const uint pos);
    virtual void Move(const ElementPtr element, const uint pos, CaretState& caret_state);
    virtual void Clear();
    virtual uint Count();

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual Rect GetRect();

    virtual bool GetFirstCaretState(CaretState& caret_state, bool selection);
    virtual bool GetLastCaretState(CaretState& caret_state, bool selection);
    virtual bool GetLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetRightCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetWordLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection);
    virtual bool GetWordRightCaretState(const CaretState& before_state, CaretState& after_state, bool selection);

    virtual bool HasLastCaretState();

    virtual std::string ToHtml();
    virtual std::string ToText();

protected:
    virtual void UpdateIds();

protected:
    Element* parent;

private:
    std::vector<ElementPtr> elements;
};

}

#endif
