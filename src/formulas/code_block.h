#ifndef __CODE_BLOCK_H__
#define __CODE_BLOCK_H__

#include "../block.h"
#include "../style.h"

namespace yutovo
{

//Group of code paragraphs
class CodeBlock : public Block
{
public:
    CodeBlock(Document* _document, uint _code_id);
    CodeBlock(Element* parent, uint _code_id);
    CodeBlock(Document* _document, Element* parent, uint _code_id);
    CodeBlock(const CodeBlock& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* parent);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool AfterInsert(bool with_undo);

    virtual void GetMargin(int& left, int& top, int& right, int& bottom) const;

    virtual bool HasCaretState();
    virtual bool HasLastCaretState();
    virtual bool CanContinueSelection();
    virtual bool CanContinueVerticalMoving();

    virtual StringFormatPtr GetStringFormat() const;
    virtual FormulaFormatPtr GetFormulaFormat() const;
    virtual ParagraphFormatPtr GetParagraphFormat();

    virtual void AddEmptyElement();

    virtual bool IsFormula();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << code_id;
        ar << (boost::serialization::base_object<Element>(*this), elements);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<Element>(*this), elements);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
    uint code_id = 0; //id for unification of code blocks

    CodeFormatPtr code_format;
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::CodeBlock* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::CodeBlock* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    uint code_id;
    ar >> code_id;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::CodeBlock(user_data.document, p, code_id);
}

}
}

#endif
