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
    ErrorResult(Element* parent, const ErrorCode error_code);
    ErrorResult(const ErrorResult& source) = default;
};

class AutoResult : public Element
{
public:
    AutoResult(Document* _document);
    AutoResult(Element* parent);
    AutoResult(const AutoResult& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);

    void Solve(const std::string& expression, ResultType result_type);
    void PutResult(Result result);

private:
    uint precision = 3;
    AngleMeasure angle_measure = AngleMeasure::RADIAN;
    Notation notation = Notation::DECIMAL;

    std::string last_expression;
};

typedef std::shared_ptr<AutoResult> AutoResultPtr;

}

#endif
