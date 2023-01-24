#ifndef __PAGE_H__
#define __PAGE_H__

#include "block.h"
#include "style.h"
#include <boost/serialization/unique_ptr.hpp>

namespace yutovo
{

class Page : public Block
{
public:
    Page(Element* parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);
    virtual void UpdateRect(bool with_elements = false);

    virtual ParagraphFormatPtr GetParagraphFormat();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << (boost::serialization::base_object<Element>(*this), elements);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<Element>(*this), elements);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
    int page_width = 0;

private:
    PageFormatPtr format;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Page* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Page* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Page(p);
}

}
}

#endif
