#ifndef __CODE_STRING_H__
#define __CODE_STRING_H__

#include "../str.h"

namespace yutovo
{

class CodeString : public String
{
public:
    CodeString(Element* parent);
    CodeString(Element* parent, const std::string str);
    CodeString(Element* parent, const std::string str, const StringFormatPtr _format);
    CodeString(Element* parent, const std::u32string str);
    CodeString(Element* parent, const std::u32string str, const StringFormatPtr _format);
    CodeString(Document* _document);
    CodeString(Document* _document, const std::string str, const StringFormatPtr _format);
    CodeString(const String& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string str, const StringFormatPtr _format);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual Rect GetCaretRect(const uint pos) const;

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool IsFormula();

    virtual std::string ToHtml();

protected:
    const int empty_rect_width = 6;
};

typedef std::shared_ptr<CodeString> CodeStringPtr;

}

#endif
