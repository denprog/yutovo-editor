#include "subscript.h"
#include "code_row.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

Subscript::Subscript(Element* _parent, bool with_init) :
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::SUBSCRIPT;
    UpdateLevel(level);
}

Subscript::Subscript(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::SUBSCRIPT;
    UpdateLevel(level);
}

Subscript::Subscript(const Subscript& source) :
    MiddleShapeFormula(source)
{
}

Element* Subscript::Clone()
{
    return new Subscript(*this);
}

Element* Subscript::Create(Element* _parent)
{
    return new Subscript(_parent);
}

Element* Subscript::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new Subscript(parent, false);
    return new Subscript(document, false);
}

void Subscript::Draw() const
{
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool Subscript::Remake(bool with_elements)
{
    UpdateLevel(level);

    bool changed = MiddleShapeFormula::Remake(with_elements);

    first->rect.Move(0, 0);
    shape->rect.SetRect(0, 0, 1, last->rect.height + first->rect.height / 2);
    shape->rect.Move(first->rect.width, 0);
    last->rect.Move(first->rect.width + shape->rect.width, first->rect.height / 2);

    baseline = first->rect.top + first->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Subscript::AfterChildInsert(const ElementId child_id, bool with_undo)
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
    }
}

void Subscript::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (last)
        last->UpdateLevel(_level + 1);
}

std::string Subscript::ToHtml()
{
    if (!first || !last)
        return "";
    std::string s = "<msub>";
    s += first->ToHtml();
    s += last->ToHtml();
    s += "</msub>";
    return s;
}

std::u32string Subscript::ToText()
{
    if (!first || !last)
        return U"";
    auto _first = first->ToText();
    auto _last = last->ToText();
    if (_last == U"bin" || _last == U"oct" || _last == U"dec" || _last == U"hex")
        return _last + U"[" + _first + U"]";
    if (_first == U"log") //TODO: get names of functions from the parser
        return _first + U"%" + _last + U",";
    return _first + U"{" + _last + U"}";
}

void Subscript::ToParserString(ParserString& str)
{
    if (!first || !last)
        return;
    if (first->ToText() == U"log")
    {
        first->ToParserString(str);
        str.Add(id, U"%");
        last->ToParserString(str);
        str.Add(id, U",");
    }
    else
    {
        auto _last = last->ToText();
        if (_last == U"bin" || _last == U"oct" || _last == U"dec" || _last == U"hex")
        {
            last->ToParserString(str);
            str.Add(id, U"[");
            first->ToParserString(str);
            str.Add(id, U"]");
        }
        else
        {
            first->ToParserString(str);
            str.Add(id, U"{");
            last->ToParserString(str);
            str.Add(id, U"}");
        }
    }
}

}
