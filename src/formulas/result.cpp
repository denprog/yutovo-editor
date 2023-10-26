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
#include "subscript.h"

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
            document->Redraw(err_id, false);
        }
    }
}

void ResultRow::Reset()
{
    elements->Clear();
    last_expression.Reset();
}

void ResultRow::PutUnit(const Result& result)
{
    if (result.unit.IsEmpty())
        return;
    
    const yutovo_calculator::Unit& unit = result.unit;

    ElementPtr numerator(new CodeRow(this));
    numerator->elements->Clear();
    ElementPtr denomerator;
    for (auto& u : unit.unit)
    {
        auto s = ToBasicString(u.first);
        if (u.second > 0)
        {
            if (numerator->elements->Count() > 0)
                numerator->AddElement(ElementPtr(new Multiply(this)));
            if (u.second == 1)
                numerator->AddElement(CodeStringPtr(new CodeString(this, s)));
            else
            {
                PowerPtr p(new Power(this));
                p->AddBase(CodeStringPtr(new CodeString(p.get(), s)));
                p->AddExponent(CodeStringPtr(new CodeString(p.get(), std::to_string(u.second))));
                numerator->AddElement(p);
            }
        }
        else
        {
            if (!denomerator)
            {
                denomerator.reset(new CodeRow(this));
                denomerator->elements->Clear();
            }
            if (denomerator->elements->Count() > 0)
                denomerator->AddElement(ElementPtr(new Multiply(this)));
            if (u.second == -1)
                denomerator->AddElement(CodeStringPtr(new CodeString(this, s)));
            else
            {
                PowerPtr p(new Power(this));
                p->AddBase(CodeStringPtr(new CodeString(p.get(), s)));
                p->AddExponent(CodeStringPtr(new CodeString(p.get(), std::to_string(-u.second))));
                denomerator->AddElement(p);
            }
        }
    }

    ElementPtr s;
    if (unit.system != U"" && unit.system != U"SI")
    {
        s.reset(new Subscript(this));
        s->elements->Get(0)->elements->Clear();
        s->elements->Get(2)->elements->Clear();
        s->elements->Get(2)->elements->Add(CodeStringPtr(new CodeString(s.get(), unit.system)));
        AddElement(s);
    }

    if (denomerator)
    {
        auto* d = new Division(this);
        if (numerator->elements->Count() == 0)
            numerator->AddElement(CodeStringPtr(new CodeString(this, U"1")));
        d->AddNumerator(numerator);
        d->AddDenomerator(denomerator);
        if (s)
            s->elements->Get(0)->AddElement(ElementPtr(d));
        else
            AddElement(ElementPtr(d));
    }
    else
    {
        for (int i = 0; i < numerator->elements->Count(); ++i)
        {
            if (s)
                s->elements->Get(0)->AddElement(numerator->elements->Get(i));
            else
                AddElement(numerator->elements->Get(i));
        }
    }

    elements->Get(0)->SetEditable(false);
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

RealResult::RealResult(Element* parent, Config::RealResultConfig _config) :
    ResultRow(parent), 
    config(_config)
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

void RealResult::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    ResultRow::ToJson(value, alloc);
    config.ToJson(value, alloc);
}

Element* RealResult::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::RealResultConfig config;
    config.FromJson((rapidjson::Value&)value, alloc);
    return new RealResult(parent, config);
}

void RealResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (delay && last_error_code != yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void RealResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;
    cast_units = result.cast_units;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_service::ErrorCode::OK)
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

        PutUnit(result);

        if (config.show_angle_measure)
        {
            std::u32string angle_measure = ToUtfString(result.values["angle_measure"]);
            if (!angle_measure.empty())
                AddElement(ElementPtr(new CodeString(this, U"(" + window->Translate(angle_measure) + U")", GetStringFormat())));
        }
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool RealResult::SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.precision != precision)
        config.precision = precision;
    if (exp != -1 && config.exp != exp)
        config.exp = exp;
    if (result_angle_measure != AngleMeasure::None && config.result_angle_measure != result_angle_measure)
        config.result_angle_measure = result_angle_measure;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool RealResult::SetConfig(const yutovo_calculator::Unit& unit)
{
    if (config.unit == unit)
        return false;
    
    config.unit = unit;
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
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

IntegerResult::IntegerResult(Element* parent, Config::IntegerResultConfig _config) :
    ResultRow(parent)
{
    type = ElementType::INTEGER_RESULT;
    config = _config;
}

Element* IntegerResult::Clone()
{
    return new IntegerResult(*this);
}

void IntegerResult::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    ResultRow::ToJson(value, alloc);
    config.ToJson(value, alloc);
}

Element* IntegerResult::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::IntegerResultConfig config;
    config.FromJson((rapidjson::Value&)value, alloc);
    return new IntegerResult(parent, config);
}

void IntegerResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (delay && last_error_code != yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void IntegerResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_service::ErrorCode::OK)
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
                AddElement(ElementPtr(new CodeString(this, U"(" + window->Translate(notation) + U")", GetStringFormat())));
        }
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    if (elements->Count() > 1)
        elements->Get(1)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool IntegerResult::SetConfig(Notation default_notation, Notation result_notation)
{
    if (config.default_notation == default_notation && config.result_notation == result_notation)
        return false;
    config.default_notation = default_notation;
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

RationalResult::RationalResult(Element* parent, Config::RationalResultConfig _config) :
    ResultRow(parent)
{
    type = ElementType::RATIONAL_RESULT;
    config = _config;
}

Element* RationalResult::Clone()
{
    return new RationalResult(*this);
}

void RationalResult::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    ResultRow::ToJson(value, alloc);
    config.ToJson(value, alloc);
}

Element* RationalResult::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::RationalResultConfig config;
    config.FromJson((rapidjson::Value&)value, alloc);
    return new RationalResult(parent, config);
}

void RationalResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (delay && last_error_code != yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void RationalResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;
    cast_units = result.cast_units;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_service::ErrorCode::OK)
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
        else if (integer.empty())
        {
            AddElement(ElementPtr(ElementPtr(new CodeString(this, numerator))));
        }

        PutUnit(result);
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

bool RationalResult::SetConfig(const yutovo_calculator::Unit& unit)
{
    if (config.unit == unit)
        return false;
    
    config.unit = unit;
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

ComplexResult::ComplexResult(Element* parent, Config::ComplexResultConfig _config) :
    ResultRow(parent)
{
    type = ElementType::COMPLEX_RESULT;
    config = _config;
}

Element* ComplexResult::Clone()
{
    return new ComplexResult(*this);
}

void ComplexResult::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    ResultRow::ToJson(value, alloc);
    config.ToJson(value, alloc);
}

Element* ComplexResult::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::ComplexResultConfig config;
    config.FromJson((rapidjson::Value&)value, alloc);
    return new ComplexResult(parent, config);
}

bool ComplexResult::SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.precision != precision)
        config.precision = precision;
    if (exp != -1 && config.exp != exp)
        config.exp = exp;
    if (result_angle_measure != AngleMeasure::None && config.result_angle_measure != result_angle_measure)
        config.result_angle_measure = result_angle_measure;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
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
        AddElement(ElementPtr(new CodeString(this, window->Translate(ErrorCodeToString(error.parser_error_code)))));
    else
        AddElement(ElementPtr(new CodeString(this, window->Translate(ErrorCodeToString(error.error_code)))));
}

//AutoResult

AutoResult::AutoResult(Document* _document) :
    ResultRow(_document),
    config(_document->config.auto_result)
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
        config = parent->document->config.auto_result;
}

AutoResult::AutoResult(Element* parent, Config::AutoResultConfig _config) :
    ResultRow(parent),
    config(_config)
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

void AutoResult::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value _id(IdToString(id).c_str(), alloc);
    value.AddMember("id", _id, alloc);
    value.AddMember("type", (int)type, alloc);
    config.ToJson(value, alloc);
}

Element* AutoResult::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::AutoResultConfig config;
    config.FromJson((rapidjson::Value&)value, alloc);
    return new AutoResult(parent, config);
}

void AutoResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    document->Solve(id, ((CodeBlock*)code.get())->code_id, config, last_expression.Text(), 
        (delay && last_error_code != yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void AutoResult::PutResult(Result result)
{
    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_service::ErrorCode::OK)
        PutError(result.error); //put error message
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

bool AutoResult::SetConfig(const int precision, const int exp, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.real_result.precision != precision)
        config.real_result.precision = precision;
    if (exp != -1 && config.real_result.exp != exp)
        config.real_result.exp = exp;
    if (result_angle_measure != AngleMeasure::None && config.real_result.result_angle_measure != result_angle_measure)
        config.real_result.result_angle_measure = result_angle_measure;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool AutoResult::SetConfig(Notation default_notation, Notation result_notation)
{
    if (config.integer_result.default_notation == default_notation && config.integer_result.result_notation == result_notation)
        return false;
    
    config.integer_result.default_notation = default_notation;
    config.integer_result.result_notation = result_notation;

    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool AutoResult::SetConfig(FractionForm fraction_form)
{
    if (config.rational_result.fraction_form == fraction_form)
        return false;
    
    config.rational_result.fraction_form = fraction_form;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool AutoResult::SetConfig(const yutovo_calculator::Unit& unit)
{
    if (config.real_result.unit == unit && config.rational_result.unit == unit)
        return false;
    
    config.real_result.unit = unit;
    config.rational_result.unit = unit;
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

ResultType AutoResult::GetResultType()
{
    if (elements->Count() == 0)
        return ResultType::NONE;
    switch (elements->Get(0)->type)
    {
    case ElementType::REAL_RESULT:
        return ResultType::REAL;
    case ElementType::INTEGER_RESULT:
        return ResultType::INTEGER;
    case ElementType::RATIONAL_RESULT:
        return ResultType::RATIONAL;
    case ElementType::COMPLEX_RESULT:
        return ResultType::COMPLEX;
    }
    return ResultType::AUTO;
}

void AutoResult::GetCastUnits(std::vector<yutovo_calculator::Unit>& cast_units)
{
    if (elements->Count() == 0)
        return;
    auto el = elements->Get(0);
    switch (el->type)
    {
    case ElementType::REAL_RESULT:
        cast_units = ((RealResult*)el.get())->cast_units;
        break;
    case ElementType::RATIONAL_RESULT:
        cast_units = ((RationalResult*)el.get())->cast_units;
        break;
    default:
        break;
    }
}

std::string AutoResult::ToHtml()
{
    if (elements->Count() > 0)
        return elements->Get(0)->ToHtml();
    return "<mrow></mrow>";
}

}
