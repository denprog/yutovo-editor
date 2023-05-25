#ifndef __DIVISION_H__
#define __DIVISION_H__

#include "formula.h"
#include "middle_shape_formula.h"
#include "code_row.h"

namespace yutovo
{

class Division : public MiddleShapeFormula
{
public:
    Division(Element* _parent);
    Division(Document* _document);
    Division(const Division& source);

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select);
    virtual bool GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select);

    virtual std::string ToHtml();
    virtual std::u32string ToText();
    virtual void ToParserString(ParserString& str);

    void AddNumerator(ElementPtr numerator);
    void AddDenomerator(ElementPtr denomerator);

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

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Division* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Division* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    if (p)
        ::new(t)yutovo::Division(p);
    else
        ::new(t)yutovo::Division(user_data.document);
}

}
}

#endif
