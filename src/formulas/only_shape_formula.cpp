#include "only_shape_formula.h"
#include "../caret.h"
#include "../document.h"

namespace yutovo
{

//OnlyShapeFormula

OnlyShapeFormula::OnlyShapeFormula(Element* _parent, char32_t _symbol) :
    Formula(_parent),
    symbol(_symbol)
{
    formula_format = document->formula_formats->GetFormat("OnlyShapeFormula");
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
}

OnlyShapeFormula::OnlyShapeFormula(Document* _document, char32_t _symbol) :
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

void OnlyShapeFormula::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Formula::ToJson(value, alloc);
    rapidjson::Value _str(ToBasicString(std::u32string(1, symbol)).c_str(), alloc);
    value.AddMember("symbol", _str, alloc);
}

void OnlyShapeFormula::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (document->selection.IsSelected(id))
            {
                window->DrawText(ToBasicString(std::u32string(1, symbol)), GetStringFormat(), r, 
                    formula_format->bg_color, formula_format->bg_selection_color);
            }
            else
            {
                window->DrawText(ToBasicString(std::u32string(1, symbol)), GetStringFormat(), r, 
                    formula_format->color, formula_format->bg_color);
            }
        };

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
    Size s = window->GetTextSize(std::u32string(1, symbol), GetStringFormat());
    shape->rect.SetSize(s.width, s.height * 3 / 4);
    baseline = shape->rect.height / 3 * 2;

    Formula::UpdateRect(false);
}

std::u32string OnlyShapeFormula::ToText()
{
    return std::u32string(1, symbol);
}

void OnlyShapeFormula::ToParserString(ParserString& str)
{
    str.Add(id, ToText());
}

bool OnlyShapeFormula::SymbolFromJson(const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("symbol") || !value["symbol"].IsString())
        return false;
    auto _symbol = ToUtfString(value["symbol"].GetString());
    if (_symbol.length() != 1)
        return false;
    symbol = _symbol[0];
    return true;
}

}
