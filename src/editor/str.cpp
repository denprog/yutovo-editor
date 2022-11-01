#include "str.h"
#include "document.h"
#include <assert.h>

namespace yutovo
{

//String

String::String(Element* parent) : 
    Element(parent),
    format(document->GetDefaultStringFormat())
{
    type = ElementType::STRING;

    baseline = parent->window->GetFontAscent(format);

    elements.reset(new StringElements(this));
}

String::String(Element* parent, const std::string _str) : 
    Element(parent)
{
    type = ElementType::STRING;

    elements.reset(new StringElements(this, _str));
}

String::String(Element* parent, const std::string _str, const StringFormatPtr _format) :
    Element(parent), 
    format(_format)
{
    type = ElementType::STRING;

    elements.reset(new StringElements(this, _str));
}

Element* String::Clone()
{
    return new String(*this);
}

void String::Remake(CaretState& caret_state, bool with_elements)
{
    elements->Remake(caret_state);
    UpdateRect();
}

void String::UpdateRect()
{
    Size s = parent->window->GetTextSize(((StringElements*)elements.get())->str, format);
    rect.SetRect(0, 0, s.width, s.height);
    baseline = parent->window->GetFontAscent(format);
}

bool String::InsertElements(std::vector<ElementPtr>& _elements, const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    if (_elements.size() == 1 && _elements[0]->type == ElementType::STRING)
    {
        String* s = dynamic_cast<String*>(_elements[0].get());
        if (elements->Count() == 0)
        {
            //replace the string and format
            if (before_state.GetPos() != 0)
                return false;
            elements.reset(new StringElements(this, s->elements->ToText()));
            format = s->format;
            after_state.SetState(elements->GetElementId(elements->Count()), before_state.selections);
            if (with_undo)
                document->DeleteElements(CaretState(elements->GetElementId(0), elements->Count()), false, true);
            //elements->Remake(caret_state);
            document->Remake(parent->id, false);
            return true;
        }
        else if (!s->format || s->format == format)
        {
            elements->Insert(_elements[0], before_state.GetPos());
            after_state.SetState(elements->GetElementId(before_state.GetPos() + _elements[0]->elements->Count()), before_state.selections);
            if (with_undo)
                document->DeleteElements(CaretState(elements->GetElementId(before_state.GetPos()), s->elements->Count()), false, true);
            //elements->Remake(CaretState& caret_state);
            document->Remake(parent->id, false);
            return true;
        }
    }

    return parent->InsertElements(_elements, before_state, after_state, with_undo);
}

bool String::DeleteElements(const CaretState& before_state, CaretState& after_state, bool with_undo)
{
    elements->RemoveAt(before_state.GetPos(), 1);
    after_state.SetState(before_state.id, before_state.selections);
    return true;
}

bool String::CanSplit(const uint max_left_width)
{
    std::string& str = ((StringElements*)elements.get())->str;
    for (size_t i = str.size() - 2; i > 0; --i) //at least one character in the splitted string
    {
        if (str[i] == ' ')
        {
            Size s = window->GetTextSize(str.substr(0, i + 1), format);
            if (s.width <= max_left_width)
                return true;
        }
    }
    return false;
}

bool String::Split(const uint max_left_width, CaretState& caret_state)
{
    std::string& str = ((StringElements*)elements.get())->str;
    for (size_t i = str.size() - 2; i > 0; --i) //at least one character in the splitted string
    {
        if (str[i] == ' ')
        {
            Size s = window->GetTextSize(str.substr(0, i + 1), format);
            if (s.width <= max_left_width)
            {
                //create new string and insert it after this one
                ElementPtr el(new String(parent, str.substr(i + 1), format));
                int pos = parent->elements->GetElementPos(id);
                parent->elements->Insert(el, pos + 1);
                str = str.substr(0, i + 1);
                UpdateRect();

                if (caret_state.IsInsideElement(id))
                {
                    //update caret state
                    if (caret_state.GetPos() > i + 1)
                        caret_state.SetState(el->id, caret_state.GetPos() - i - 1);
                }
                return true;
            }
        }
    }
    return false;
}

bool String::CanMerge(const ElementPtr with_element)
{
    if (with_element->type != ElementType::STRING || ((String*)with_element.get())->format != format)
        return false;
    return true;
}

bool String::Merge(const ElementPtr with_element, CaretState& caret_state)
{
    if (with_element->type != ElementType::STRING)
        return false;
    //merge two strings if those formats are equal
    String* el = (String*)with_element.get();
    if (el->format != format)
        return false;

    if (caret_state.IsInsideElement(with_element->id))
        caret_state.SetState(id, caret_state.GetPos() + elements->Count()); //update caret state

    elements->Insert(with_element, elements->Count());
    with_element->parent->elements->RemoveAt(with_element->parent->elements->GetElementPos(with_element->id), 1);
    return true;
}

bool String::CanContinueSelection()
{
    return true;
}

//StringElements

StringElements::StringElements(Element* parent) :
    Elements(parent)
{
}

StringElements::StringElements(Element* parent, const std::string& _str) :
    Elements(parent),
    str(_str)
{
}

void StringElements::Draw(const Selections& selections) const
{
    StringFormatPtr format = ((String*)parent)->format;
    uint start = 0, size = 0;
    if (selections.HasSelection(parent->id, start, size))
    {
        //draw the selection
        Rect r1 = parent->GetAbsoluteRect(GetCaretRect(start));
        Rect r2 = parent->GetAbsoluteRect(GetCaretRect(start + size));
        parent->window->DrawFillRect(r1.left, r1.top, r2.left - r1.left, r2.GetBottom() - r1.top, Color::Blue());
    }
    parent->window->DrawText(str, format, parent->GetAbsoluteRect()); //draw the string
}

void StringElements::Remake(CaretState& caret_state)
{
    Size s = parent->window->GetTextSize(str, ((String*)parent)->format);
    parent->rect = {1, 1, s.width, s.height};
}

// ElementPtr StringElements::Get(uint pos)
// {
//     return parent;
// }

ElementId StringElements::GetElementId(uint pos)
{
    assert(pos < str.length() + 1); //empty string has one caret pos
    ElementId id = parent->id;
    id.push_back(pos);
    return id;
}

void StringElements::Add(ElementPtr element)
{
}

void StringElements::Insert(ElementPtr element, const uint pos)
{
    assert(element->type == ElementType::STRING);
    assert(str.length() >= pos);
    String* s = dynamic_cast<String*>(element.get());
    str.insert(pos, s->ToText());
}

void StringElements::Remove(const ElementPtr element)
{
}

void StringElements::RemoveAt(const uint pos, const int size)
{
    assert(str.length() >= pos + size);
    str.erase(str.begin() + pos, str.begin() + pos + size);
}

void StringElements::Clear()
{
    str = "";
}

uint StringElements::Count()
{
    return str.length();
}

Rect StringElements::GetCaretRect(const uint pos) const
{
    Size s = parent->window->GetTextSize(str.substr(0, pos), ((String*)parent)->format);
    return Rect(s.width, 0, 1, s.height);
}

void StringElements::DrawCaret(const uint pos) const
{
    Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
    parent->window->DrawLine(r.left, r.top, r.left, r.GetBottom() - 1);
}

Rect StringElements::GetRect()
{
    Size s = parent->window->GetTextSize(str, ((String*)parent)->format);
    return Rect{0, 0, s.width, s.height};
}

bool StringElements::GetFirstCaretState(CaretState& res)
{
    res.id = GetElementId(0);
    return true;
}

bool StringElements::GetLastCaretState(CaretState& res)
{
    res.id = GetElementId(str.length());
    return true;
}

bool StringElements::GetLeftCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    uint pos = before_state.GetPos();
    if (pos == 0 || pos > str.length())
        return false;
    after_state.SetState(GetElementId(pos - 1), before_state.selections);
    if (selection)
        after_state.selections.AddSelection(parent->id, pos - 1, 1);
    return true;
}

bool StringElements::GetRightCaretState(const CaretState& before_state, CaretState& after_state, bool selection)
{
    uint pos = before_state.GetPos();
    if (pos >= str.length())
        return false;
    after_state.SetState(GetElementId(pos + 1), before_state.selections);
    if (selection)
        after_state.selections.AddSelection(parent->id, pos, 1);
    return true;
}

std::string StringElements::ToHtml()
{
    return str;
}

std::string StringElements::ToText()
{
    return str;
}

}
