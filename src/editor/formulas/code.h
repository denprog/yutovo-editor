#ifndef __CODE_H__
#define __CODE_H__

#include "code_row.h"

namespace yutovo
{

class Code : public CodeRow
{
public:
    Code(Document* _document);
    Code(Element* parent);
    Code(const Code& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;

    virtual bool AfterInsert(bool with_undo);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueSelection();

    virtual StringFormatPtr GetStringFormat();
    virtual FormulaFormatPtr GetFormulaFormat() const;

    virtual std::string ToHtml();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << (boost::serialization::base_object<CodeRow>(*this), elements);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<CodeRow>(*this), elements);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

protected:
    FormulaFormatPtr code_format;
    FormulaFormatPtr formula_format;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::Code* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::Code* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::Code(p);
}

}
}

#endif
