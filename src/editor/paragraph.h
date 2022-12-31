#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include "element.h"
#include "style.h"
#include "document.h"
#include <boost/serialization/unique_ptr.hpp>

namespace yutovo
{

class Paragraph : public Element
{
public:
    Paragraph(Element* parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);
    virtual void Normalize(bool with_undo);
    virtual void UpdateRect(bool with_elements = false);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeParagraphFormat(const ParagraphFormatPtr _format, bool with_undo);

    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual bool CanContinueSelection();

    virtual StringFormatPtr GetStringFormat();
    
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

protected:
    friend class Document;
    ParagraphFormatPtr format;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Paragraph* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Paragraph* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Paragraph(p);
}

}
}

#endif
