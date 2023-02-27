#ifndef __MULTIPLY_H__
#define __MULTIPLY_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Multiply : public OnlyShapeFormula
{
public:
    Multiply(Element* _parent);
    Multiply(Document* _document);
    Multiply(const Multiply& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual std::u32string ToText();
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
void save_construct_data(Archive& ar, const yutovo::Multiply* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Multiply* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Multiply(p);
    else
        ::new(t)yutovo::Multiply(user_data.document);
}

}
}

#endif
