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
    STRING,
    CODE_BLOCK,
    CODE_PARAGRAPH,
    CODE_ROW,
    CODE_STRING,
    SHAPE,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVISION,
    POWER,
    SQUARE_ROOT,
    NTH_ROOT,
    EQUATION,
    REAL_RESULT,
    INTEGER_RESULT,
    RATIONAL_RESULT,
    COMPLEX_RESULT,
    AUTO_RESULT,
    ERROR_RESULT
};

#define MAX_LEVEL 3

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
    virtual bool Copy(std::vector<ElementPtr>& copy);

    virtual Element* Create(Element* parent) = 0;

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);
    virtual void Normalize(bool with_undo);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo);

    virtual bool Split(const uint max_left_width);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);

    virtual void UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format);

    virtual bool AfterInsert(bool with_undo);

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
    virtual bool HasLastCaretState();
    virtual bool CanContinueSelection();

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual std::string ToHtml();
    virtual std::string ToText();

    virtual void UpdateRect(bool with_elements = false);

    Element* GetElementInPos(const ElementId& _id, const uint pos);
    virtual bool GetElementAtCoords(const int x, const int y, ElementId& _id);

    void AddElement(ElementPtr element);
    
    virtual void AddEmptyElement();

    uint GetChildPos(const Element* element);

    Rect GetAbsoluteRect(const Rect& _rect) const;
    virtual Rect GetAbsoluteRect() const;
    Point GetAbsolutePoint(const Point& point) const;

    virtual ParagraphFormatPtr GetParagraphFormat();
    virtual StringFormatPtr GetStringFormat();
    virtual FormulaFormatPtr GetFormulaFormat() const;

    virtual void UpdateFormat(StringFormatPtr& _format);

    virtual void UpdateDrawRect();

    virtual void UpdateLevel(uint8_t _level);

    virtual void SetEditable(bool _editable);

    virtual void FindElements(ElementType _type, std::vector<ElementId>& _elements);

    virtual void ReSolve();

public:
    Element* parent = nullptr;

    Document* document = nullptr;
    Window* window = nullptr;

    ElementType type = ElementType::NONE;
    ElementId id; //hierarchic unique id
    Rect rect; //relative bounding rect
    Rect last_rect; //for determining of necessity of remaking parent
    Rect draw_rect;
    int baseline = 0; //baseline for drawing
    uint8_t level = 1; //level of superscript or subscript
    
    bool editable = true;

protected:
    CaretPtr caret;
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
    virtual void Clone(std::vector<ElementPtr>& _elements, const uint start, const uint size);
    
    virtual void Draw() const;
    virtual void Remake(bool with_parent, bool with_undo);

    virtual ElementPtr Get(uint pos);
    virtual ElementPtr Get(ElementId id);
    virtual ElementId GetElementId(uint pos);
    virtual int GetElementPos(ElementId id);
    virtual int GetChildPos(ElementId id);
    virtual bool IsLast(ElementId id);

    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Remove(const ElementPtr element);
    virtual void Remove(const ElementId id);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void Move(const ElementPtr element, const uint pos);
    virtual void Move(const Elements& _elements, const uint pos);
    virtual void Replace(ElementPtr element, const uint pos);
    virtual void Clear();

    virtual uint Count() const;

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual Rect GetRect();

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);

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
    CaretPtr caret;
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
