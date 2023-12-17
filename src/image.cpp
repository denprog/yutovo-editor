#include "image.h"
#include "document.h"

namespace yutovo
{

//Image

const std::string Image::base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

Image::Image(Element* _parent, const std::string& _image_base64, const int _width, const int _height) :
    Element(_parent),
    image_base64(_image_base64),
    width(_width),
    height(_height)
{
    type = ElementType::IMAGE;
    editable = false;

    Base64Decode();

    image_size = window->GetImageSize(picture, width, height);
}

Image::Image(Element* _parent, const std::vector<unsigned char>& _picture, const int _width, const int _height) :
    Element(_parent),
    picture(_picture),
    width(_width),
    height(_height)
{
    type = ElementType::IMAGE;
    editable = false;
    image_size = window->GetImageSize(picture, width, height);

    Base64Encode();
}

Image::Image(Document* _document, const std::string& _image_base64, const int _width, const int _height) :
    Element(_document),
    image_base64(_image_base64),
    width(_width),
    height(_height)
{
    type = ElementType::IMAGE;
    editable = false;

    Base64Decode();

    image_size = window->GetImageSize(picture, width, height);
}

Image::Image(Document* _document, const std::vector<unsigned char>& _picture, const int _width, const int _height) :
    Element(_document),
    picture(_picture),
    width(_width),
    height(_height)
{
    type = ElementType::IMAGE;
    editable = false;
    image_size = window->GetImageSize(picture, width, height);

    Base64Encode();
}

Element* Image::Clone()
{
    return new Image(*this);
}

Element* Image::Create(Element* parent)
{
    return new Image(parent, "", 0, 0);
}

void Image::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);
    rapidjson::Value _image_base64(image_base64.c_str(), alloc);
    value.AddMember("image_base64", _image_base64, alloc);
    value.AddMember("width", width, alloc);
    value.AddMember("height", height, alloc);
}

Element* Image::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("image_base64") || !value["image_base64"].IsString() || 
        !value.HasMember("width") || !value["width"].IsInt() || 
        !value.HasMember("height") || !value["height"].IsInt())
        return nullptr;
    auto image_base64 = value["image_base64"].GetString();
    auto width = value["width"].GetInt();
    auto height = value["height"].GetInt();
    if (parent)
        return new Image(parent, image_base64, width, height);
    return new Image(document, image_base64, width, height);
}

bool Image::Remake(bool with_elements)
{
    UpdateRect();

    bool changed = (rect != last_rect);
    last_rect = rect;
    return changed;
}

void Image::UpdateRect(bool with_elements)
{
    rect.SetSize(image_size.width + 2, image_size.height + 2);
    baseline = image_size.height;
}

void Image::Draw() const
{
    auto r = GetAbsoluteRect();
    window->DrawImage(r.left + 1, r.top + 1, image_size.width, image_size.height, picture);
    if (document->selection.IsSelected(id))
    {
        const auto f = GetStringFormat();
        if (f)
            window->DrawRect(r, f->text_bg_selection_color);
    }
}

bool Image::HasCaretState()
{
    return true;
}

bool Image::HasLastCaretState()
{
    return true;
}

std::string Image::ToHtml()
{
    return "<img src=\"data:image/bmp;base64," + image_base64 + "\">";
}

void Image::Base64Encode()
{
    image_base64.clear();

    int val = 0, valb = -6;
    for (uchar c : picture)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            image_base64.push_back(base[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        image_base64.push_back(base[((val << 8) >> (valb + 8)) & 0x3F]);
    while (image_base64.size() % 4)
        image_base64.push_back('=');
}

void Image::Base64Decode()
{
    std::vector<int> t(256, -1);
    for (int i = 0; i < 64; ++i)
        t[base[i]] = i;

    int val = 0, valb = -8;
    for (uchar c : image_base64)
    {
        if (t[c] == -1)
            break;
        val = (val << 6) + t[c];
        valb += 6;
        if (valb >= 0)
        {
            picture.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
}

}
