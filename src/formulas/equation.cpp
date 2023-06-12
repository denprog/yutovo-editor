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

Equation::Equation(Element* _parent, yutovo_service::ResultType _result_type) :
    MiddleShapeFormula(_parent),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(Document* _document, yutovo_service::ResultType _result_type) :
    MiddleShapeFormula(_document),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
    remake_always = true;
}

Equation::Equation(const Equation& source) :
    MiddleShapeFormula(source),
    result_type(source.result_type)
{
}

Element* Equation::Clone()
{
    return new Equation(*this);
}

Element* Equation::Create(Element* _parent)
{
    return new Equation(_parent);
}

void Equation::Draw() const
{
    const auto f = GetStringFormat();
    shape->draw_func = 
        [&](const Rect& r)
        {
            window->DrawText("=", f, r, document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

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
    document->AddResolveElement(id);
}

void Equation::ReSolve(bool if_error)
{
    if (if_error && result && !result->last_error)
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

void Equation::SetResult(Config::AutoResult config)
{
    result.reset(new AutoResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::RealResult config)
{
    result.reset(new RealResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::IntegerResult config)
{
    result.reset(new IntegerResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

void Equation::SetResult(Config::RationalResult config)
{
    result.reset(new RationalResult(last, config));
    last->elements->Clear();
    last->elements->Add(result);
}

bool Equation::SetResult(ResultType _result_type, bool with_undo)
{
    if (result_type == _result_type)
        return false;
    
    if (with_undo)
        document->StoreUndo(id);
    
    result_type = _result_type;
    caret->SetState(id, 1, true);
    result.reset();
    document->RemoveErrorMarks(id);

    ParserString str;
    first->ToParserString(str);
    UpdateResult(str);
    return true;
}

bool Equation::SetConfig(Notation notation, bool with_undo)
{
    if (result->type != ElementType::INTEGER_RESULT)
        return false;
    
    if (with_undo)
        document->StoreUndo(id);
    
    caret->SetState(id, 1, true);
    IntegerResult* r = (IntegerResult*)result.get();
    return r->SetConfig(notation);
}

bool Equation::SetConfig(FractionForm fraction_form, bool with_undo)
{
    if (result->type != ElementType::RATIONAL_RESULT)
        return false;
    
    if (with_undo)
        document->StoreUndo(id);
    
    caret->SetState(id, 1, true);
    RationalResult* r = (RationalResult*)result.get();
    return r->SetConfig(fraction_form);
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
    std::u32string s = first->ToText();
    s += U"=";
    if (last)
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
