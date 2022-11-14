#ifndef __STRING_H__
#define __STRING_H__

#include <string>
#include "element.h"
#include "style.h"

namespace yutovo
{

class String : public Element
{
public:
    String(Element* parent);
    String(Element* parent, const std::string _str);
    String(Element* parent, const std::string _str, const StringFormatPtr _format);

    virtual Element* Clone();

    virtual void Remake(CaretState& caret_state, bool with_elements);

    virtual void UpdateRect();

    virtual std::string ToHtml();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo);
    virtual bool DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo);

    //virtual bool CanSplit(const uint max_left_width);
    virtual bool Split(const uint max_left_width, CaretState& caret_state);
    //virtual bool CanMerge(const ElementPtr with_element);
    virtual bool Merge(const ElementPtr with_element, CaretState& caret_state);

    virtual bool CanContinueSelection();
    
protected:
    friend class StringElements;
    friend class Document;

    StringFormatPtr format;
};

class StringElements : public Elements
{
public:
    StringElements(Element* parent);
    StringElements(Element* parent, const std::string& _str);

    virtual void Draw(const Selections& selections) const;
    virtual void Remake(CaretState& caret_state);

    virtual ElementId GetElementId(uint pos);
    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Insert(ElementPtr element, const uint pos, CaretState& caret_state);
    virtual void Remove(const ElementPtr element);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void RemoveAt(const uint pos, const int size, CaretState& caret_state);
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

    virtual std::string ToHtml();
    virtual std::string ToText();

private:
    friend class String;

    std::string str;
};

}

#endif
