#ifndef __ROW_H__
#define __ROW_H__

#include "element.h"
#include "str.h"

namespace yutovo
{

class Row : public Element
{
public:
    Row(Document* _document);
    Row(Document* _document, Element* _parent);
    Row(Element* _parent, bool with_string = true);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Remake(bool with_elements = false);
    virtual void Normalize();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);
    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element);

    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual bool CanContinueSelection();

    virtual void AddEmptyElement();

    virtual bool IsEmpty();
};

typedef std::shared_ptr<Row> RowPtr;

}

#endif
