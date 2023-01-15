#include "result.h"
#include "multiply.h"
#include "plus.h"
#include "minus.h"
#include "power.h"
#include "division.h"
#include "../solver.h"
#include "code_string.h"

namespace yutovo
{

//ResultRow

ResultRow::ResultRow(Document* _document) : 
    CodeRow(_document)
{
}

ResultRow::ResultRow(Element* parent) :
    CodeRow(parent)
{
}

//RealResult

RealResult::RealResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::REAL_RESULT;
}

RealResult::RealResult(Element* parent, const std::string& mantissa, const std::string& exponent) :
    ResultRow(parent)
{
    type = ElementType::REAL_RESULT;
    elements->Clear();

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

Element* RealResult::Clone()
{
    return new RealResult(*this);
}

Element* RealResult::Create(Element* _parent)
{
    return new RealResult(_parent, "", "");
}

//IntegerResult

IntegerResult::IntegerResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::INTEGER_RESULT;
}

IntegerResult::IntegerResult(Element* parent, const std::string& value) :
    ResultRow(parent)
{
    type = ElementType::INTEGER_RESULT;
    elements->Clear();
    if (value[0] == '-')
    {
        AddElement(ElementPtr(new Minus(this)));
        AddElement(CodeStringPtr(new CodeString(this, value.substr(1, value.size() - 1))));
    }
    else
        AddElement(ElementPtr(new CodeString(this, value)));
}

//RationalResult

RationalResult::RationalResult(Document* _document) :
    ResultRow(_document)
{
    type = ElementType::RATIONAL_RESULT;
}

RationalResult::RationalResult(Element* parent, const std::string& numerator, const std::string& denomerator) :
    ResultRow(parent)
{
    type = ElementType::RATIONAL_RESULT;
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

ErrorResult::ErrorResult(Element* parent, const ErrorCode error_code) :
    ResultRow(parent)
{
    type = ElementType::ERROR_RESULT;
    elements->Clear();
    AddElement(ElementPtr(new CodeString(this, "Error")));
}

//AutoResult

AutoResult::AutoResult(Document* _document) :
    Element(_document)
{
    type = ElementType::AUTO_RESULT;
}

AutoResult::AutoResult(Element* parent) :
    Element(parent)
{
    type = ElementType::AUTO_RESULT;
}

AutoResult::AutoResult(const AutoResult& source) :
    Element(source),
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

void AutoResult::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    Element::Remake(with_elements, with_parent, with_undo);
    if (elements->Count() > 0)
        baseline = elements->Get(0)->baseline;
}

void AutoResult::Solve(const std::string& expression, ResultType result_type)
{
    if (last_expression == expression)
        return;

    document->Solve(id, ExpressionType::CALC, result_type, precision, angle_measure, notation, expression);

    last_expression = expression;
}

void AutoResult::PutResult(Result result)
{
    elements->Clear();
    if (result.error.error_code != ErrorCode::NONE)
    {
        //put error message
        elements->Add(ElementPtr(new ErrorResult(this, result.error.error_code)));
        document->Remake(parent->parent->id, true, false, false);
        return;
    }

    //put element of returned result type
    switch (result.type)
    {
    case ResultType::REAL:
        elements->Add(ResultPtr(new RealResult(this, result.values["mantissa"], result.values["exponent"])));
        break;
	case ResultType::INTEGER:
        elements->Add(ResultPtr(new IntegerResult(this, result.values["value"])));
        break;
	case ResultType::RATIONAL:
        elements->Add(ResultPtr(new RationalResult(this, result.values["numerator"], result.values["denomerator"])));
        break;
	case ResultType::COMPLEX:
        break;
    }

    Remake(true, false, false);
    document->Remake(parent->parent->id, true, false, false);
}

}
