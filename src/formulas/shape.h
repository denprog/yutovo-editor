#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "../element.h"
#include <functional>

namespace yutovo
{

#define ROOT_X_LEFT_OFFSET 2
#define ROOT_Y_OFFSET 4
#define ROOT_X_RIGHT_OFFSET 2

class Shape : public Element
{
public:
    Shape(Element* _parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool HasCaretState();

    virtual bool GetElementAtCoords(const int x, const int y, ElementId& _id);

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
    std::function<void(const Rect& rect)> draw_func;
};

typedef std::shared_ptr<Shape> ShapePtr;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Shape* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Shape* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Shape(p);
}

}
}

#endif
