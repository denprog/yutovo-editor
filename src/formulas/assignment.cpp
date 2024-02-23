#include "assignment.h"
#include "code_block.h"

namespace yutovo
{

//Assignment

Assignment::Assignment(Element* _parent, bool with_init, bool _auto_solve) :
    MiddleShapeFormula(_parent, with_init),
    auto_solve(_auto_solve)
{
    type = ElementType::ASSIGNMENT;
}

Assignment::Assignment(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::ASSIGNMENT;
}

Assignment::Assignment(const Assignment& source) :
    MiddleShapeFormula(source),
    auto_solve(source.auto_solve)
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

Element* Assignment::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Assignment(parent, false);
    return new Assignment(document, false);
}

void Assignment::Draw() const
{
    const auto f = GetStringFormat();
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
                window->DrawText(":=", f, r, formula_format->bg_color, formula_format->bg_selection_color);
            else
                window->DrawText(":=", f, r, formula_format->color, formula_format->bg_color);
        };

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
    first = (CodeRow*)elements->Get(0).get();
    shape = (Shape*)elements->Get(1).get();
    last = (CodeRow*)elements->Get(2).get();

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
    if (auto_solve && caret->GetPos() == 1 && last_identifier != U"")
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
    if (!id.empty() && auto_solve)
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
    if (!auto_solve)
        return;
    
    MiddleShapeFormula::Solve();

    ParserString str;
    first->ToParserString(str);
    str.Add(id, U"=");
    last->ToParserString(str);
    if (last_expression != str)
        document->AddResolveElement(id);
}

void Assignment::ReSolve(bool if_error)
{
    if (!auto_solve)
        return;
    
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
    std::u32string s;
    if (elements->Count() > 0)
        s = elements->Get(0)->ToText();
    s += U"=";
    if (elements->Count() == 3)
        s += elements->Get(2)->ToText();
    return s;
}

void Assignment::ToParserString(ParserString& str)
{
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, U"=");
    if (elements->Count() == 3)
        elements->Get(2)->ToParserString(str);
}

}
