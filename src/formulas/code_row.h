#ifndef __CODE_ROW_H__
#define __CODE_ROW_H__

#include "../row.h"

namespace yutovo
{

class CodeRow : public Row
{
public:
    CodeRow(Document* _document);
    CodeRow(Element* parent);
    CodeRow(const CodeRow& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);

    virtual void AddEmptyElement();

    virtual std::string ToHtml();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << (boost::serialization::base_object<Row>(*this), elements);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<Row>(*this), elements);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::CodeRow* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::CodeRow* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::CodeRow(p);
}

}
}

#endif
