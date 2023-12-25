#ifndef __TEXT_H__
#define __TEXT_H__

#include <vector>
#include "block.h"
#include "util.h"

namespace yutovo
{

//The root element of the editor
class Text : public Block
{
public:
    Text(Document* _document, bool with_paragraph = true);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;

    virtual bool Remake(bool with_elements = false);

    virtual void UpdateRect(bool with_elements = false);
    virtual void UpdateDrawRect();

    virtual bool GetElementAtCoords(const int x, const int y, ElementId& _id);
    virtual bool GetNearestElement(const int x, const int y, ElementId& _id, int& dist);
    virtual bool GetNearestCaretState(const int x, const int y, CaretState& caret_state);

    virtual Rect GetCaretRect(const uint pos) const;

    virtual ParagraphFormatPtr GetParagraphFormat();

    virtual bool IsEmpty();

    virtual std::string ToHtml();

    virtual Rect GetAbsoluteRect() const;

public:
    int page_width = 0;

    TextFormatPtr text_format;
    PageFormatPtr page_format;
};

}

#endif
