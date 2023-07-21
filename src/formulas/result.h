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

    void PutUnit(const Result& result);

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
    RealResult(Element* parent, Config::RealResultConfig _config);
    RealResult(const RealResult& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);
    bool SetConfig(const yutovo_calculator::Unit& unit);

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
    Config::RealResultConfig config;
    std::vector<yutovo_calculator::Unit> cast_units;
};

class IntegerResult : public ResultRow
{
public:
    IntegerResult(Document* _document);
    IntegerResult(Element* parent);
    IntegerResult(Element* parent, Config::IntegerResultConfig _config);
    IntegerResult(const IntegerResult& source) = default;

    virtual Element* Clone();

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(Notation result_notation);

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
    Config::IntegerResultConfig config;
};

class RationalResult : public ResultRow
{
public:
    RationalResult(Document* _document);
    RationalResult(Element* parent);
    RationalResult(Element* parent, Config::RationalResultConfig _config);
    RationalResult(const RationalResult& source) = default;

    virtual Element* Clone();

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(FractionForm fraction_form);
    bool SetConfig(const yutovo_calculator::Unit& unit);

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
    Config::RationalResultConfig config;
    std::vector<yutovo_calculator::Unit> cast_units;
};

class ComplexResult : public ResultRow
{
public:
    ComplexResult(Document* _document);
    ComplexResult(Element* parent);
    ComplexResult(Element* parent, Config::ComplexResultConfig _config);
    ComplexResult(const ComplexResult& source) = default;

    virtual Element* Clone();

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);

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
    Config::ComplexResultConfig config;
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
    AutoResult(Element* parent, Config::AutoResultConfig _config);
    AutoResult(const AutoResult& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);
    bool SetConfig(Notation result_notation);
    bool SetConfig(FractionForm fraction_form);
    bool SetConfig(const yutovo_calculator::Unit& unit);

    ResultType GetResultType();

    void GetCastUnits(std::vector<yutovo_calculator::Unit>& cast_units);

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
    Config::AutoResultConfig config;
};

typedef std::shared_ptr<AutoResult> AutoResultPtr;

}

namespace boost
{

namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::RealResult* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::RealResult* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::Config::RealResultConfig config;
    ar >> config;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::RealResult(p, config);
}

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::IntegerResult* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::IntegerResult* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::Config::IntegerResultConfig config;
    ar >> config;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::IntegerResult(p, config);
}

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::RationalResult* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::RationalResult* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::Config::RationalResultConfig config;
    ar >> config;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::RationalResult(p, config);
}

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::ComplexResult* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::ComplexResult* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::Config::ComplexResultConfig config;
    ar >> config;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::ComplexResult(p, config);
}

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
    yutovo::Config::AutoResultConfig config;
    ar >> config;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::AutoResult(p, config);
}

}
}

#endif
