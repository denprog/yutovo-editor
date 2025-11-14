/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

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
#include "brackets.h"
#include "comma.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

using namespace yutovo_solver;

//ResultRow

ResultRow::ResultRow(Document* _document) : 
    CodeColumn(_document)
{
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
}

ResultRow::ResultRow(Element* parent) :
    CodeColumn(parent)
{
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
}

bool ResultRow::Remake(bool with_elements)
{
    bool changed = CodeColumn::Remake(with_elements);

    if (elements->Count() == 0)
    {
        PutWaitingSymbol();
        parent->Remake(true);
        changed = true;
    }
    return changed;
}

void ResultRow::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!document->saving)
    {
        CodeColumn::ToJson(value, alloc);
        return;
    }

    rapidjson::Value _id(ElementIdToString(id).c_str(), alloc);
    value.AddMember("id", _id, alloc);
    value.AddMember("type", (int)type, alloc);

    rapidjson::Value arr(rapidjson::kArrayType);
    value.AddMember("elements", arr, alloc);
}

void ResultRow::PutWaitingSymbol()
{
    bool c = caret->IsInsideElement(id);
    elements->Clear();
    elements->Add(ElementPtr(new CodeString(this, "~")));
    if (c)
        caret->SetState(id);
    elements->Get(0)->SetEditable(false);
    solving = true;
    parent->parent->Remake(true);
    parent->parent->UpdateRect();
}

void ResultRow::Solve(const ParserString& expression)
{
}

void ResultRow::PutResult(Result& result)
{
    solving = false;
}

void ResultRow::PutError(const Error& error)
{
    elements->Add(ElementPtr(new ErrorResult(this, error)));
    ElementId err_id = last_expression.GetElement(error.pos, error.size);
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

void ResultRow::BeforeReplace()
{
    replacing = true;
}

void ResultRow::AfterReplace()
{
    replacing = false;
    if (FindParent(ElementType::EQUATION) != ElementId{})
        return;
    
    //move the child elements outside
    int pos = parent->elements->GetChildPos(id);
    for (int i = 0; i < elements->Count();)
    {
        auto el = elements->Get(i);
        el->SetEditable(true);
        parent->elements->Move(el, pos++);
    }
    parent->elements->Remove(id);
}

void ResultRow::BeforePaste()
{
    //move child elements outside and remove this element
    int c = parent->elements->Count();
    for (int i = 0, j = 0; i < elements->Count();)
        parent->elements->Move(elements->Get(0), c + j++);
    parent->elements->RemoveAt(c - 1, 1);
}

void ResultRow::BeforeDelete()
{
    CodeColumn::BeforeDelete();
    
    if (solving && !replacing)
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        document->BreakSolving(solving_id, guid, ((CodeBlock*)code.get())->code_id);
        solving = false;
    }
}

void ResultRow::LogicalIdChanged(const LogicalId& last_id)
{
    if (!solving_id.empty())
        document->UpdateSolveId(guid, logical_id);
}

void ResultRow::AddElement(ElementPtr element)
{
    GetCurRow()->AddElement(element);
}

void ResultRow::PutUnit(const Value& value)
{
    if (value.unit.IsEmpty())
        return;
    
    const yutovo_calculator::Unit& unit = value.unit;

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

void ResultRow::AddExponent(Element* parent, const std::string& exponent)
{
    if (exponent.empty() | exponent == "0")
        return;
    
    //make mantissa*10^exponent
    parent->AddElement(ElementPtr(new Multiply(this)));
    PowerPtr p(new Power(parent));
    parent->AddElement(p);
    p->AddBase(CodeStringPtr(new CodeString(p.get(), "10")));
    if (exponent[0] == '-')
    {
        p->AddExponent(ElementPtr(new Minus(p.get())));
        p->AddExponent(CodeStringPtr(new CodeString(p.get(), exponent.substr(1, exponent.size() - 1))));
    }
    else
        p->AddExponent(CodeStringPtr(new CodeString(p.get(), exponent)));
}

void ResultRow::AddExponent(const std::string& exponent)
{
    AddExponent(GetCurRow().get(), exponent);
}

void ResultRow::AddNumber(const std::string& number)
{
    if (!number.empty() && number[0] == '-')
    {
        AddElement(ElementPtr(new Minus(this)));
        AddElement(CodeStringPtr(new CodeString(this, number.substr(1, number.size() - 1))));
    }
    else
        AddElement(ElementPtr(new CodeString(this, number)));
}

void ResultRow::AddOpenSquareBracket()
{
    AddElement(ElementPtr(new OpenBracket(this, ElementType::OPEN_SQUARE_BRACKET)));
}

void ResultRow::AddCloseSquareBracket()
{
    AddElement(ElementPtr(new CloseBracket(this, ElementType::CLOSE_SQUARE_BRACKET)));
}

void ResultRow::AddComma()
{
    AddElement(ElementPtr(new Comma(this)));
}

void ResultRow::AddResult()
{
    next_result = true; //adding next elements will be proceed on the next row
}

ElementPtr ResultRow::GetCurRow()
{
    if (elements->Count() == 0 || next_result)
    {
        next_result = false;
        AddEmptyElement();
    }
    return elements->Get(elements->Count() - 1);
}

int ResultRow::GetCodeId()
{
    auto code = document->FindParent(id, ElementType::CODE_BLOCK);
    return ((CodeBlock*)code.get())->code_id;
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
    value.AddMember("with_angle_measure", with_angle_measure, alloc);
}

Element* RealResult::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::RealResultConfig config;
    config.FromJson(value, alloc);
    auto* p = new RealResult(parent, config);
    if (value.HasMember("with_angle_measure") && value["with_angle_measure"].IsBool())
        p->with_angle_measure = value["with_angle_measure"].GetBool();
    return p;
}

void RealResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    solving_id = logical_id;
    document->Solve(logical_id, guid, GetCodeId(), config, !GetParent(1)->visible, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void RealResult::PutResult(Result& result)
{
    ResultRow::PutResult(result);

    solving_id.clear();

    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;
    if (result.values.size() == 1)
        cast_units = result.values[0].cast_units;
    else
        cast_units.clear();

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);

        if (result.values.empty())
            return;
        Value& value = result.values[0];
        std::string mantissa = value.value["mantissa"];
        std::string exponent = value.value["exponent"];

        AddNumber(mantissa);
        AddExponent(exponent);
        PutUnit(value);

        if (config.show_angle_measure && result.values.size() == 1)
        {
            std::string angle_measure = AngleMeasureToString(result.values[0].angle_measure);
            with_angle_measure = !angle_measure.empty();
            if (!angle_measure.empty())
                AddElement(ElementPtr(new CodeString(this, "(" + angle_measure + ")", GetStringFormat())));
        }
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool RealResult::SetConfig(const int precision, const int exp, const AngleMeasure default_angle_measure, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.precision != precision)
        config.precision = precision;
    if (exp != -1 && config.exp != exp)
        config.exp = exp;
    if (default_angle_measure != AngleMeasure::None && config.default_angle_measure != default_angle_measure)
        config.default_angle_measure = default_angle_measure;
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
    value.AddMember("with_notation", with_notation, alloc);
}

Element* IntegerResult::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::IntegerResultConfig config;
    config.FromJson(value, alloc);
    auto* p = new IntegerResult(parent, config);
    if (value.HasMember("with_notation") && value["with_notation"].IsBool())
        p->with_notation = value["with_notation"].GetBool();
    return p;
}

void IntegerResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    solving_id = logical_id;
    document->Solve(logical_id, guid, GetCodeId(), config, !GetParent(1)->visible, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void IntegerResult::PutResult(Result& result)
{
    ResultRow::PutResult(result);

    solving_id.clear();

    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        if (result.values.empty())
            return;
        Value& value = result.values[0];
        std::string val = value.value["value"];
        elements->Clear();
        AddNumber(val);

        config.result_notation = value.notation;
        
        if (config.show_notation)
        {
            std::string notation = NotationToString(value.notation);
            with_notation = !notation.empty();
            if (!notation.empty())
                AddElement(ElementPtr(new CodeString(this, "(" + notation + ")", GetStringFormat())));
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

Element* RationalResult::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::RationalResultConfig config;
    config.FromJson(value, alloc);
    return new RationalResult(parent, config);
}

void RationalResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    solving_id = logical_id;
    document->Solve(logical_id, guid, GetCodeId(), config, !GetParent(1)->visible, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void RationalResult::PutResult(Result& result)
{
    ResultRow::PutResult(result);

    solving_id.clear();

    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;
    if (result.values.size() == 1)
        cast_units = result.values[0].cast_units;
    else
        cast_units.clear();

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);
        if (result.values.empty())
            return;
        
        Value& value = result.values[0];
        std::string integer = value.value["integer"];
        std::string numerator = value.value["numerator"];
        std::string denomerator = value.value["denomerator"];

        elements->Clear();
        if (numerator[0] == '-')
        {
            AddElement(ElementPtr(new Minus(this)));
            numerator = numerator.substr(1, numerator.size() - 1);
        }

        if (!integer.empty())
        {
            AddElement(ElementPtr(new CodeString(this, integer)));
        }

        if (numerator != "0")
        {
            if (denomerator == "1")
            {
                AddElement(ElementPtr(new CodeString(this, numerator)));
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
            AddNumber(numerator);
        }

        PutUnit(result.values[0]);
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
    ResultRow(_document),
    config(_document->config.complex_result)
{
    type = ElementType::COMPLEX_RESULT;
}

ComplexResult::ComplexResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::COMPLEX_RESULT;

    if (parent)
        config = parent->document->config.complex_result;
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
    value.AddMember("with_angle_measure", with_angle_measure, alloc);
}

Element* ComplexResult::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::ComplexResultConfig config;
    config.FromJson(value, alloc);
    auto* p = new ComplexResult(parent, config);
    if (value.HasMember("with_angle_measure") && value["with_angle_measure"].IsBool())
        p->with_angle_measure = value["with_angle_measure"].GetBool();
    return p;
}

void ComplexResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    solving_id = logical_id;
    document->Solve(logical_id, guid, GetCodeId(), config, !GetParent(1)->visible, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void ComplexResult::PutResult(Result& result)
{
    ResultRow::PutResult(result);

    solving_id.clear();

    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);

        if (config.form == ComplexForm::Exponential || config.form == ComplexForm::Trigonometric)
        {
            for (Value& value : result.values)
            {
                std::string mantissa = value.value["module_mantissa"];
                std::string exponent = value.value["module_exponent"];

                if (!mantissa.empty())
                    AddNumber(mantissa);
                AddExponent(exponent);

                mantissa = value.value["argument_mantissa"];
                exponent = value.value["argument_exponent"];

                if (config.form == ComplexForm::Exponential)
                {
                    PowerPtr p(new Power(this));
                    p->AddBase(CodeStringPtr(new CodeString(p.get(), "e")));

                    if (!mantissa.empty())
                        p->AddExponent(ElementPtr(new CodeString(this, mantissa)));

                    AddExponent(p->elements->Get(2)->elements->Get(0).get(), exponent);

                    p->AddExponent(ElementPtr(new CodeString(this, document->config.language == yutovo_calculator::Language::Russian ? "j" : "i")));

                    AddElement(p);
                }
                else
                {
                    AddElement(ElementPtr(new OpenBracket(this, ElementType::OPEN_ROUND_BRACKET)));

                    AddElement(ElementPtr(new CodeString(this, "cos")));
                    AddElement(ElementPtr(new OpenBracket(this, ElementType::OPEN_ROUND_BRACKET)));
                    AddNumber(mantissa);
                    AddExponent(exponent);
                    AddElement(ElementPtr(new CloseBracket(this, ElementType::CLOSE_ROUND_BRACKET)));

                    AddElement(ElementPtr(new Plus(this)));

                    AddElement(ElementPtr(new CodeString(this, document->config.language == yutovo_calculator::Language::Russian ? "j" : "i")));

                    AddElement(ElementPtr(new Multiply(this)));

                    AddElement(ElementPtr(new CodeString(this, "sin")));
                    AddElement(ElementPtr(new OpenBracket(this, ElementType::OPEN_ROUND_BRACKET)));
                    AddNumber(mantissa);
                    AddExponent(exponent);
                    AddElement(ElementPtr(new CloseBracket(this, ElementType::CLOSE_ROUND_BRACKET)));

                    AddElement(ElementPtr(new CloseBracket(this, ElementType::CLOSE_ROUND_BRACKET)));
                }

                AddResult();
            }
        }
        else
        {
            for (Value& value : result.values)
            {
                std::string re_mantissa = value.value["re_mantissa"];
                std::string re_exponent = value.value["re_exponent"];

                if (!re_mantissa.empty())
                    AddNumber(re_mantissa);
                AddExponent(re_exponent);

                std::string im_mantissa = value.value["im_mantissa"];
                std::string im_exponent = value.value["im_exponent"];

                if (!im_mantissa.empty())
                {
                    if (im_mantissa[0] == '-')
                    {
                        AddElement(ElementPtr(new Minus(this)));
                        im_mantissa = im_mantissa.substr(1, im_mantissa.size() - 1);
                    }
                    else if (!re_mantissa.empty())
                        AddElement(ElementPtr(new Plus(this)));
                    AddNumber(im_mantissa);
                }
                if (!im_exponent.empty())
                    AddExponent(im_exponent);

                if (!im_mantissa.empty() || !im_exponent.empty())
                    AddElement(ElementPtr(new CodeString(this, document->config.language == yutovo_calculator::Language::Russian ? "j" : "i")));

                if (config.show_angle_measure)
                {
                    std::string angle_measure = AngleMeasureToString(value.angle_measure);
                    with_angle_measure = !angle_measure.empty();
                    if (!angle_measure.empty())
                        AddElement(ElementPtr(new CodeString(this, "(" + angle_measure + ")", GetStringFormat())));
                }

                AddResult();
            }
        }
    }

    for (int i = 0; i < elements->Count(); ++i)
        elements->Get(i)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool ComplexResult::SetConfig(const int precision, const int exp, const AngleMeasure default_angle_measure, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.precision != precision)
        config.precision = precision;
    if (exp != -1 && config.exp != exp)
        config.exp = exp;
    if (default_angle_measure != AngleMeasure::None && config.default_angle_measure != default_angle_measure)
        config.default_angle_measure = default_angle_measure;
    if (result_angle_measure != AngleMeasure::None && config.result_angle_measure != result_angle_measure)
        config.result_angle_measure = result_angle_measure;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool ComplexResult::SetConfig(const int precision, const int exp, const AngleMeasure default_angle_measure, const AngleMeasure result_angle_measure, 
    ComplexForm form, uint max_count)
{
    if (precision != -1 && config.precision != precision)
        config.precision = precision;
    if (exp != -1 && config.exp != exp)
        config.exp = exp;
    if (default_angle_measure != AngleMeasure::None && config.default_angle_measure != default_angle_measure)
        config.default_angle_measure = default_angle_measure;
    if (result_angle_measure != AngleMeasure::None && config.result_angle_measure != result_angle_measure)
        config.result_angle_measure = result_angle_measure;
    config.form = form;
    config.max_count = max_count;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool ComplexResult::SetConfig(ComplexForm form)
{
    config.form = form;
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

//ArrayRealResult

ArrayRealResult::ArrayRealResult(Document* _document) :
    ResultRow(_document),
    config(_document->config.array_real_result)
{
    type = ElementType::ARRAY_REAL_RESULT;
}

ArrayRealResult::ArrayRealResult(Element* parent) :
    ResultRow(parent)
{
    type = ElementType::ARRAY_REAL_RESULT;

    if (parent)
        config = parent->document->config.array_real_result;
}

ArrayRealResult::ArrayRealResult(Element* parent, Config::ArrayRealResultConfig _config) :
    ResultRow(parent), 
    config(_config)
{
    type = ElementType::ARRAY_REAL_RESULT;
}

Element* ArrayRealResult::Clone()
{
    return new ArrayRealResult(*this);
}

Element* ArrayRealResult::Create(Element* _parent)
{
    return new RealResult(_parent);
}

void ArrayRealResult::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    ResultRow::ToJson(value, alloc);
    config.ToJson(value, alloc);
    value.AddMember("with_angle_measure", with_angle_measure, alloc);
}

Element* ArrayRealResult::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::ArrayRealResultConfig config;
    config.FromJson(value, alloc);
    auto* p = new ArrayRealResult(parent, config);
    if (value.HasMember("with_angle_measure") && value["with_angle_measure"].IsBool())
        p->with_angle_measure = value["with_angle_measure"].GetBool();
    return p;
}

void ArrayRealResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    solving_id = logical_id;
    document->Solve(logical_id, guid, GetCodeId(), config, !GetParent(1)->visible, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void ArrayRealResult::PutResult(Result& result)
{
    ResultRow::PutResult(result);

    solving_id.clear();

    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
    {
        PutError(result.error); //put error message
    }
    else
    {
        document->RemoveErrorMarks(parent->parent->id);

        if (result.values.empty())
        {
            AddOpenSquareBracket();
            AddCloseSquareBracket();
        }
        else
        {
            auto add_value = 
                [this](Value& value)
                {
                    std::string mantissa = value.value["mantissa"];
                    std::string exponent = value.value["exponent"];
                    AddNumber(mantissa);
                    AddExponent(exponent);
                    PutUnit(value);
                };

            if (result.values.size() == 1)
            {
                add_value(result.values[0]); //without square brackets
            }
            else
            {
                AddOpenSquareBracket();
                for (size_t i = 0; i < result.values.size(); ++i)
                {
                    add_value(result.values[i]);
                    if (i < result.values.size() - 1)
                        AddComma();
                }
                AddCloseSquareBracket();
            }
        }
    }

    if (elements->Count() > 0)
        elements->Get(0)->SetEditable(false);
    Remake(true);
    parent->Remake(true);
}

bool ArrayRealResult::SetConfig(const int precision, const int exp, const AngleMeasure default_angle_measure, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.precision != precision)
        config.precision = precision;
    if (exp != -1 && config.exp != exp)
        config.exp = exp;
    if (default_angle_measure != AngleMeasure::None && config.default_angle_measure != default_angle_measure)
        config.default_angle_measure = default_angle_measure;
    if (result_angle_measure != AngleMeasure::None && config.result_angle_measure != result_angle_measure)
        config.result_angle_measure = result_angle_measure;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool ArrayRealResult::SetConfig(const yutovo_calculator::Unit& unit)
{
    if (config.unit == unit)
        return false;
    
    config.unit = unit;
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

    ElementPtr row = GetCurRow();
    if (error.parser_error_code != yutovo_calculator::ParserExceptionCode::None)
        row->elements->Insert(ElementPtr(new CodeString(this, ErrorCodeToString(error.parser_error_code), true)), 0);
    else
        row->elements->Insert(ElementPtr(new CodeString(this, ErrorCodeToString(error.error_code), true)), 0);
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
    ResultRow::ToJson(value, alloc);
    config.ToJson(value, alloc);
}

Element* AutoResult::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Config::AutoResultConfig config;
    config.FromJson(value, alloc);
    return new AutoResult(parent, config);
}

void AutoResult::Solve(const ParserString& expression)
{
    if (last_expression == expression && last_expression.Text() != U"")
        return;
    last_expression = expression;

    PutWaitingSymbol();

    solving_id = logical_id;
    document->Solve(logical_id, guid, GetCodeId(), config, !GetParent(1)->visible, last_expression.Text(), 
        (delay && last_error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ? document->config.solve_delay : 0);
    delay = true;
}

void AutoResult::PutResult(Result& result)
{
    ResultRow::PutResult(result);

    solving_id.clear();

    ElementPtr el = document->FindParent(id, ElementType::EQUATION);
    Equation* eq = (Equation*)el.get();
    eq->dependencies = result.dependencies;

    last_error_code = result.error.error_code;
    if (result.error.error_code == yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR)
    {
        eq->last_expression.Reset();
        return;
    }

    elements->Clear();
    if (result.error.error_code != yutovo_solver::ErrorCode::OK)
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
            result_row.reset(new ComplexResult(this, config.complex_result));
            break;
    	case ResultType::ARRAY_REAL:
            result_row.reset(new ArrayRealResult(this));
            break;
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

void AutoResult::BeforeReplace()
{
    replacing = true;
}

void AutoResult::AfterReplace()
{
    replacing = false;
    if (FindParent(ElementType::EQUATION) != ElementId{})
        return;

    //move the child elements outside
    SetEditable(true);
    int pos = parent->elements->GetChildPos(id) + 1;
    for (int i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        for (int j = 0; j < el->elements->Count();)
        {
            auto ch = el->elements->Get(j);
            parent->elements->Move(ch, pos++);
        }
    }
    parent->elements->Remove(id);
}

void AutoResult::BeforePaste()
{
    //move child elements outside and remove this element
    int c = parent->elements->Count();
    for (int i = 0; i < elements->Count(); ++i)
        for (int j = 0; j < elements->Get(i)->elements->Count(); ++j)
        {
            auto _el = elements->Get(i)->elements->Get(j);
            if (_el)
                parent->elements->Move(_el, c + j);
        }
    parent->elements->RemoveAt(c - 1, 1);
}

bool AutoResult::SetConfig(const int precision, const int exp, const AngleMeasure default_angle_measure, const AngleMeasure result_angle_measure)
{
    if (precision != -1 && config.real_result.precision != precision)
        config.real_result.precision = precision;
    if (exp != -1 && config.real_result.exp != exp)
        config.real_result.exp = exp;
    if (default_angle_measure != AngleMeasure::None && config.real_result.default_angle_measure != default_angle_measure)
    {
        config.real_result.default_angle_measure = default_angle_measure;
        config.complex_result.default_angle_measure = default_angle_measure;
    }
    if (result_angle_measure != AngleMeasure::None && config.real_result.result_angle_measure != result_angle_measure)
    {
        config.real_result.result_angle_measure = result_angle_measure;
        config.complex_result.result_angle_measure = result_angle_measure;
    }
    
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

bool AutoResult::SetConfig(const int precision, const int exp, const AngleMeasure default_angle_measure, const AngleMeasure result_angle_measure, 
    ComplexForm form, uint max_count)
{
    if (precision != -1 && config.complex_result.precision != precision)
        config.complex_result.precision = precision;
    if (exp != -1 && config.complex_result.exp != exp)
        config.complex_result.exp = exp;
    if (default_angle_measure != AngleMeasure::None && config.complex_result.default_angle_measure != default_angle_measure)
        config.complex_result.default_angle_measure = default_angle_measure;
    if (result_angle_measure != AngleMeasure::None && config.complex_result.result_angle_measure != result_angle_measure)
        config.complex_result.result_angle_measure = result_angle_measure;
    config.complex_result.form = form;
    config.complex_result.max_count = max_count;
    
    ParserString expr = last_expression;
    last_expression.Reset();
    Solve(expr);
    return true;
}

bool AutoResult::SetConfig(ComplexForm form)
{
    if (config.complex_result.form == form)
        return false;

    config.complex_result.form = form;
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
    case ElementType::ARRAY_REAL_RESULT:
        return ResultType::ARRAY_REAL;
    case ElementType::ERROR_RESULT:
        return ResultType::AUTO;
    default:
        assert(false);
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

std::string AutoResult::ToHtml() const
{
    if (elements->Count() > 0)
        return elements->Get(0)->ToHtml();
    return "<mrow></mrow>";
}

}
