#include "power.h"
#include "code_row.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

Power::Power(Element* _parent) :
    MiddleShapeFormula(_parent)
{
    type = ElementType::POWER;
    UpdateLevel(level);
}

Power::Power(Document* _document) :
    MiddleShapeFormula(_document)
{
    type = ElementType::POWER;
    UpdateLevel(level);
}

Power::Power(const Power& source) :
    MiddleShapeFormula(source)
{
}

Element* Power::Clone()
{
    return new Power(*this);
}

Element* Power::Create(Element* _parent)
{
    return new Power(_parent);
}

void Power::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    Formula::Draw();
}

void Power::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    if (with_elements)
        elements->Remake(with_parent, with_undo);

    first->rect.Move(0, last->rect.height);
    shape->rect.SetRect(0, 0, 4, last->rect.height + first->rect.height);
    shape->rect.Move(first->rect.width, 0);
    last->rect.Move(first->rect.width + shape->rect.width, 0);

    baseline = first->rect.top + first->baseline;

    UpdateRect();

    if (rect != last_rect && with_parent)
        parent->Remake(false, true, with_undo);
    last_rect = rect;

    document->Remake(parent->id, false, with_undo, false);
}

void Power::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    last->UpdateLevel(_level + 1);
}

std::string Power::ToHtml()
{
    std::string s = "<msup>";
    s += first->ToHtml();
    s += last->ToHtml();
    s += "</msup>";
    return s;
}

std::string Power::ToText()
{
    return "pow(" + first->ToText() + "," + last->ToText() + ")";
}

void Power::AddBase(ElementPtr base)
{
    if (first->IsEmpty())
        first->elements->Clear();
    first->elements->Add(base);
}

void Power::AddExponent(ElementPtr exponent)
{
    if (last->IsEmpty())
        last->elements->Clear();
    last->elements->Add(exponent);
    UpdateLevel(level);
}

}
