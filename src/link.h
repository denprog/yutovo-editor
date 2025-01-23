#ifndef __LINK_H__
#define __LINK_H__

#include "str.h"

namespace yutovo
{

class Link : public String
{
public:
    Link(Element* parent);
    Link(Element* parent, const std::string _str, const std::string _url, bool _translate = false);
    Link(Element* parent, const std::string _str, const std::string _url, const StringFormatPtr _format, bool _translate = false);
    Link(Element* parent, const std::u32string _str, const std::u32string _url);
    Link(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format);
    Link(Document* _document);
    Link(Document* _document, const std::string _str, const std::string _url, const StringFormatPtr _format);
    Link(Document* _document, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual std::string ToHtml();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element);

    void Visit();

public:
    std::u32string url;
};

}

#endif