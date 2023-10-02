#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "element.h"

namespace yutovo
{

typedef unsigned char uchar;

//Image
class Image : public Element
{
public:
    Image(Element* _parent, const std::string& _image_base64, const int _width, const int _height);
    Image(Element* _parent, const std::vector<unsigned char>& _bmp, const int _width, const int _height);
    Image(Document* _document, const std::string& _image_base64, const int _width, const int _height);
    Image(Document* _document, const std::vector<unsigned char>& _bmp, const int _width, const int _height);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual bool Remake(bool with_elements = false);

    virtual void UpdateRect(bool with_elements = false);

    virtual void Draw() const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();

    virtual std::string ToHtml();

private:
    void Base64Encode();
    void Base64Decode();

public:
    std::string image_base64;
    std::vector<unsigned char> bmp; //ARGB32 format
    int width = 0, height = 0;

private:
    static const std::string base;
    Size image_size;
};

}

#endif
