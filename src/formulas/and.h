#ifndef __AND_H__
#define __AND_H__

#include "only_shape_formula.h"

namespace yutovo
{

class And : public OnlyShapeFormula
{
public:
    And(Element* _parent);
    And(Document* _document);
    And(const And& source);

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
void save_construct_data(Archive& ar, const yutovo::And* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::And* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::And(p);
    else
        ::new(t)yutovo::And(user_data.document);
}

}
}

#endif
