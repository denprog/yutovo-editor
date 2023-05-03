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

    parent->Remake(false, true, with_undo);
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
    for (int i = pos - 1; i >= open_pos; --i)
    {
        auto _el = parent->elements->Get(i);
        first->elements->Move(_el, 0);
        if (with_undo)
        {
            document->CallFunc(parent->id, 
                [d = document](const ElementId id)
                {
                    d->GetElement(id)->Normalize(true);
                },
                true);
            document->InsertElement(_el->Clone(), false, true);
            document->PushEditorState(CaretState(parent->id, parent->elements->GetElementPos(id)), true);
            document->DeleteElements(false, false, true);
            document->PushEditorState(CaretState(first->id, 0), true);
            document->CallFunc(parent->id, 
                [d = document](const ElementId id)
                {
                    d->GetElement(id)->Normalize(false);
                },
                true);
        }
    }
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

std::u32string Power::ToText()
{
    return U"pow(" + first->ToText() + U"," + last->ToText() + U")";
}

void Power::ToParserString(ParserString& str)
{
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
