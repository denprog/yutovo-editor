#ifndef __SQUARE_ROOT_H__
#define __SQUARE_ROOT_H__

#include "formula.h"
#include "shape.h"

namespace yutovo
{

class SquareRoot : public Formula
{
public:
    SquareRoot(Element* _parent);
    SquareRoot(Document* _document);
    SquareRoot(const SquareRoot& source);

    void Init();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;
    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool AfterInsert(bool with_undo);
    
    virtual bool DeleteElements(bool left, bool with_undo);

    virtual std::string ToHtml();
    virtual std::string ToText();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << last;
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> last;
        elements->Replace(ElementPtr(last), 1);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

protected:
    Element *last;
    Shape *shape;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::SquareRoot* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::SquareRoot* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::SquareRoot(p);
    else
        ::new(t)yutovo::SquareRoot(user_data.document);
}

}
}

#endif
