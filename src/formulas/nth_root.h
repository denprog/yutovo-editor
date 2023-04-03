#ifndef __NTH_ROOT_H__
#define __NTH_ROOT_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class NthRoot : public MiddleShapeFormula
{
public:
    NthRoot(Element* _parent);
    NthRoot(Document* _document);
    NthRoot(const NthRoot& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual void UpdateLevel(uint8_t _level);

    virtual std::string ToHtml();
    virtual std::u32string ToText();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << first;
        ar << last;
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> first;
        elements->Replace(ElementPtr((Element*)first), 0);
        ar >> last;
        elements->Replace(ElementPtr((Element*)last), 2);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::NthRoot* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::NthRoot* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::NthRoot(p);
    else
        ::new(t)yutovo::NthRoot(user_data.document);
}

}
}

#endif
