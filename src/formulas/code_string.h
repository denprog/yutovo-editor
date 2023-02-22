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
    CodeString(Element* parent, const std::u32string str, const StringFormatPtr _format);
    CodeString(Document* _document, const std::string str, const StringFormatPtr _format);
    CodeString(const String& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string str, const StringFormatPtr _format);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual Rect GetCaretRect(const uint pos) const;

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool IsFormula();

    virtual std::string ToHtml();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << format->id;
        ar << (boost::serialization::base_object<String>(*this), elements);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<String>(*this), elements);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

protected:
    const int empty_rect_width = 6;
};

typedef std::shared_ptr<CodeString> CodeStringPtr;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::CodeString* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::CodeString* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    uint format_id;
    ar >> format_id;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    auto f = user_data.document->GetStringFormat(format_id);
    if (f)
    {
        if (p)
            ::new(t)yutovo::CodeString(p, "", f);
        else
            ::new(t)yutovo::CodeString(user_data.document, "", f);
    }
    else
        ::new(t)yutovo::CodeString(p);
}

}
}

#endif
