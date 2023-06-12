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
        PutWaitingSymbol();
        parent->Remake(true);
        changed = true;
    }
    return changed;
}

void ResultRow::PutWaitingSymbol()
{
    elements->Clear();
    elements->Add(ElementPtr(new CodeString(this, "~")));
    elements->Get(0)->SetEditable(false);
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
    ResultRow(_document),
    config(_document->config.real_result)
{
    type = ElementType::REAL_RESULT;
}

RealResult::RealResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::REAL_RESULT;

    if (parent)
        config = parent->document->config.real_result;
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
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), delay ? document->config.solve_delay : 0);
    delay = true;
}

void RealResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

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

        if (!exponent.empty() && exponent != "0")
        {
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
        
        if (config.show_angle_measure)
        {
            std::u32string angle_measure = ToUtfString(result.values["angle_measure"]);
            if (!angle_measure.empty())
                AddElement(ElementPtr(new CodeString(this, U"(" + window->GetString(angle_measure) + U")", GetStringFormat())));
        }
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
    ResultRow(_document),
    config(_document->config.integer_result)
{
    type = ElementType::INTEGER_RESULT;
}

IntegerResult::IntegerResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::INTEGER_RESULT;

    if (parent)
        config = parent->document->config.integer_result;
}

void IntegerResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), delay ? document->config.solve_delay : 0);
    delay = true;
}

void IntegerResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

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
        
        if (config.show_notation)
        {
            std::u32string notation = ToUtfString(result.values["notation"]);
            if (!notation.empty())
                AddElement(ElementPtr(new CodeString(this, U"(" + window->GetString(notation) + U")", GetStringFormat())));
        }
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool IntegerResult::SetConfig(Notation result_notation)
{
    if (config.result_notation == result_notation)
        return false;
    config.result_notation = result_notation;
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

//RationalResult

RationalResult::RationalResult(Document* _document) :
    ResultRow(_document),
    config(_document->config.rational_result)
{
    type = ElementType::RATIONAL_RESULT;
}

RationalResult::RationalResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::RATIONAL_RESULT;

    if (parent)
        config = parent->document->config.rational_result;
}

void RationalResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), delay ? document->config.solve_delay : 0);
    delay = true;
}

void RationalResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);

        std::string integer = result.values["integer"];
        std::string numerator = result.values["numerator"];
        std::string denomerator = result.values["denomerator"];

        elements->Clear();
        if (numerator[0] == '-')
        {
            elements->Insert(ElementPtr(new Minus(this)), 0);
            numerator = numerator.substr(1, numerator.size() - 1);
        }

        if (!integer.empty())
        {
            AddElement(ElementPtr(ElementPtr(new CodeString(this, integer))));
        }

        if (numerator != "0")
        {
            if (denomerator == "1")
            {
                AddElement(ElementPtr(ElementPtr(new CodeString(this, numerator))));
            }
            else
            {
                Division* d = new Division(this);
                AddElement(ElementPtr(d));
                d->AddNumerator(ElementPtr(new CodeString(this, numerator)));
                d->AddDenomerator(ElementPtr(new CodeString(this, denomerator)));
            }
        }
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool RationalResult::SetConfig(FractionForm fraction_form)
{
    if (config.fraction_form == fraction_form)
        return false;
    config.fraction_form = fraction_form;
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
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
    ResultRow(_document),
    auto_config(_document->config.auto_result)
{
    type = ElementType::AUTO_RESULT;
    remake_always = true;
}

AutoResult::AutoResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::AUTO_RESULT;
    remake_always = true;

    if (parent)
        auto_config = parent->document->config.auto_result;
}

AutoResult::AutoResult(Element* parent, Config::AutoResult _auto_config) :
    ResultRow(parent),
    auto_config(_auto_config)
{
    type = ElementType::AUTO_RESULT;
    remake_always = true;
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
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, auto_config, last_expression.Text(), delay ? document->config.solve_delay : 0);
    delay = true;
}

void AutoResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error = result.error.error_code != ErrorCode::OK;
    if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

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
