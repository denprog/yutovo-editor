#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "middle_shape_formula.h"
#include "code_row.h"

namespace yutovo
{

class Assignment : public MiddleShapeFormula
{
public:
    Assignment(Element* _parent);
    Assignment(Document* _document);
    Assignment(const Assignment& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool DeleteElements(bool left, bool with_undo);
    
    virtual bool AfterInsert(bool with_undo);
    virtual void BeforeDelete();
    virtual void BeforeReplace();
    virtual void AfterReplace();

    virtual void ReSolve(bool if_error = false);
    void PutResult(Result result);

    virtual std::string ToHtml();
    virtual std::u32string ToText();

    virtual void OnChanged(const ElementId _id);

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

private:
    std::u32string last_identifier;
    ParserString last_expression;
    bool last_error = false;
    bool delay = false; //don't delay on the first calculation
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Assignment* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Assignment* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Assignment(p);
    else
        ::new(t)yutovo::Assignment(user_data.document);
}

}
}

#endif
