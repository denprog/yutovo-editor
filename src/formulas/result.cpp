#include "result.h"
#include "multiply.h"
#include "plus.h"
#include "minus.h"
#include "power.h"
#include "division.h"
#include "../solver.h"
#include "code_string.h"
#include "code_block.h"
#include "equation.h"

namespace yutovo
{

using namespace yutovo_service;

//ResultRow

ResultRow::ResultRow(Document* _document) : 
    CodeRow(_document)
{
}

ResultRow::ResultRow(Element* parent) :
    CodeRow(parent)
{
}

bool ResultRow::Remake(bool with_elements)
{
    bool changed = CodeRow::Remake(with_elements);

    if (elements->Count() == 0)
    {
        //put waiting symbol
        elements->Add(ElementPtr(new CodeString(this, "~")));
        elements->Get(0)->SetEditable(false);
        parent->Remake(true);
        changed = true;
    }
    return changed;
}

void ResultRow::Solve(const ParserString& expression)
{
}

void ResultRow::PutResult(Result result)
{
}

void ResultRow::PutError(Error error)
{
    elements->Add(ElementPtr(new ErrorResult(this, error)));
    ElementId err_id = last_expression.GetElement(error.pos);
    if (!err_id.empty())
    {
        auto el = document->GetElement(err_id);
        if (el)
        {
            document->RemoveErrorMarks(parent->parent->id);
            document->AddErrorMark(err_id, 0, el->elements->Count());
        }
    }
}

void ResultRow::Reset()
{
    elements->Clear();
    last_expression.Reset();
}

bool ResultRow::CanSetPrecision()
{
    return false;
}

void ResultRow::SetPrecision(const int precision)
{
}

//RealResult

RealResult::RealResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::REAL_RESULT;
}

RealResult::RealResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::REAL_RESULT;
}

Element* RealResult::Clone()
{
    return new RealResult(*this);
}

Element* RealResult::Create(Element* _parent)
{
    return new RealResult(_parent);
}

void RealResult::Solve(const ParserString& expression)
{
    if (last_expression == expression)
        return;
    last_expression = expression;

    elements->Clear();
    Remake(false);

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, ResultType::REAL, precision, angle_measure, Notation::DECIMAL, last_expression.Text(), 
        delay ? document->config.solve_delay : 0);
    delay = true;
}

void RealResult::PutResult(Result result)
{
    ElementPtr eq = document->FindParent(id, ElementType::EQUATION);
    ((Equation*)eq.get())->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
        return;

    elements->Clear();
    if (result.error.error_code != ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);

        std::string mantissa = result.values["mantissa"];
        std::string exponent = result.values["exponent"];

        AddElement(ElementPtr(new CodeString(this, mantissa)));

        if (exponent.empty() || exponent == "0")
            return;
        
        //make mantissa*10^exponent
        AddElement(ElementPtr(new Multiply(this)));
        PowerPtr p(new Power(this));
        AddElement(p);
        p->AddBase(CodeStringPtr(new CodeString(p.get(), "10")));
        if (exponent[0] == '-')
        {
            p->AddExponent(ElementPtr(new Minus(p.get())));
            p->AddExponent(CodeStringPtr(new CodeString(p.get(), exponent.substr(1, exponent.size() - 1))));
        }
        else
            p->AddExponent(CodeStringPtr(new CodeString(p.get(), exponent)));
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool RealResult::CanSetPrecision()
{
    return true;
}

void RealResult::SetPrecision(const uint _precision)
{
}

//IntegerResult

IntegerResult::IntegerResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::INTEGER_RESULT;
}

IntegerResult::IntegerResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::INTEGER_RESULT;
}

void IntegerResult::Solve(const ParserString& expression)
{
    if (last_expression == expression)
        return;
    last_expression = expression;

    elements->Clear();
    Remake(false);

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, ResultType::INTEGER, 0, AngleMeasure::RADIAN, notation, last_expression.Text(), 
        delay ? document->config.solve_delay : 0);
    delay = true;
}

void IntegerResult::PutResult(Result result)
{
    ElementPtr eq = document->FindParent(id, ElementType::EQUATION);
    ((Equation*)eq.get())->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
        return;

    elements->Clear();
    if (result.error.error_code != ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        std::string value = result.values["value"];
        elements->Clear();
        if (value[0] == '-')
        {
            AddElement(ElementPtr(new Minus(this)));
            AddElement(CodeStringPtr(new CodeString(this, value.substr(1, value.size() - 1))));
        }
        else
            AddElement(ElementPtr(new CodeString(this, value)));
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

//RationalResult

RationalResult::RationalResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::RATIONAL_RESULT;
}

RationalResult::RationalResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::RATIONAL_RESULT;
}

void RationalResult::Solve(const ParserString& expression)
{
    if (last_expression == expression)
        return;
    last_expression = expression;

    elements->Clear();
    Remake(false);

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, ResultType::RATIONAL, 0, AngleMeasure::RADIAN, Notation::DECIMAL, last_expression.Text(), 
        delay ? document->config.solve_delay : 0);
    delay = true;
}

void RationalResult::PutResult(Result result)
{
    ElementPtr eq = document->FindParent(id, ElementType::EQUATION);
    ((Equation*)eq.get())->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
        return;

    elements->Clear();
    if (result.error.error_code != ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);
        std::string numerator = result.values["numerator"];
        std::string denomerator = result.values["denomerator"];
        elements->Clear();
        Division* d = new Division(this);
        AddElement(ElementPtr(d));
        if (numerator[0] == '-')
        {
            elements->Insert(ElementPtr(new Minus(this)), 0);
            d->AddNumerator(ElementPtr(new CodeString(this, numerator.substr(1, numerator.size() - 1))));
        }
        else
            d->AddNumerator(ElementPtr(new CodeString(this, numerator)));
        d->AddDenomerator(ElementPtr(new CodeString(this, denomerator)));
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

//ComplexResult

ComplexResult::ComplexResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::COMPLEX_RESULT;
}

ComplexResult::ComplexResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::COMPLEX_RESULT;
}

//ErrorResult

ErrorResult::ErrorResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::ERROR_RESULT;
}

ErrorResult::ErrorResult(Element* parent, const Error& error) :
    ResultRow(parent)
{
    type = ElementType::ERROR_RESULT;
    elements->Clear();

    if (!error.description.empty())
        AddElement(ElementPtr(new CodeString(this, error.description, GetStringFormat())));
    else if (error.parser_error_code != yutovo_calculator::ParserExceptionCode::None)
        AddElement(ElementPtr(new CodeString(this, ErrorCodeToString(error.parser_error_code))));
    else
        AddElement(ElementPtr(new CodeString(this, ErrorCodeToString(error.error_code))));
}

//AutoResult

AutoResult::AutoResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::AUTO_RESULT;
    remake_always = true;
}

AutoResult::AutoResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::AUTO_RESULT;
    remake_always = true;
}

AutoResult::AutoResult(Element* parent, uint _precision, AngleMeasure _angle_measure, Notation _notation) :
    ResultRow(parent),
    precision(_precision),
    angle_measure(_angle_measure),
    notation(_notation)
{
    type = ElementType::AUTO_RESULT;
    remake_always = true;
}

AutoResult::AutoResult(const AutoResult& source) :
    ResultRow(source),
    precision(source.precision),
    angle_measure(source.angle_measure),
    notation(source.notation)
{
}

Element* AutoResult::Clone()
{
    return new AutoResult(*this);
}

Element* AutoResult::Create(Element* _parent)
{
    return new AutoResult(_parent);
}

void AutoResult::Solve(const ParserString& expression)
{
    if (last_expression == expression)
        return;
    last_expression = expression;

    elements->Clear();
    Remake(false);

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, ResultType::AUTO, precision, angle_measure, notation, last_expression.Text(), 
        delay ? document->config.solve_delay : 0);
    delay = true;
}

void AutoResult::PutResult(Result result)
{
    ElementPtr eq = document->FindParent(id, ElementType::EQUATION);
    ((Equation*)eq.get())->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
        return;

    elements->Clear();
    if (result.error.error_code != ErrorCode::OK)
    {
        //put error message
        elements->Add(ElementPtr(new ErrorResult(this, result.error)));
        ElementId err_id = last_expression.GetElement(result.error.pos);
        if (!err_id.empty())
        {
            auto el = document->GetElement(err_id);
            if (el)
            {
                document->RemoveErrorMarks(parent->parent->id);
                document->AddErrorMark(err_id, 0, el->elements->Count());
            }
        }
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);
        //put element of returned result type
        ResultPtr result_row;
        switch (result.type)
        {
        case ResultType::REAL:
            result_row.reset(new RealResult(this));
            break;
        case ResultType::INTEGER:
            result_row.reset(new IntegerResult(this));
            break;
        case ResultType::RATIONAL:
            result_row.reset(new RationalResult(this));
            break;
        case ResultType::COMPLEX:
        default:
            return;
        }
        elements->Add(result_row);
        result_row->PutResult(result);
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

std::string AutoResult::ToHtml()
{
    if (elements->Count() > 0)
        return elements->Get(0)->ToHtml();
    return "<mrow></mrow>";
}

}
