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
    CodeString(Document* _document, const std::string str, const StringFormatPtr _format);
    CodeString(const String& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::string str, const StringFormatPtr _format);

    virtual void Draw() const;

    virtual void UpdateRect();

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual std::string ToHtml();
};

}

#endif
