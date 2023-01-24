#ifndef __MINUS_H__
#define __MINUS_H__

#include "only_shape_formula.h"

namespace yutovo
{

class Minus : public OnlyShapeFormula
{
public:
    Minus(Element* _parent);
    Minus(Document* _document);
    Minus(const Minus& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

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
void save_construct_data(Archive& ar, const yutovo::Minus* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Minus* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Minus(p);
}

}
}

#endif
