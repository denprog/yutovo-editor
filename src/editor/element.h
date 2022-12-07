#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <memory>
#include <fstream>
#include "window.h"
#include "util.h"
#include "caret_state.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/unique_ptr.hpp>

namespace yutovo
{

class Document;
class Elements;
class Caret;
class Selection;

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
    Element(Document* _document);
    Element(Element* _parent);
    Element(const Element& source);
    virtual ~Element();

    virtual Element* Clone() = 0;

    virtual Element* Create(Element* parent) = 0;

    virtual void Draw() const;
    virtual void Remake(bool with_elements);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo);

    virtual bool Split(const uint max_left_width);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);

    virtual void UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);

    virtual bool HasCaretState();
    virtual bool CanContinueSelection();

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual std::string ToHtml();
    virtual std::string ToText();

    virtual void UpdateRect();

    Element* GetElementInPos(const ElementId& _id, const uint pos);

    void AddElement(ElementPtr element);

    uint GetChildPos(const Element* element);

    Rect GetAbsoluteRect(const Rect& _rect) const;
    virtual Rect GetAbsoluteRect() const;
    Point GetAbsolutePoint(const Point& point) const;

    virtual ParagraphFormatPtr GetParagraphFormat();
    virtual StringFormatPtr GetStringFormat();

public:
    Element* parent = nullptr;

    Document* document = nullptr;
    Window* window = nullptr;

    ElementType type = ElementType::NONE;
    ElementId id; //hierarchic unique id
    Rect rect; //relative bounding rect
    int baseline = 0;
    
    bool modified = false;
    
    bool editable = true;

protected:
    Caret* caret = nullptr;
    Selection* selection = nullptr;

public:
    std::unique_ptr<Elements> elements; //child nodes

#ifdef DEBUG
public:
    std::string to_str;
#endif
};

class Elements
{
public:
    Elements(Element* _parent);
    Elements(const Elements& source);

    ElementPtr operator[](const int pos);

    virtual Elements* Clone(Element* _parent);
    
    virtual void Draw() const;
    virtual void Remake();

    virtual ElementPtr Get(uint pos);
    virtual ElementPtr Get(ElementId id);
    virtual ElementId GetElementId(uint pos);
    virtual int GetElementPos(ElementId id);
    virtual int GetChildPos(ElementId id);

    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Remove(const ElementPtr element);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void Move(const ElementPtr element, const uint pos);
    virtual void Clear();

    virtual uint Count();

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual Rect GetRect();

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);

    virtual bool HasLastCaretState();

    virtual std::string ToHtml();
    virtual std::string ToText();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << parent;
        ar << elements;
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        elements.clear();
        ar >> elements;
        UpdateIds();
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

protected:
    virtual void UpdateIds();

protected:
    Element* parent = nullptr;
    Caret* caret = nullptr;
    Selection* selection = nullptr;

private:
    std::vector<ElementPtr> elements;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Elements* t, const unsigned int version)
{
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Elements* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Elements(p);
}

}
}

#endif
