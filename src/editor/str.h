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
    String(Document* _document, const std::string _str, const StringFormatPtr _format);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Remake(bool with_elements);

    virtual void UpdateRect();

    virtual std::string ToHtml();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);

    virtual bool Split(const uint max_left_width);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);

    virtual bool CanContinueSelection();
    
protected:
    friend class StringElements;
    friend class Document;
    friend class ChangeStringFormatTask;

    StringFormatPtr format;
};

class StringElements : public Elements
{
public:
    StringElements(Element* parent);
    StringElements(Element* parent, const std::string& _str);

    virtual Elements* Clone(Element* _parent);

    virtual void Draw() const;
    virtual void Remake();

    virtual ElementPtr Get(uint pos);
    virtual ElementId GetElementId(uint pos);
    
    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Remove(const ElementPtr element);
    virtual void RemoveAt(const uint pos, const int size);
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

    virtual std::string ToHtml();
    virtual std::string ToText();

private:
    friend class String;

    std::string str;
};

}

#endif
