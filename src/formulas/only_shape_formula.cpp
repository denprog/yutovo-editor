#include "only_shape_formula.h"
#include "../caret.h"
#include "../document.h"

namespace yutovo
{

//OnlyShapeFormula

OnlyShapeFormula::OnlyShapeFormula(Element* _parent, char _symbol) :
    Formula(_parent),
    symbol(_symbol)
{
    formula_format = document->formula_formats->GetFormat("OnlyShapeFormula");
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
}

OnlyShapeFormula::OnlyShapeFormula(Document* _document, char _symbol) :
    Formula(_document),
    symbol(_symbol)
{
    formula_format = document->formula_formats->GetFormat("OnlyShapeFormula");
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
}

OnlyShapeFormula::OnlyShapeFormula(const OnlyShapeFormula& source) :
    Formula(source),
    shape((Shape*)elements->Get(0).get()),
    symbol(source.symbol)
{
}

void OnlyShapeFormula::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    Formula::Draw();
}

bool OnlyShapeFormula::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (parent->elements->IsLast(id))
        caret->SetState(parent->id, parent->elements->GetElementPos(id) + 1, true);
    else if (parent->elements->Get(pos + 1)->HasCaretState())
        caret->SetState(parent->elements->Get(pos + 1)->id, true);
    else
    {
        CaretState c;
        if (parent->elements->Get(pos + 1)->GetFirstCaretState(c, nullptr))
            caret->SetState(c, true);
        else
            caret->SetState(parent->id, pos + 1, true);
    }
    parent->AfterChildInsert(id, with_undo);
    return true;
}

bool OnlyShapeFormula::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    return false;
}

bool OnlyShapeFormula::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    return false;
}

void OnlyShapeFormula::UpdateRect(bool with_elements)
{
    Size s = parent->window->GetTextSize(std::string(1, symbol), GetStringFormat());
    shape->rect.SetSize(s.width, s.height);
    baseline = shape->rect.height / 2;

    Formula::UpdateRect(false);
}

std::string OnlyShapeFormula::ToText()
{
    return std::string(1, symbol);
}

}
