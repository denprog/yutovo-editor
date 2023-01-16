#ifndef __POWER_H__
#define __POWER_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class Power : public MiddleShapeFormula
{
public:
    Power(Element* _parent);
    Power(Document* _document);
    Power(const Power& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual void UpdateLevel(uint8_t _level);

    virtual std::string ToHtml();
    virtual std::string ToText();

    void AddBase(ElementPtr base);
    void AddExponent(ElementPtr exponent);

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
        elements->Replace(ElementPtr(first), 0);
        ar >> last;
        elements->Replace(ElementPtr(last), 2);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

typedef std::shared_ptr<Power> PowerPtr;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Power* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Power* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Power(p);
}

}
}

#endif
