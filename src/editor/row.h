#ifndef __ROW_H__
#define __ROW_H__

#include "element.h"
#include "str.h"
#include <boost/serialization/unique_ptr.hpp>

namespace yutovo
{

class Row : public Element
{
public:
    Row(Document* _document);
    Row(Element* _parent);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Remake(bool with_elements, bool with_parent);
    virtual void Normalize(bool with_undo);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);

    virtual bool GetBeginCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetEndCaretState(CaretState& caret_state, Selection* select);

    virtual bool CanContinueSelection();

    virtual void AddEmptyElement();

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
};

typedef std::shared_ptr<Row> RowPtr;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Row* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Row* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Row(p);
}

}
}

#endif
