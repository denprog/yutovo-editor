#ifndef __RESULT_H__
#define __RESULT_H__

#include "code_row.h"
#include "yutovo_calculator/parser_exception.h"
#include <variant>

namespace yutovo
{

class ResultRow : public CodeRow
{
public:
    ResultRow(Document* _document);
    ResultRow(Element* parent);
    ResultRow(const ResultRow& source) = default;

    virtual bool Remake(bool with_elements = false);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);
    virtual void PutError(Error error);
    
    virtual void Reset();

    virtual bool CanSetPrecision();
    virtual void SetPrecision(const int precision);

public:
    bool last_error = false;

protected:
    ParserString last_expression;

    bool delay = false; //don't delay on the first calculation
};

typedef std::shared_ptr<ResultRow> ResultPtr;

class RealResult : public ResultRow
{
public:
    RealResult(Document* _document);
    RealResult(Element* parent);
    RealResult(const RealResult& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    virtual bool CanSetPrecision();
    virtual void SetPrecision(const uint _precision);

protected:
    uint precision = 3;
    AngleMeasure angle_measure = AngleMeasure::RADIAN;
};

class IntegerResult : public ResultRow
{
public:
    IntegerResult(Document* _document);
    IntegerResult(Element* parent);
    IntegerResult(const IntegerResult& source) = default;

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

protected:
    Notation notation = Notation::DECIMAL;
};

class RationalResult : public ResultRow
{
public:
    RationalResult(Document* _document);
    RationalResult(Element* parent);
    RationalResult(const RationalResult& source) = default;

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);
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
    ErrorResult(Element* parent, const Error& error);
    ErrorResult(const ErrorResult& source) = default;
};

class AutoResult : public ResultRow
{
public:
    AutoResult(Document* _document);
    AutoResult(Element* parent);
    AutoResult(Element* parent, uint _precision, AngleMeasure _angle_measure, Notation _notation);
    AutoResult(const AutoResult& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    virtual std::string ToHtml();

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
