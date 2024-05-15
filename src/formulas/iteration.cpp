#include "iteration.h"
#include "document.h"
#include "code_row.h"
#include "assignment.h"

namespace yutovo
{

//Iteration

const std::string Iteration::family_name = "DejaVu Serif";

Iteration::Iteration(Element* _parent, char32_t _symbol, bool with_init) : 
    Formula(_parent),
    symbol(_symbol)
{
    if (with_init)
        Init();
}

Iteration::Iteration(Document* _document, char32_t _symbol, bool with_init) : 
    Formula(_document),
    symbol(_symbol)
{
    if (with_init)
        Init();
}

Iteration::Iteration(const Iteration& source) :
    Formula(source),
    lower((Assignment*)elements->Get(0).get()),
    shape((Shape*)elements->Get(1).get()),
    upper((CodeRow*)elements->Get(2).get()),
    right((CodeRow*)elements->Get(3).get()),
    symbol(source.symbol),
    symbol_str(source.symbol_str)
{
}

void Iteration::Init()
{
    symbol_str = ToBasicString(std::u32string(1, symbol));
    lower = new Assignment(this, true, false);
    lower->editable = false;
    shape = new Shape(this);
    upper = new CodeRow(this);
    right = new CodeRow(this);
    elements->Add(ElementPtr(lower));
    elements->Add(ElementPtr(shape));
    elements->Add(ElementPtr(upper));
    elements->Add(ElementPtr(right));

    UpdateLevel(level);
}

bool Iteration::AfterFromJson()
{
    if (elements->Count() != 4)
        return false;
    symbol_str = ToBasicString(std::u32string(1, symbol));
    lower = (Assignment*)elements->Get(0).get();
    shape = (Shape*)elements->Get(1).get();
    upper = (CodeRow*)elements->Get(2).get();
    right = (CodeRow*)elements->Get(3).get();
    return true;
}

void Iteration::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText(symbol_str, format, r, document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color, 
                    document->selection.IsSelected(id) ? document->config.bg_selection_color : document->config.formula_bg_color);
            }
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool Iteration::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = Formula::Remake(with_elements);

    Size s;
    int size = window->GetSymbolSize(symbol, (int)lround(right->rect.height * 2), family_name, s, baseline);
    if (size != 0)
    {
        shape->rect.SetRect(0, 0, s.width, s.height);
        format = document->string_formats->GetFormat(family_name, size, false, false, false, false, Color::Black(), Color::White(), Color::Blue());
    }

    int max_width = std::max(upper->rect.width, std::max(shape->rect.width, lower->rect.width));

    if (upper->rect.width < max_width)
        upper->rect.Move((max_width - upper->rect.width) / 2, 0);
    
    if (shape->rect.width < max_width)
        shape->rect.Move((max_width - shape->rect.width) / 2, upper->rect.height + 3);
    else
        shape->rect.Move(0, upper->rect.height + 3);

    if (lower->rect.width < max_width)
        lower->rect.Move((max_width - lower->rect.width) / 2, upper->rect.height + 3 + shape->rect.height + 3);
    else
        lower->rect.Move(0, upper->rect.height + 3 + shape->rect.height + 3);

    baseline = upper->rect.height + 2 + s.height / 2;

    right->rect.Move(max_width + 2, baseline - right->baseline);

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool Iteration::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (lower->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(lower->id))
        return lower->GetTopCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(shape->id))
        return shape->GetTopCaretState(x, y, caret_state, select);
    if (upper->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(upper->id))
        return upper->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Iteration::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (upper->GetAbsoluteRect().top >= y)
        return upper->GetBottomCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().top >= y)
        return shape->GetBottomCaretState(x, y, caret_state, select);
    if (lower->GetAbsoluteRect().top >= y)
        return lower->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

bool Iteration::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (caret->GetPos() == 0) //don't delete the counter
        return false;
    
    if (with_undo)
        document->StoreUndo(parent->id);

    //remove this element by deleting its shape
    lower->UpdateLevel(level);
    int p = parent->elements->GetElementPos(id);
    uint c1 = 0;
    caret->SetState(id);
    if (right)
    {
        c1 = right->elements->Count();
        parent->elements->Move(*right->elements, p);
    }
    CaretState c;
    if (parent->elements->Get(p)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    auto t = parent->elements->Get(p + c1); //for not removing this element until this function ends
    parent->elements->Remove(id);
    parent->Normalize();
    changed_element = parent->id;
    return true;
}

void Iteration::UpdateLevel(uint8_t _level)
{
    Formula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (lower)
        lower->UpdateLevel(_level + 1);
    if (upper)
        upper->UpdateLevel(_level + 1);
}

bool Iteration::AfterInsert(bool with_undo)
{
    if (lower->elements->Count() != 3 || !caret)
        return false;
    CaretState c;
    if (lower->elements->Get(0)->GetFirstCaretState(c, nullptr))
    {
        caret->SetState(c);
        return true;
    }
    return false;
}

std::string Iteration::ToHtml()
{
    std::string s = "<munderover>";
    s += "<mo>" + symbol_str + "</mo>";
    s += lower->ToHtml();
    s += upper->ToHtml();
    s += "</munderover>";
    s += right->ToHtml();
    return s;
}

}
