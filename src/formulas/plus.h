#ifndef __PLUS_H__
#define __PLUS_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Plus : public OnlyShapeFormula
{
public:
    Plus(Element* _parent);
    Plus(Document* _document);
    Plus(const Plus& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual std::string ToHtml();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Plus* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Plus* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Plus(p);
    else
        ::new(t)yutovo::Plus(user_data.document);
}

}
}

#endif
