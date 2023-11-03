#include "equation.h"
#include "result.h"
#include "../document.h"

namespace yutovo
{

//Equation

Equation::Equation(Element* _parent) : 
    MiddleShapeFormula(_parent)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(Element* _parent, yutovo_service::ResultType _result_type, bool with_init) :
    MiddleShapeFormula(_parent, with_init),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(Document* _document, yutovo_service::ResultType _result_type, bool with_init) :
    MiddleShapeFormula(_document, with_init),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(const Equation& source) :
    MiddleShapeFormula(source),
    result_type(source.result_type)
{
    if (last && last->elements->Count() > 0)
    {
        auto* r = last->elements->Get(0).get();
        if (dynamic_cast<ResultRow*>(r))
        {
            result.reset((ResultRow*)r->Clone());
            last->elements->Replace(result, 0);
        }
    }
}

Element* Equation::Clone()
{
    return new Equation(*this);
}

Element* Equation::Create(Element* _parent)
{
    return new Equation(_parent);
}

void Equation::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    MiddleShapeFormula::ToJson(value, alloc);
    value.AddMember("result_type", (int)result_type, alloc);
}

Element* Equation::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("result_type") || !value["result_type"].IsInt())
        return nullptr;
    yutovo_service::ResultType result_type = (yutovo_service::ResultType)value["result_type"].GetInt();

    if (parent)
        return new Equation(parent, result_type, false);
    return new Equation(document, result_type, false);
}

bool Equation::AfterFromJson()
{
    first = (CodeRow*)elements->Get(0).get();
    shape = (Shape*)elements->Get(1).get();
    last = (CodeRow*)elements->Get(2).get();
    auto* r = last->elements->Get(0).get();
    result.reset((ResultRow*)r->Clone());
    last->elements->Replace(result, 0);
    return true;
}

void Equation::Draw() const
{
    const auto f = GetStringFormat();
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
                window->DrawText("=", f, r, formula_format->bg_color, formula_format->bg_selection_color);
            else
                window->DrawText("=", f, r, formula_format->color, formula_format->bg_color);
        };

    MiddleShapeFormula::Draw();
}

void Equation::UpdateRect(bool with_elements)
{
    Size s = parent->window->GetTextSize(std::u32string(1, '='), GetStringFormat());
    shape->rect.SetSize(s.width, s.height * 3 / 4);
    shape->baseline = shape->rect.height / 3 * 2;

    MiddleShapeFormula::UpdateRect(false);
}

bool Equation::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);
    if (!with_elements)
        last->Remake(true);

    UpdateRect();

    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    first->rect.Move(0, baseline - first->baseline);
    shape->rect.Move(first->rect.width, baseline - shape->baseline);
    last->rect.Move(first->rect.width + shape->rect.width, baseline - last->baseline);

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Equation::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->IsOnElement(shape->id) && !left)
    {
        if (with_undo)
            document->StoreUndo(parent->id);
        last = nullptr;
        elements->RemoveAt(2, 1);
    }
    return MiddleShapeFormula::DeleteElements(left, with_undo, changed_element);
}

void Equation::BeforeReplace()
{
    MiddleShapeFormula::BeforeReplace();
    document->RemoveErrorMarks(id);
}

void Equation::BeforeDelete()
{
    MiddleShapeFormula::BeforeDelete();
    document->RemoveErrorMarks(id);
}

bool Equation::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        first->elements->Clear();
    ready = false;
    for (int i = 0; i < pos; ++i)
    {
        auto el = parent->elements->Get(0);
        first->elements->Move(el, i);
    }
    ready = true;
    caret->SetState(shape->id);
    first->SubscribeOnChange(id);
    return true;
}

void Equation::Solve()
{
    MiddleShapeFormula::Solve();

    ParserString str;
    first->ToParserString(str);
    if (last_expression != str)
        document->AddResolveElement(id);
}

void Equation::ReSolve(bool if_error)
{
    if (if_error && result && result->last_error_code == yutovo_service::ErrorCode::OK)
        return;
    
    ParserString str;
    first->ToParserString(str);
    if (last_expression == str)
        return;
    
    last_expression = str;
    document->RemoveErrorMarks(id);
    UpdateResult(str);
}

bool Equation::Depends(const std::string& identifier)
{
    if (std::find(dependencies.begin(), dependencies.end(), identifier) != dependencies.end())
        return true;
    return false;
}

void Equation::SetResult(Config::AutoResultConfig config)
{
    result.reset(new AutoResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::RealResultConfig config)
{
    result.reset(new RealResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::IntegerResultConfig config)
{
    result.reset(new IntegerResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::RationalResultConfig config)
{
    result.reset(new RationalResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::ComplexResultConfig config)
{
}

bool Equation::SetResult(ResultType _result_type, bool with_undo)
{
    if (result_type == _result_type)
        return false;

    caret->SetState(id, 1, true);

    if (with_undo)
        document->StoreUndo(id);
    
    result_type = _result_type;
    result.reset();
    document->RemoveErrorMarks(id);

    ParserString str;
    first->ToParserString(str);
    UpdateResult(str);
    return true;
}

bool Equation::SetConfig(int precision, int exp, AngleMeasure result_angle_measure, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(precision, exp, result_angle_measure);
    }
    case ElementType::REAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RealResult* r = (RealResult*)result.get();
        return r->SetConfig(precision, exp, result_angle_measure);
    }
    case ElementType::COMPLEX_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        ComplexResult* r = (ComplexResult*)result.get();
        return r->SetConfig(precision, exp, result_angle_measure);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(Notation default_notation, Notation result_notation, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::INTEGER_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        IntegerResult* r = (IntegerResult*)result.get();
        return r->SetConfig(default_notation, result_notation);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(default_notation, result_notation);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(FractionForm fraction_form, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::RATIONAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RationalResult* r = (RationalResult*)result.get();
        return r->SetConfig(fraction_form);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(fraction_form);
    }
    default:
        return false;
    }
}

bool Equation::SetConfig(const yutovo_calculator::Unit& unit, bool with_undo)
{
    switch (result->type)
    {
    case ElementType::RATIONAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RationalResult* r = (RationalResult*)result.get();
        return r->SetConfig(unit);
    }
    case ElementType::REAL_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        RealResult* r = (RealResult*)result.get();
        return r->SetConfig(unit);
    }
    case ElementType::AUTO_RESULT:
    {
        caret->SetState(id, 1, true);
        if (with_undo)
            document->StoreUndo(id);
        AutoResult* r = (AutoResult*)result.get();
        return r->SetConfig(unit);
    }
    default:
        return false;
    }
}

std::string Equation::ToHtml()
{
    std::string s = first->ToHtml();
    s += "<mo>=</mo>";
    if (last)
        s += last->ToHtml();
    return s;
}

std::u32string Equation::ToText()
{
    if (!first || !last)
        return U"";
    std::u32string s = first->ToText();
    s += U"=";
    s += last->ToText();
    return s;
}

void Equation::UpdateResult(ParserString& str)
{
    if (!ready)
        return;
    if (last)
    {
        if (!result)
        {
            switch (result_type)
            {
        	case ResultType::REAL:
                result.reset(new RealResult(last));
                break;
        	case ResultType::INTEGER:
                result.reset(new IntegerResult(last));
                break;
        	case ResultType::RATIONAL:
                result.reset(new RationalResult(last));
                break;
            case ResultType::AUTO:
                result.reset(new AutoResult(last));
                break;
            default:
                return;
            }
            last->elements->Clear();
            last->elements->Add(result);
        }
        else
        {
            result->Reset();
        }

        result->Solve(str); //solve the expression in the left part
        document->AddChangedElement(id);
    }
}

}
