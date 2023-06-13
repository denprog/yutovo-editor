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

    virtual void PutWaitingSymbol();
    virtual void PutResult(Result result);
    virtual void PutError(Error error);
    
    virtual void Reset();

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
    RealResult(Element* parent, Config::RealResult _config);
    RealResult(const RealResult& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);

public:
    Config::RealResult config;
};

class IntegerResult : public ResultRow
{
public:
    IntegerResult(Document* _document);
    IntegerResult(Element* parent);
    IntegerResult(Element* parent, Config::IntegerResult _config);
    IntegerResult(const IntegerResult& source) = default;

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(Notation result_notation);

public:
    Config::IntegerResult config;
};

class RationalResult : public ResultRow
{
public:
    RationalResult(Document* _document);
    RationalResult(Element* parent);
    RationalResult(Element* parent, Config::RationalResult _config);
    RationalResult(const RationalResult& source) = default;

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(FractionForm fraction_form);

public:
    Config::RationalResult config;
};

class ComplexResult : public ResultRow
{
public:
    ComplexResult(Document* _document);
    ComplexResult(Element* parent);
    ComplexResult(const ComplexResult& source) = default;

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);

public:
    Config::ComplexResult config;
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
    AutoResult(Element* parent, Config::AutoResult _config);
    AutoResult(const AutoResult& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);
    bool SetConfig(Notation result_notation);
    bool SetConfig(FractionForm fraction_form);

    ResultType GetResultType();

    virtual std::string ToHtml();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << config;
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
    Config::AutoResult config;
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
    yutovo::Config::AutoResult config;
    ar >> config;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::AutoResult(p, config);
}

}
}

#endif
