#ifndef __CODE_PARAGRAPH_H__
#define __CODE_PARAGRAPH_H__

#include "../paragraph.h"

namespace yutovo
{

//Group of code rows
class CodeParagraph : public Paragraph
{
public:
    CodeParagraph(Element* _parent);
    CodeParagraph(Document* _document);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void UpdateRect(bool with_elements = false);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual void AddEmptyElement();

    virtual std::string ToHtml();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << (boost::serialization::base_object<Element>(*this), elements);
        ar << format->name;
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<Element>(*this), elements);
        std::string format_name;
        ar >> format_name;
        auto f = document->paragraph_formats->GetFormat(format_name);
        if (f)
            format = f;
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::CodeParagraph* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::CodeParagraph* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::CodeParagraph(p);
}

}
}

#endif
