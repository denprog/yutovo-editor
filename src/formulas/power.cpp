#include "power.h"
#include "code_row.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

Power::Power(Element* _parent, bool with_init) :
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::POWER;
    UpdateLevel(level);
}

Power::Power(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
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

Element* Power::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Power(parent, false);
    return new Power(document, false);
}

void Power::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool Power::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = MiddleShapeFormula::Remake(with_elements);

    first->rect.Move(0, last->rect.height);
    shape->rect.SetRect(0, 0, 4, last->rect.height + first->rect.height);
    shape->rect.Move(first->rect.width, 0);
    last->rect.Move(first->rect.width + shape->rect.width, 0);

    baseline = first->rect.top + first->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Power::AfterChildInsert(const ElementId child_id, bool with_undo)
{
    //if a close fense was inserted, move elements from parent row into first child until open fence
    if (GetParent(child_id) != first->id)
        return;
    auto el = document->GetElement(child_id);
    if (el->type != ElementType::CLOSE_FENCE)
        return;
    int pos = parent->elements->GetElementPos(id);
    int open_pos = 0;
    for (int i = pos - 1; i >= 0; --i)
    {
        auto c = parent->elements->Get(i);
        if (c->type == ElementType::OPEN_FENCE)
        {
            open_pos = i;
            break;
        }
    }
    if (with_undo)
        document->StoreUndo(parent->id);
    for (int i = pos - 1; i >= open_pos; --i)
    {
        auto _el = parent->elements->Get(i);
        first->elements->Move(_el, 0);
    }
}

void Power::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (last)
        last->UpdateLevel(_level + 1);
}

std::string Power::ToHtml()
{
    if (!first || !last)
        return "";
    std::string s = "<msup>";
    s += first->ToHtml();
    s += last->ToHtml();
    s += "</msup>";
    return s;
}

std::u32string Power::ToText()
{
    if (!first || !last)
        return U"";
    return U"pow(" + first->ToText() + U"," + last->ToText() + U")";
}

void Power::ToParserString(ParserString& str)
{
    if (!first || !last)
        return;
    str.Add(id, U"pow(");
    first->ToParserString(str);
    str.Add(id, U",");
    last->ToParserString(str);
    str.Add(id, U")");
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
