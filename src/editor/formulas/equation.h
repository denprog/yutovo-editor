#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "middle_shape_formula.h"
#include "result.h"

namespace yutovo
{

class Equation : public MiddleShapeFormula
{
public:
    Equation(Element* _parent);
    Equation(Document* _document, ResultType _result_type);
    Equation(const Equation& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml();
    virtual std::string ToText();

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

protected:
    AutoResultPtr auto_result;
    ResultType result_type;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Equation* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Equation* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Equation(p);
}

}
}

#endif
