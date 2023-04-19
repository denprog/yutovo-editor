#ifndef __SUBSCRIPT_H__
#define __SUBSCRIPT_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class Subscript : public MiddleShapeFormula
{
public:
    Subscript(Element* _parent);
    Subscript(Document* _document);
    Subscript(const Subscript& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual void AfterChildInsert(const ElementId child_id, bool with_undo);

    virtual void UpdateLevel(uint8_t _level);

    virtual std::string ToHtml();
    virtual std::u32string ToText();
    virtual void ToParserString(ParserString& str);

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

typedef std::shared_ptr<Subscript> SubscriptPtr;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Subscript* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Subscript* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Subscript(p);
    else
        ::new(t)yutovo::Subscript(user_data.document);
}

}
}

#endif
