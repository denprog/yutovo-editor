#include "assignment.h"
#include "code_block.h"

namespace yutovo
{

//Assignment

Assignment::Assignment(Element* _parent) :
    MiddleShapeFormula(_parent)
{
    type = ElementType::ASSIGNMENT;
}

Assignment::Assignment(Document* _document) :
    MiddleShapeFormula(_document)
{
    type = ElementType::ASSIGNMENT;
}

Assignment::Assignment(const Assignment& source) :
    MiddleShapeFormula(source)
{
}

Element* Assignment::Clone()
{
    return new Assignment(*this);
}

Element* Assignment::Create(Element* _parent)
{
    return new Assignment(_parent);
}

void Assignment::Draw() const
{
    const auto f = GetStringFormat();
    shape->draw_func = 
        [&](const Rect& r)
        {
            window->DrawText(":=", f, r, document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    MiddleShapeFormula::Draw();
}

void Assignment::UpdateRect(bool with_elements)
{
    Size s = parent->window->GetTextSize(std::u32string(U":="), GetStringFormat());
    shape->rect.SetSize(s.width, s.height * 3 / 4);
    shape->baseline = shape->rect.height / 3 * 2;

    MiddleShapeFormula::UpdateRect(false);
}

bool Assignment::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);

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

bool Assignment::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->GetPos() == 1 && last_identifier != U"")
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        document->RemoveIdentifier(id, ((CodeBlock*)code.get())->code_id, last_identifier, delay ? document->config.solve_delay : 0);
        delay = true;
    }

    return MiddleShapeFormula::DeleteElements(left, with_undo, changed_element);
}

bool Assignment::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        first->elements->Clear();
    for (int i = 0; i < pos; ++i)
    {
        auto el = parent->elements->Get(0);
        if (with_undo)
            document->StoreUndo(id);
        first->elements->Move(el, i);
    }
    CaretState c;
    last->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    last_expression.Reset();
    last->SubscribeOnChange(id);
    return true;
}

void Assignment::BeforeDelete()
{
    if (!id.empty())
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (code)
        {
            document->RemoveIdentifier(id, ((CodeBlock*)code.get())->code_id, last_identifier, delay ? document->config.solve_delay : 0);
            delay = true;
        }
        last->UnsubscribeOnChange(id);
    }
}

void Assignment::Solve()
{
    MiddleShapeFormula::Solve();
    document->AddResolveElement(id);
}

void Assignment::ReSolve(bool if_error)
{
    document->RemoveErrorMarks(id);
    if (if_error && !last_error)
        return;
    last_expression.Reset();

    ParserString expr;
    first->ToParserString(expr);
    expr.Add(id, U"=");
    last->ToParserString(expr);
    if (last_expression != expr)
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (last_identifier != U"")
            document->RemoveIdentifier(id, ((CodeBlock*)code.get())->code_id, last_identifier, delay ? document->config.solve_delay : 0);
        document->SetUserIdentifier(id, ((CodeBlock*)code.get())->code_id, first->ToText(), expr.Text(), delay ? document->config.solve_delay : 0);
        delay = true;
        last_identifier = first->ToText();
        last_expression = expr;
    }
}

void Assignment::PutResult(Result result)
{
    last_error = result.error.error_code != ErrorCode::OK;
    Remake(true);
}

std::string Assignment::ToHtml()
{
    std::string s = first->ToHtml();
    s += "<mo>=</mo>";
    if (last)
        s += last->ToHtml();
    return s;
}

std::u32string Assignment::ToText()
{
    std::u32string s = first->ToText();
    s += U"=";
    if (last)
        s += last->ToText();
    return s;
}

}
