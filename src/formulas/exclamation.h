#ifndef __NOT_H__
#define __NOT_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Exclamation : public OnlyShapeFormula
{
public:
    Exclamation(Element* _parent);
    Exclamation(Document* _document);
    Exclamation(const Exclamation& source);

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
void save_construct_data(Archive& ar, const yutovo::Exclamation* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Exclamation* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Exclamation(p);
    else
        ::new(t)yutovo::Exclamation(user_data.document);
}

}
}

#endif
