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
}

Equation::Equation(Element* _parent, yutovo_service::ResultType _result_type) :
    MiddleShapeFormula(_parent),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
}

Equation::Equation(Document* _document, yutovo_service::ResultType _result_type) :
    MiddleShapeFormula(_document),
    result_type(_result_type)
{
    type = ElementType::EQUATION;
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
    shape->rect.SetSize(s.width, s.height);
    shape->baseline = shape->rect.height / 2;

    MiddleShapeFormula::UpdateRect(false);
}

void Equation::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    if (with_elements)
        elements->Remake(with_parent, with_undo);

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

    if (rect != last_rect && with_parent)
        parent->Remake(false, true, with_undo);
    last_rect = rect;
}

bool Equation::DeleteElements(bool left, bool with_undo)
{
    if (caret->IsOnElement(shape->id))
    {
        last = nullptr;
        elements->RemoveAt(2, 1);
    }
    return MiddleShapeFormula::DeleteElements(left, with_undo);
}

bool Equation::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        first->elements->Clear();
    for (int i = 0; i < pos; ++i)
    {
        auto el = parent->elements->Get(0);
        if (with_undo)
        {
            document->InsertElement(el->Clone(), false, true);
            document->PushEditorState(CaretState(parent->id, 0), true);
        }
        first->elements->Move(el, i);
    }
    caret->SetState(shape->id);
    first->SubscribeOnChange(id);
    ReSolve();
    return true;
}

void Equation::ReSolve()
{
    auto_result.reset();
    OnChanged({});
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

void Equation::OnChanged(const ElementId _id)
{
    if (!auto_result)
    {
        auto_result.reset(new AutoResult(last));
        last->elements->Clear();
        last->elements->Add(auto_result);
    }
    auto_result->Solve(first->ToText(), result_type); //solve the expression in the left part
}

}
