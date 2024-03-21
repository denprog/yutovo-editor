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
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
                window->DrawText(draw_sign, f, r, document->config.formula_bg_color, document->config.bg_selection_color);
            else
                window->DrawText(draw_sign, f, r, document->config.shapes_color, document->config.formula_bg_color);
        };

    MiddleShapeFormula::Draw();
}

void Assignment::UpdateRect(bool with_elements)
{
    Size s = parent->window->GetTextSize(ToUtfString(draw_sign), GetStringFormat());
    GetShape()->rect.SetSize(s.width, s.height * 3 / 4);
    GetShape()->baseline = GetShape()->rect.height / 3 * 2;

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

    GetFirst()->rect.Move(0, baseline - GetFirst()->baseline);
    GetShape()->rect.Move(GetFirst()->rect.width, baseline - GetShape()->baseline);
    GetLast()->rect.Move(GetFirst()->rect.width + GetShape()->rect.width, baseline - GetLast()->baseline);

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
        GetFirst()->elements->Clear();
    for (int i = 0; i < pos; ++i)
    {
        auto el = parent->elements->Get(0);
        GetFirst()->elements->Move(el, i);
    }
    CaretState c;
    GetLast()->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    last_expression.Reset();
    GetLast()->SubscribeOnChange(id);
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
        GetLast()->UnsubscribeOnChange(id);
    }
}

void Assignment::Solve()
{
    if (!auto_solve)
        return;
    
    MiddleShapeFormula::Solve();

    ParserString str;
    GetFirst()->ToParserString(str);
    str.Add(id, solve_sign);
    GetLast()->ToParserString(str);
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
    GetFirst()->ToParserString(expr);
    expr.Add(id, solve_sign);
    GetLast()->ToParserString(expr);
    if (last_expression != expr)
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (last_identifier != U"")
            document->RemoveIdentifier(id, ((CodeBlock*)code.get())->code_id, last_identifier, delay ? document->config.solve_delay : 0);
        document->SetIdentifier(id, ((CodeBlock*)code.get())->code_id, GetFirst()->ToText(), expr.Text(), delay ? document->config.solve_delay : 0);
        delay = true;
        last_identifier = GetFirst()->ToText();
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
    std::string s = GetFirst()->ToHtml();
    s += "<mo>" + ToBasicString(solve_sign) + "</mo>";
    if (GetLast())
        s += GetLast()->ToHtml();
    return s;
}

std::u32string Assignment::ToText()
{
    std::u32string s;
    if (elements->Count() > 0)
        s = elements->Get(0)->ToText();
    s += solve_sign;
    if (elements->Count() == 3)
        s += elements->Get(2)->ToText();
    return s;
}

void Assignment::ToParserString(ParserString& str)
{
    if (elements->Count() > 0)
        elements->Get(0)->ToParserString(str);
    str.Add(id, solve_sign);
    if (elements->Count() == 3)
        elements->Get(2)->ToParserString(str);
}

}
