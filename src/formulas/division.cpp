#include "division.h"
#include "code_row.h"
#include "shape.h"

namespace yutovo
{

//Division

Division::Division(Element* _parent, bool with_init) : 
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::DIVISION;
}

Division::Division(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::DIVISION;
}

Division::Division(const Division& source) :
    MiddleShapeFormula(source)
{
}

Element* Division::Clone()
{
    return new Division(*this);
}

Element* Division::Create(Element* parent)
{
    return new Division(parent);
}

Element* Division::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Division(parent, false);
    return new Division(document, false);
}

void Division::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color;
            if (r.height == 0)
                window->DrawLine(r.left, r.top, r.left + r.width, r.top, c);
            else
                window->DrawFillRect(r.left, r.top, r.width, r.height, c);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    MiddleShapeFormula::Draw();
}

bool Division::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);
    
    int w = std::max(first->rect.width + 2, last->rect.width + 2);
    if (w < 200)
        shape->rect.SetRect(0, 0, w, 0);
    else
        shape->rect.SetRect(0, 0, w, w / 200 > 2 ? 2 : w / 200);
    first->rect.Move((w - first->rect.width) / 2, 0);
    shape->rect.Move(0, first->rect.height + shape->rect.height + 4);
    last->rect.Move((w - last->rect.width) / 2, first->rect.height + shape->rect.height + (shape->rect.height + 4) * 2);
    baseline = shape->rect.GetBottom() - shape->rect.height / 2;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Division::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        CaretState c;
        if ((last->GetFirstCaretState(c, nullptr) && caret_state == c) || (first->GetFirstCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(id);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetLeftCaretState(caret_state, select);
}

bool Division::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        CaretState c;
        if ((last->GetLastCaretState(c, nullptr) && caret_state == c) || (first->GetLastCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetRightCaretState(caret_state, select);
}

bool Division::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (last->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(last->id))
        return last->GetTopCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(shape->id))
        return shape->GetTopCaretState(x, y, caret_state, select);
    if (first->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(first->id))
        return first->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Division::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (first->GetAbsoluteRect().top >= y)
        return first->GetBottomCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().top >= y)
        return shape->GetBottomCaretState(x, y, caret_state, select);
    if (last->GetAbsoluteRect().top >= y)
        return last->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool Division::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(id))
    {
        caret_state.SetState(parent->id, parent->elements->GetChildPos(id));
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return true;
    }
    return MiddleShapeFormula::GetBeginCaretState(caret_state, select);
}

bool Division::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(id))
    {
        caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
        select->Add(parent->id, parent->elements->GetChildPos(id), 1);
        return true;
    }
    return MiddleShapeFormula::GetEndCaretState(caret_state, select);
}

std::string Division::ToHtml()
{
    std::string s = "<mfrac>";
    s += first->ToHtml();
    s += last->ToHtml();
    s += "</mfrac>";
    return s;
}

std::u32string Division::ToText()
{
    if (!first || !last)
        return U"";
    return U"(" + first->ToText() + U")/(" + last->ToText() + U")";
}

void Division::ToParserString(ParserString& str)
{
    str.Add(id, U"(");
    first->ToParserString(str);
    if (first->elements->Count() == 1 && document->IsString(first->elements->Get(0)->id) && 
        last->elements->Count() == 1 && document->IsString(last->elements->Get(0)->id) && 
        first->elements->Get(0)->ToText().find_first_not_of(U"0123456789.") == std::string::npos && 
        last->elements->Get(0)->ToText().find_first_not_of(U"0123456789.") == std::string::npos)
    {
        str.Add(id, U"/");
    }
    else
    {
        str.Add(id, U")/(");
    }
    last->ToParserString(str);
    str.Add(id, U")");
}

void Division::AddNumerator(ElementPtr numerator)
{
    if (first->IsEmpty())
        first->elements->Clear();
    first->elements->Add(numerator);
}

void Division::AddDenomerator(ElementPtr denomerator)
{
    if (last->IsEmpty())
        last->elements->Clear();
    last->elements->Add(denomerator);
}

}
