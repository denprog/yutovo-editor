#ifndef __RESULT_H__
#define __RESULT_H__

#include "code_column.h"
#include "yutovo_calculator/parser_exception.h"
#include <variant>

namespace yutovo
{

class ResultRow : public CodeColumn
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

    virtual void AfterReplace();
    virtual void BeforePaste();
    virtual void ElementIdChanged();

    virtual void AddElement(ElementPtr element);
    
    void PutUnit(const Result& result);
    void AddExponent(Element* parent, const std::string& exponent);
    void AddExponent(const std::string& exponent);
    void AddNumber(const std::string& number);

    void AddResult();

protected:
    ElementPtr GetCurRow();

public:
    yutovo_service::ErrorCode last_error_code = yutovo_service::ErrorCode::OK;

protected:
    ParserString last_expression;

    bool delay = false; //don't delay on the first calculation
    bool next_result = false;

    ElementId solving_id;
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

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    virtual void BeforePaste();

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);
    bool SetConfig(const yutovo_calculator::Unit& unit);

public:
    Config::RealResultConfig config;
    std::vector<yutovo_calculator::Unit> cast_units;
    bool with_angle_measure = false;
};

class IntegerResult : public ResultRow
{
public:
    IntegerResult(Document* _document);
    IntegerResult(Element* parent);
    IntegerResult(Element* parent, Config::IntegerResultConfig _config);
    IntegerResult(const IntegerResult& source) = default;

    virtual Element* Clone();

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    virtual void BeforePaste();

    bool SetConfig(Notation default_notation, Notation result_notation);

public:
    Config::IntegerResultConfig config;
    bool with_notation = false;
};

class RationalResult : public ResultRow
{
public:
    RationalResult(Document* _document);
    RationalResult(Element* parent);
    RationalResult(Element* parent, Config::RationalResultConfig _config);
    RationalResult(const RationalResult& source) = default;

    virtual Element* Clone();

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    bool SetConfig(FractionForm fraction_form);
    bool SetConfig(const yutovo_calculator::Unit& unit);

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

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    virtual void BeforePaste();

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);
    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure, ComplexForm form, uint max_count);
    bool SetConfig(ComplexForm form);

public:
    Config::ComplexResultConfig config;
    bool with_angle_measure = false;
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

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Solve(const ParserString& expression);

    virtual void PutResult(Result result);

    virtual void AfterReplace();
    virtual void BeforePaste();

    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure);
    bool SetConfig(Notation default_notation, Notation result_notation);
    bool SetConfig(FractionForm fraction_form);
    bool SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure, ComplexForm form, uint max_count);
    bool SetConfig(ComplexForm form);
    bool SetConfig(const yutovo_calculator::Unit& unit);

    ResultType GetResultType();

    void GetCastUnits(std::vector<yutovo_calculator::Unit>& cast_units);

    virtual std::string ToHtml();

public:
    Config::AutoResultConfig config;
};

typedef std::shared_ptr<AutoResult> AutoResultPtr;

}

#endif
