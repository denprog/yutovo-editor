#ifndef __TEXT_H__
#define __TEXT_H__

#include <vector>
#include "element.h"
#include "util.h"
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>

namespace yutovo
{

//The root element of the editor
class Text : public Element
{
public:
    Text(Document* _document);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);

    virtual std::string ToHtml();

    virtual Rect GetAbsoluteRect() const;

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

private:
    TextFormatPtr format;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Text* t, const unsigned int version)
{
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Text* t, const unsigned int version)
{
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Text(user_data.document);
}

}
}

#endif
