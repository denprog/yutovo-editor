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
    Equation(Element* _parent, yutovo_service::ResultType _result_type);
    Equation(Document* _document, yutovo_service::ResultType _result_type);
    Equation(const Equation& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    virtual bool DeleteElements(bool left, bool with_undo);
    
    virtual bool AfterInsert(bool with_undo);
    virtual void BeforeReplace();
    virtual void AfterReplace();

    virtual void ReSolve(bool if_error = false);

    virtual bool Depends(const std::string& identifier);
    
    virtual std::string ToHtml();
    virtual std::u32string ToText();

    virtual void OnChanged(const ElementId _id);

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << result_type;
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
    yutovo_service::ResultType result_type;
    bool ready = true;
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
    int result_type;
    ar >> result_type;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Equation(p, (yutovo_service::ResultType)result_type);
    else
        ::new(t)yutovo::Equation(user_data.document, (yutovo_service::ResultType)result_type);
}

}
}

#endif
