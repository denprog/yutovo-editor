#include "only_shape_formula.h"
#include "../caret.h"

namespace yutovo
{

//OnlyShapeFormula

OnlyShapeFormula::OnlyShapeFormula(Element* _parent, char _symbol) :
    Formula(_parent),
    symbol(_symbol)
{
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
}

OnlyShapeFormula::OnlyShapeFormula(Document* _document, char _symbol) :
    Formula(_document),
    symbol(_symbol)
{
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
}

OnlyShapeFormula::OnlyShapeFormula(const OnlyShapeFormula& source) :
    Formula(source),
    shape((Shape*)elements->Get(0).get()),
    symbol(source.symbol)
{
}

bool OnlyShapeFormula::AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo)
{
    caret->SetState(parent->id, parent->elements->GetElementPos(id) + 1, true);
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
    shape->rect.SetRect(0, 0, s.width, s.height);
    baseline = shape->rect.height / 2;

    Formula::UpdateRect(false);
}

}
