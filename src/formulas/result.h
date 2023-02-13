#ifndef __RESULT_H__
#define __RESULT_H__

#include "code_row.h"
#include <variant>

namespace yutovo
{

class ResultRow : public CodeRow
{
public:
    ResultRow(Document* _document);
    ResultRow(Element* parent);
    ResultRow(const ResultRow& source) = default;
};

typedef std::shared_ptr<ResultRow> ResultPtr;

class RealResult : public ResultRow
{
public:
    RealResult(Document* _document);
    RealResult(Element* parent, const std::string& mantissa, const std::string& exponent);
    RealResult(const RealResult& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);
};

class IntegerResult : public ResultRow
{
public:
    IntegerResult(Document* _document);
    IntegerResult(Element* parent, const std::string& value);
    IntegerResult(const IntegerResult& source) = default;
};

class RationalResult : public ResultRow
{
public:
    RationalResult(Document* _document);
    RationalResult(Element* parent, const std::string& numerator, const std::string& denomerator);
    RationalResult(const RationalResult& source) = default;
};

class ComplexResult : public ResultRow
{
public:
    ComplexResult(Document* _document);
    ComplexResult(Element* parent);
    ComplexResult(const ComplexResult& source) = default;
};

class ErrorResult : public ResultRow
{
public:
    ErrorResult(Document* _document);
    ErrorResult(Element* parent, const yutovo_service::ErrorCode error_code);
    ErrorResult(Element* parent, const yutovo_calculator::ParserExceptionCode parser_error_code);
    ErrorResult(const ErrorResult& source) = default;
};

class AutoResult : public Element
{
public:
    AutoResult(Document* _document);
    AutoResult(Element* parent);
    AutoResult(Element* parent, uint _precision, AngleMeasure _angle_measure, Notation _notation);
    AutoResult(const AutoResult& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    void Solve(const std::string& expression, yutovo_service::ResultType result_type);
    void PutResult(Result result);

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << precision;
        ar << angle_measure;
        ar << notation;
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    uint precision = 3;
    AngleMeasure angle_measure = AngleMeasure::RADIAN;
    Notation notation = Notation::DECIMAL;

    std::string last_expression;
};

typedef std::shared_ptr<AutoResult> AutoResultPtr;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::AutoResult* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::AutoResult* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    uint precision;
    ar >> precision;
    yutovo::AngleMeasure angle_measure;
    ar >> angle_measure;
    yutovo::Notation notation;
    ar >> notation;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::AutoResult(p, precision, angle_measure, notation);
}

}
}

#endif
