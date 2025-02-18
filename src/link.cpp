#include "link.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//Link

Link::Link(Element* parent) : 
    String(parent)
{
    type = ElementType::LINK;
    if (document)
    {
        format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
            document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
    }
}

Link::Link(Element* parent, const std::string _str, const std::string _url, bool _translate) : 
    String(parent, _str, _translate),
    url(ToUtfString(_url))
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Element* parent, const std::string _str, const std::string _url, const StringFormatPtr _format, bool _translate) : 
    String(parent, _str, _format, _translate),
    url(ToUtfString(_url))
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Element* parent, const std::u32string _str, const std::u32string _url) : 
    String(parent, _str),
    url(_url)
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format) : 
    String(parent, _str, _format),
    url(_url)
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Document* _document) : 
    String(_document)
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Document* _document, const std::string _str, const std::string _url, const StringFormatPtr _format) : 
    String(_document, _str, _format),
    url(ToUtfString(_url))
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Link::Link(Document* _document, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format): 
    String(_document, _str, _format),
    url(_url)
{
    type = ElementType::LINK;
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, true, format->strikethrough, 
        document->config.link_color, format->text_bg_color, format->text_bg_selection_color);
}

Element* Link::Clone()
{
    return new Link(*this);
}

Element* Link::Create(Element* parent)
{
    return new Link(parent);
}

Element* Link::Create(Element* parent, const std::u32string _str, const std::u32string _url, const StringFormatPtr _format)
{
    return new Link(parent, _str, _url, _format);
}

void Link::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    String::ToJson(value, alloc);
    rapidjson::Value _url(ToBasicString(url).c_str(), alloc);
    value.AddMember("url", _url, alloc);
}

Element* Link::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("url") || !value["url"].IsString())
        return nullptr;
    auto url = value["url"].GetString();
    if (value.HasMember("format_id") && value["format_id"].IsString())
    {
        auto format_id_str = value["format_id"].GetString();
        boost::uuids::uuid format_id;
        try
        {
            format_id = boost::lexical_cast<boost::uuids::uuid>(format_id_str);
        }
        catch (std::bad_cast& ex)
        {
            return nullptr;
        }

        auto f = document->GetStringFormat(format_id);
        if (f)
        {
            if (parent)
                return new Link(parent, "", url, f);
            return new Link(document, "", url, f);
        }
    }
    return new Link(parent, "", url);
}

std::string Link::ToHtml()
{
    std::string s = "<a url=\"" + ToBasicString(url) + "\" ";
    s += "style=\"font-family:'";
    s += format->family;
    s += "';";
    s += "font-size:";
    s += std::to_string(format->size);
    s += "px;";
    if (format->underline)
        s += "text-decoration: underline;";
    if (format->strikethrough)
        s += "text-decoration: line-through;";
    if (format->text_color != Color::Black())
        s += "color:" + format->text_color.ToString() + ";";
    if (format->text_bg_color != Color::White())
        s += "bgcolor:" + format->text_bg_color.ToString() + ";";
    s += "\">";
    if (format->bold)
        s += "<strong>";
    if (format->italic)
        s += "<em>";
    s += elements->ToHtml();
    if (format->italic)
        s += "</em>";
    if (format->bold)
        s += "</strong>";
    s += "</a>";
    return s;
}

bool Link::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    if (_elements.size() == 1)
    {
        Link* link = (Link*)_elements[0].get();
        if (link->ToText() == U"" || link->url.empty() || (link->ToText() == ToText() && url == link->url))
            return false;

        format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, format->underline, format->strikethrough,
            document->config.link_color, format->text_bg_color, format->text_bg_selection_color);

        if (with_undo)
            document->StoreUndo(id);
        SetString(link->ToText());
        caret->SetState(id, elements->Count());
        url = link->url; //just change the url
        changed_element = id;
        return true;
    }
    return String::InsertElements(_elements, with_undo, changed_element);
}

void Link::Visit()
{
    window->OnLinkClicked(id, url);
    format = document->string_formats->GetFormat(format->family, format->size, format->bold, format->italic, format->underline, format->strikethrough, 
        document->config.link_visited_color, format->text_bg_color, format->text_bg_selection_color);
    document->Redraw(id, false);
}

}
