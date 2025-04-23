#include "str.h"
#include "document.h"
#include "editor_utils.h"
#include <assert.h>
#include <boost/locale.hpp>
#include <boost/lexical_cast.hpp>
#ifdef min
#undef min
#endif
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace yutovo
{

//String

String::String(Element* parent) : 
    Element(parent),
    format(parent->GetStringFormat())
{
    type = ElementType::STRING;
    can_merge = true;

    baseline = parent->window->GetFontAscent(format);

    elements.reset(new StringElements(this));
}

String::String(Element* parent, const std::string _str, bool _translate) : 
    Element(parent),
    format(parent->GetStringFormat()),
    translate(_translate)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, ToUtfString(_str)));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Element* parent, const std::string _str, const StringFormatPtr _format, bool _translate) :
    Element(parent), 
    format(_format),
    translate(_translate)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, ToUtfString(_str)));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Element* parent, const std::u32string _str) :
    Element(parent), 
    format(parent->GetStringFormat())
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Element* parent, const std::u32string _str, const StringFormatPtr _format) :
    Element(parent), 
    format(_format)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Document* _document) :
    Element(_document)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this));
}

String::String(Document* _document, const std::string _str, const StringFormatPtr _format) :
    Element(_document), 
    format(_format)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, boost::locale::conv::utf_to_utf<char32_t>(_str)));

#ifdef DEBUG
    to_str = ToText();
#endif
}

String::String(Document* _document, const std::u32string _str, const StringFormatPtr _format) :
    Element(_document), 
    format(_format)
{
    type = ElementType::STRING;
    can_merge = true;

    elements.reset(new StringElements(this, _str));

#ifdef DEBUG
    to_str = ToText();
#endif
}

Element* String::Clone()
{
    return new String(*this);
}

bool String::Copy(std::vector<ElementPtr>& copy)
{
    uint start, size;
    if (!selection->Has(id, start, size))
        return false;
    if (start == 0 && size == elements->Count())
    {
        copy.push_back(ElementPtr(Clone()));
        return true;
    }
    ElementPtr s(Create(parent, ((StringElements*)elements.get())->str.substr(start, size), format));
    s->id = id;
    copy.push_back(s);
    return true;
}

Element* String::Create(Element* parent)
{
    return new String(parent);
}

Element* String::Create(Element* parent, const std::u32string _str, const StringFormatPtr _format)
{
    return new String(parent, _str, _format);
}

void String::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Element::ToJson(value, alloc);
    rapidjson::Value _uuid(boost::uuids::to_string(format->id).c_str(), alloc);
    value.AddMember("format_id", _uuid, alloc);
}

Element* String::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("format_id") && value["format_id"].IsString())
    {
        auto format_id_str = value["format_id"].GetString();
        boost::uuids::uuid format_id;
        try
        {
            format_id = boost::lexical_cast<boost::uuids::uuid>(format_id_str);
        }
        catch (std::bad_cast& ex)
        {
            return nullptr;
        }

        auto f = document->GetStringFormat(format_id);
        if (f)
        {
            if (parent)
                return new String(parent, U"", f);
            return new String(document, U"", f);
        }
    }

    return new String(parent, U"");
}

bool String::Remake(bool with_elements)
{
    auto& str = ((StringElements*)elements.get())->str;
    if (translate && !str.empty())
    {
        str = window->Translate(id, str);
        if (str == U"")
            return true; //translation is async, it will insert new string later and remake it
        translate = false;
    }

    stretch_width = 0;
    UpdateRect();

    bool changed = (rect != last_rect);
    last_rect = rect;
    return changed;
}

void String::Normalize()
{
}

void String::UpdateRect(bool with_elements)
{
    if (last_stretch_width != stretch_width)
    {
        last_stretch_width = stretch_width;
        size_cache.clear();
    }

    Size s = GetTextSize(elements->Count());
    rect.SetSize(s.width, s.height);
    baseline = window->GetFontAscent(format);
}

bool String::GetElementAtCoords(const int x, const int y, ElementId& _id)
{
    Rect r = GetAbsoluteRect();
    if (!r.IsPointInside(x, y))
        return false;
    //find nearest caret state
    CaretState next, last, nearest;
    if (!GetFirstCaretState(next, nullptr) || !GetLastCaretState(last, nullptr))
        return false;

    nearest = next;
    r = document->GetCaretRect(nearest);
	int min_dist = r.DistToPoint(x, y);
    while (next != last)
    {
        ElementPtr el = document->GetParent(next.id);
        if (!el->GetRightCaretState(next, nullptr))
            break;
        r = document->GetCaretRect(next);
        int dist = r.DistToPoint(x, y);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = next;
        }
    }
    _id = nearest.id;
    return true;
}

std::string String::ToHtml()
{
    std::string s = "<span ";
    s += "style=\"font-family:'";
    s += format->family;
    s += "';";
    s += "font-size:";
    s += std::to_string(format->size);
    s += "px;";
    if (format->underline)
        s += "text-decoration: underline;";
    if (format->strikethrough)
        s += "text-decoration: line-through;";
    if (format->text_color != Color::Black())
        s += "color:" + format->text_color.ToString() + ";";
    if (format->text_bg_color != Color::White())
        s += "bgcolor:" + format->text_bg_color.ToString() + ";";
    s += "\">";
    if (format->bold)
        s += "<strong>";
    if (format->italic)
        s += "<em>";
    s += elements->ToHtml();
    if (format->italic)
        s += "</em>";
    if (format->bold)
        s += "</strong>";
    s += "</span>";
    return s;
}

void String::ToParserString(ParserString& str)
{
    str.Add(id, elements->ToText());
}

bool String::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    if (!editable)
        return false;
    
    if (!caret->IsInsideElement(id))
        return parent->InsertElements(_elements, with_undo, changed_element);
    
    if (_elements.size() == 1 && _elements[0]->type == type)
    {
        size_cache.clear();
        String* s = dynamic_cast<String*>(_elements[0].get());
        if (elements->Count() == 0)
        {
            //replace the string and format
            if (caret->GetPos() != 0)
                return false;
            if (with_undo)
                document->StoreUndo(id);
            elements.reset(new StringElements(this, s->elements->ToText()));
            format = s->format;
            caret->SetState(elements->GetElementId(elements->Count()));
            parent->Normalize();
            auto p = document->FindParent(id, ElementType::PARAGRAPH);
            p->elements->UpdateIds();
            changed_element = id;
#ifdef DEBUG
            to_str = ToText();
#endif
            return true;
        }
        else if (!s->format || (format && *s->format == *format))
        {
            if (with_undo)
                document->StoreUndo(id);
            elements->Insert(_elements[0], caret->GetPos());
            caret->SetState(elements->GetElementId(caret->GetPos() + s->elements->Count()));
            parent->Normalize();
            auto p = document->FindParent(id, ElementType::PARAGRAPH);
            p->elements->UpdateIds();
            changed_element = id;
#ifdef DEBUG
            to_str = ToText();
#endif
            return true;
        }
    }

    return parent->InsertElements(_elements, with_undo, changed_element);
}

bool String::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    if (!editable)
        return false;

    size_cache.clear();
    uint caret_pos = caret->GetPos();
    if (caret->IsInsideElement(id) && selection->IsEmpty())
    {
        if (caret_pos == 0 && left)
        {
            int p = parent->elements->GetElementPos(id);
            if (p > 0)
            {
                ElementPtr _el = parent->elements->Get(p - 1);
                if (_el->type == type && ((String*)_el.get())->format == format)
                {
                    if (with_undo)
                        document->StoreUndo(parent->id);
                    bool _can_merge = true;
                    if (!_el->can_merge || !can_merge)
                        _can_merge = false;
                    _el->can_merge = true;
                    can_merge = true;
                    bool r = _el->Merge(parent->elements->Get(p));
                    _el->can_merge = _can_merge;
                    changed_element = parent->id;
                    return r;
                }
            }
            return parent->DeleteElements(left, with_undo, changed_element);
        }
        if (caret_pos == elements->Count() && !left)
        {
            int p = parent->elements->GetElementPos(id);
            if (p < parent->elements->Count() - 1)
            {
                ElementPtr _el = parent->elements->Get(p + 1);
                if (_el->type == type && ((String*)_el.get())->format == format)
                {
                    if (with_undo)
                        document->StoreUndo(parent->id);
                    bool _can_merge = true;
                    if (!_el->can_merge || !can_merge)
                        _can_merge = false;
                    _el->can_merge = true;
                    can_merge = true;
                    bool r = Merge(_el);
                    can_merge = _can_merge;
                    changed_element = parent->id;
                    return r;
                }
            }
            return parent->DeleteElements(left, with_undo, changed_element);
        }
    }
    
    std::u32string& str = ((StringElements*)elements.get())->str;

    uint pos = 0;
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (with_undo)
        {
            if (selection->selection.size() > 1)
                document->StoreUndo(parent->id);
            else
                document->StoreUndo(id);
        }
        elements->RemoveAt(start, size);
        pos = start;
    }
    else if (caret->IsInsideElement(id))
    {
        if (with_undo)
        {
            if (str.size() == 1)
                document->StoreUndo(parent->id);
            else
                document->StoreUndo(id);
        }
        if (left)
        {
            elements->RemoveAt(caret_pos - 1, 1);
            pos = caret_pos - 1;
        }
        else
        {
            elements->RemoveAt(caret_pos, 1);
            pos = caret_pos;
        }
        caret->SetPos(pos, true);
    }

    CaretState before_state = caret->GetCaretState();
    auto row = document->FindParentRow(id);
    if (row)
    {
        CaretState first_state, last_state;
        row->GetFirstCaretState(first_state, nullptr);
        row->GetLastCaretState(last_state, nullptr);
        if (before_state == first_state || before_state == last_state)
            parent->parent->Normalize();
        else
            parent->Normalize();
    }

    if (str.length() == 0)
        changed_element = parent->id;
    else
        changed_element = id;

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool String::ChangeStringFormat(const StringFormatPtr _format, bool with_undo, ElementId& changed_element)
{
    if (!editable)
        return false;
    
    size_cache.clear();
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (*format == *_format)
            return false;
        if (with_undo)
            document->StoreUndo(parent->parent->id);
        if (start == 0 && size == elements->Count())
        {
            //change format of the whole string
            format = _format;
            parent->Normalize();
            auto p = document->FindParent(id, ElementType::PARAGRAPH);
            p->elements->UpdateIds();
            changed_element = id;
            window->OnCaretMoved(document->MakeEditorState());
            return true;
        }

        ElementPtr el = parent->elements->Get(id);
        if (SplitAt(start))
            el = parent->elements->Get(parent->elements->GetElementPos(id) + 1);
        el->SplitAt(size);
        ((String*)el.get())->format = _format;
        ((String*)el.get())->size_cache.clear();
        window->OnCaretMoved(document->MakeEditorState());

        auto p = document->FindParent(id, ElementType::PARAGRAPH);
        p->elements->UpdateIds();
        
        parent->Normalize();
        changed_element = id;
        return true;
    }
    return false;
}

void String::SetString(const std::u32string& str)
{
    elements.reset(new StringElements(this, str));
}

bool String::Split(const uint width, bool split_more)
{
    if (!editable)
        return false;

    size_cache.clear();

    int i = 0;
    std::u32string& str = ((StringElements*)elements.get())->str;
    for (int j = 1; j < (int)str.size() - 1; ++j) //at least one character in the splitted string
    {
        if (str[j] == ' ')
        {
            Size s = GetTextSize(j + 1);
            if (s.width <= width)
                i = j;
            else
            {
                if (split_more)
                    i = j;
                break;
            }
        }
    }

    if (i == 0)
        return false;

    uint start, size;
    bool s = selection->Has(id, start, size);
    if (s)
        selection->Remove(id, start, size);

    //create new string and insert it after this one
    ElementPtr el(Create(parent, str.substr(i + 1), format));
    int pos = parent->elements->GetElementPos(id);
    parent->elements->Insert(el, pos + 1);
    str = str.substr(0, i + 1);
    UpdateRect();

    if (caret->IsInsideElement(id))
    {
        //update caret state
        if (caret->GetPos() > i + 1)
            caret->SetState(el->id, caret->GetPos() - i - 1, true);
    }

    if (s)
    {
        if (start >= i + 1)
        {
            //move selection into the new element
            selection->Add(el->id, start - i - 1, size);
        }
        else if (start < str.length())
        {
            //split the selection
            if (str.length() - start > size)
                selection->Add(id, start, size);
            else
                selection->Add(id, start, str.length() - start);
            if (start + size > str.length())
                selection->Add(el->id, 0, size - str.length() + start);
        }
    }
#ifdef DEBUG
    parent->to_str = parent->ToText();
    to_str = ToText();
#endif
    return true;
}

bool String::SplitAt(const uint pos)
{
    if (!editable)
        return false;
    if (pos == 0 || pos >= elements->Count())
        return false;

    size_cache.clear();

    int cs_pos = -1;
    if (caret->IsInsideElement(id))
        cs_pos = caret->GetPos();
    
    std::u32string& str = ((StringElements*)elements.get())->str;
    ElementPtr el(Create(parent, str.substr(pos), format));
    int p = parent->elements->GetElementPos(id);
    parent->elements->Insert(el, p + 1);
    str = str.substr(0, pos);
    UpdateRect();
    el->UpdateRect();

    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start >= pos && size <= el->elements->Count())
        {
            selection->Remove(id, start, size);
            selection->Add(el->id, start - pos, size);
        }
        else
            selection->Optimize();
        
        if (cs_pos == pos && start == pos)
            caret->SetState(el->id, 0, true);
        else if (cs_pos == pos && start + size == pos)
            caret->SetState(id, elements->Count(), true);
        else if (cs_pos > (int)pos)
            caret->SetState(el->id, cs_pos - pos, true);
    }
    else if (cs_pos >= (int)pos)
        caret->SetState(el->id, cs_pos - pos, true);

#ifdef DEBUG
    parent->to_str = parent->ToText();
    to_str = ToText();
#endif
    return true;
}

bool String::Merge(const ElementPtr with_element)
{
    if (!editable || !document->IsString(with_element) || !with_element->can_merge)
        return false;
    //merge two strings if those formats are equal
    String* el = (String*)with_element.get();
    if (*el->format != *format)
        return false;

    size_cache.clear();

    if (caret->IsInsideElement(with_element->id))
        caret->SetState(id, caret->GetPos() + elements->Count()); //update caret state

    uint start, size = 0;
    if (selection->Has(with_element->id, start, size))
    {
        selection->Remove(with_element->id, start, size);
        selection->Add(id, elements->Count() + start, size);
    }

    elements->Insert(with_element, elements->Count());
    with_element->parent->elements->RemoveAt(with_element->parent->elements->GetElementPos(with_element->id), 1);

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool String::CanMerge(const ElementPtr with_element)
{
    if (!editable)
        return false;
    if (!document->IsString(with_element))
        return false;
    String* el = (String*)with_element.get();
    if (el->format != format)
        return false;
    return can_merge;
}

bool String::AfterInsert(bool with_undo)
{
    if (!caret)
        return false;
    CaretState c;
    if (GetLastCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

void String::BeforeDelete()
{
}

void String::BeforeReplace()
{
}

void String::AfterReplace()
{
}

void String::BeforePaste()
{
}

StringFormatPtr String::GetStringFormat() const
{
    return format;
}

void String::UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format)
{
    StringFormat f = *format;
    //change params only the same with the base format
    if (base_format->family == format->family)
        f.family = new_format->family;
    if (base_format->size == format->size)
        f.size = new_format->size;
    if (base_format->bold == format->bold)
        f.bold = new_format->bold;
    if (base_format->italic == format->italic)
        f.italic = new_format->italic;
    if (base_format->underline == format->underline)
        f.underline = new_format->underline;
    if (base_format->strikethrough == format->strikethrough)
        f.strikethrough = new_format->strikethrough;
    if (base_format->text_color == format->text_color)
        f.text_color = new_format->text_color;
    if (base_format->text_bg_color == format->text_bg_color)
        f.text_bg_color = new_format->text_bg_color;
    format = document->GetStringFormat(f.family, f.size, f.bold, f.italic, f.underline, f.strikethrough, f.text_color, f.text_bg_color);
    size_cache.clear();
}

void String::UpdateFormat(StringFormatPtr& _format)
{
    format = document->GetStringFormat(_format->family, GetFontSize(_format->size), _format->bold, _format->italic, _format->underline, 
        _format->strikethrough, _format->text_color, _format->text_bg_color);
}

int String::GetFontSize(const uint size)
{
    if (level == 1)
        return size;
    if (size - (level - 1) * 2 > 8)
        return size - (level - 1) * 2;
    return 8;
}

Size String::GetTextSize(const uint pos) const
{
    auto it = size_cache.find(pos);
    if (it == size_cache.end())
    {
        auto& str = ((StringElements*)elements.get())->str;
        auto& tabs = ((StringElements*)elements.get())->tabs;
        auto _str = str.substr(0, pos);
        Size tabs_size;
        if (!tabs.empty())
        {
            uint c = 0;
            for (size_t i = 0; i < tabs.size(); ++i)
            {
                if (tabs[i] < pos)
                {
                    _str.erase(tabs[i] - c, 1);
                    ++c;
                }
                else
                    break;
            }
            if (c > 0)
                tabs_size = window->GetTextSize(std::u32string(document->config.tab_spaces * c, U' '), format);
        }
        if (stretch_width == 0)
        {
            Size s = window->GetTextSize(_str, format);
            s.width += tabs_size.width;
            size_cache[pos] = s;
            return s;
        }

        Size s = window->GetTextSize(_str, format);
        int spaces = std::count_if(str.begin(), str.end(),
            [](char32_t c)
            {
                return std::isspace(c);
            });
        if (spaces == 0)
            return s;
        for (int i = 0; i < pos; ++i)
        {
            if (isspace(str[i]))
                s.width += floor(stretch_width / spaces);
        }
        s.width += tabs_size.width;
        return s;
    }
    return it->second;
}

bool String::CanContinueSelection()
{
    return true;
}

void String::UpdateDrawRect()
{
    draw_rect = GetAbsoluteRect();
}

void String::UpdateLevel(uint8_t _level)
{
    if (level == _level)
        return;
    level = _level;
    if (!parent)
        return;
    size_cache.clear();
    auto f = parent->GetStringFormat();
    format = document->GetStringFormat(f->family, GetFontSize(f->size), format->bold, format->italic, format->underline, 
        format->strikethrough, format->text_color, format->text_bg_color);
}

void String::SetEditable(bool _editable)
{
    editable = _editable;
}

void String::GetElements(ElementType _type, std::vector<ElementId>& _elements)
{
    if (type == _type)
        _elements.push_back(id);
}

void String::GetElementsBelow(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements)
{
}

bool String::GetNearestElement(const int x, const int y, ElementId& _id, int& dist)
{
    Rect r = GetAbsoluteRect();
    int d = r.DistToPoint(x, y);
    if (d < dist)
    {
        _id = id;
        dist = d;
        return true;
    }
    return false;
}

void String::ReSolve(bool if_error, bool force)
{
}

void String::SetStretchWidth(float val)
{
    stretch_width = val;
    last_stretch_width = val;
    size_cache.clear();
    UpdateRect();
}

//StringElements

StringElements::StringElements(Element* parent) :
    Elements(parent)
{
    UpdateTabs();
}

StringElements::StringElements(Element* parent, const std::u32string& _str) :
    Elements(parent),
    str(_str)
{
    if (parent && parent->document && !parent->document->config.use_tabs)
        boost::replace_all(str, std::u32string(U"\t"), std::u32string(parent->document->config.tab_spaces, U' '));
    UpdateTabs();
}

void StringElements::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value _str(ToBasicString(str).c_str(), alloc);
    value.AddMember("elements", _str, alloc);
}

bool StringElements::FromJson(Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("elements") || !value["elements"].IsString())
        return false;
    str = ToUtfString(value["elements"].GetString());
    UpdateTabs();
    return true;
}

Elements* StringElements::Clone(Element* _parent)
{
    return new StringElements(_parent, str);
}

void StringElements::Draw() const
{
    StringFormatPtr format = ((String*)parent)->format;
    uint start = 0, size = 0;
    if (((String*)parent)->stretch_width == 0 && tabs.empty())
    {
        parent->window->DrawText(ToBasicString(str), format, parent->GetAbsoluteRect(), format->text_color, format->text_bg_color); //draw the string
        if (parent->document->selection.Has(parent->id, start, size))
        {
            //draw text with selection
            Rect r = parent->GetAbsoluteRect();
            int p = parent->window->GetCharPos(str, format, start);
            if (str.empty() && start == 0 && size == 0)
            {
                Size s = parent->window->GetTextSize(U" ", format);
                parent->window->DrawText(" ", format, Rect{r.left + p, r.top, s.width, r.height}, 
                    format->text_bg_color, format->text_bg_selection_color);
            }
            else
            {
                std::u32string u_part = str.substr(start, size);
                parent->window->DrawText(ToBasicString(u_part), format, Rect{r.left + p, r.top, r.width - p, r.height}, 
                    format->text_bg_color, format->text_bg_selection_color);
            }
        }
    }
    else
    {
        //draw the string by symbols
        Rect r = parent->GetAbsoluteRect();
        if (parent->document->selection.Has(parent->id, start, size))
        {
            Size s1 = ((String*)parent)->GetTextSize(start);
            Size s2 = ((String*)parent)->GetTextSize(start + size);
            parent->window->DrawFillRect(Rect{r.left + s1.width, r.top, s2.width - s1.width, r.height}, format->text_bg_selection_color);
        }
        for (int i = 0; i < str.length(); ++i)
        {
            if (std::find(tabs.begin(), tabs.end(), i) == tabs.end())
            {
                Size s = ((String*)parent)->GetTextSize(i);
                std::u32string p = str.substr(i, 1);
                if (i >= start && i < start + size)
                {
                    parent->window->DrawText(ToBasicString(p), format, Rect{r.left + s.width, r.top, r.width - s.width, r.height}, 
                        format->text_bg_color, format->text_bg_selection_color);
                }
                else
                {
                    parent->window->DrawText(ToBasicString(p), format, Rect{r.left + s.width, r.top, r.width - s.width, r.height}, 
                        format->text_color, format->text_bg_color);
                }
            }
        }
    }
}

ElementPtr StringElements::Get(uint pos)
{
    if (pos > str.length())
        return nullptr;
    return parent->document->GetElement(parent->id);
}

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
    assert(parent->document->IsString(element));
    assert(str.length() >= pos);
    std::u32string s = dynamic_cast<String*>(element.get())->ToText();

    uint start, size;
    if (selection->Has(parent->id, start, size))
    {
        selection->Remove(parent->id, start, size);
        str.insert(pos, s);
        selection->Add(parent->id, start, size);
    }
    else
        str.insert(pos, s);

    if (selection->Has(element, start, size))
    {
        selection->Add(parent->id, Count(), size);
        selection->Remove(element->id, start, size);
    }

    UpdateTabs();

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void StringElements::Remove(const ElementPtr element)
{
    assert(false);
}

void StringElements::RemoveAt(const uint pos, const int size)
{
    assert(str.length() >= pos + size);
    int p = -1;
    if (caret->IsInsideElement(parent->id))
    {
        p = caret->GetPos();
        if (p >= pos + size)
            caret->SetPos(p - size);
    }

    str.erase(str.begin() + pos, str.begin() + pos + size);
    selection->Remove(parent->id, pos, size);

    UpdateTabs();

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

void StringElements::Clear()
{
    str = U"";

#ifdef DEBUG
    parent->to_str = parent->ToText();
#endif
}

uint StringElements::Count() const
{
    return str.length();
}

Rect StringElements::GetCaretRect(const uint pos) const
{
    Size s = ((String*)parent)->GetTextSize(pos);
    return Rect(s.width, 0, 1, s.height);
}

void StringElements::DrawCaret(const uint pos) const
{
    Rect r = parent->GetAbsoluteRect(GetCaretRect(pos));
    if (Count() == 0)
        parent->window->DrawLine(r.left + parent->rect.width / 2, r.top, r.left + parent->rect.width / 2, r.GetBottom() - 1, Color::Black());
    else
        parent->window->DrawLine(r.left, r.top, r.left, r.GetBottom() - 1, Color::Black());
}

Rect StringElements::GetRect()
{
    Size s = ((String*)parent)->GetTextSize(str.length());
    return Rect{0, 0, s.width, s.height};
}

Rect StringElements::GetRect(const uint pos)
{
    String* p = (String*)parent;
    Size s1 = p->GetTextSize(pos);
    Size s2 = p->GetTextSize(pos + 1);
    return Rect{s1.width, p->rect.top, s2.width - s1.width, s2.height};
}

bool StringElements::GetFirstCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(parent->id))
        select->Add(parent->id, 0, caret_state.GetPos());
    caret_state.id = GetElementId(0);
    return true;
}

bool StringElements::GetLastCaretState(CaretState& caret_state, Selection* select)
{
    if (select && caret_state.IsInsideElement(parent->id))
        select->Add(parent->id, caret_state.GetPos(), str.length() - caret_state.GetPos());
    caret_state.id = GetElementId(str.length());
    return true;
}

bool StringElements::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos == 0 || pos > str.length())
        return false;
    caret_state.SetState(GetElementId(pos - 1));
    if (select)
        select->Add(parent->id, pos - 1, 1);
    return true;
}

bool StringElements::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos >= str.length())
        return false;
    caret_state.SetState(GetElementId(pos + 1));
    if (select)
        select->Add(parent->id, pos, 1);
    return true;
}

bool StringElements::GetWordLeftCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos == 0 || pos > str.length())
        return false;
    for (int i = pos - 1; i > 0; --i)
    {
        if (isspace(str[i]))
        {
            while (isspace(str[i]))
                --i;
            ++i;
        }
        if ((IsOpenDelimiter(str[i - 1]) || IsCloseDelimiter(str[i]) || IsOpenDelimiter(str[i])) || 
            (IsCloseDelimiter(str[i - 1]) || IsDelimiter(str[i - 1]) || str[i] == U'.' || str[i] == U',') && !(str[i - 1] == U'.' || str[i - 1] == U','))
        {
            caret_state.SetState(GetElementId(i));
            if (select)
                select->Add(parent->id, i, pos - i);
            return true;
        }
    }
    if (select)
    {
        if (select->IsSelected(parent->parent->parent->id))
            select->Add(parent->parent->parent->id);
        else
            select->Add(parent->id, 0, pos);
    }
    return GetFirstCaretState(caret_state, nullptr);
}

bool StringElements::GetWordRightCaretState(CaretState& caret_state, Selection* select)
{
    uint pos = caret_state.GetPos();
    if (pos >= str.length())
        return false;
    if (pos == str.length() - 1 && !str.empty() && str[str.length() - 1] == ' ')
    {
        if (select)
            select->Add(parent->id, pos, 1);
        return parent->parent->GetWordRightCaretState(caret_state, select);
    }
    for (int i = pos + 1; i < str.length(); ++i)
    {
        while (isspace(str[i]))
            ++i;
        if ((IsCloseDelimiter(str[i]) || IsCloseDelimiter(str[i - 1])) ||
            (IsOpenDelimiter(str[i - 1]) || IsDelimiter(str[i - 1]) || str[i] == U'.' || str[i] == U',') && 
            !(str[i - 1] == U'.' || str[i - 1] == U','))
        {
            caret_state.SetState(GetElementId(i));
            if (select)
                select->Add(parent->id, pos, i - pos);
            return true;
        }
    }
    if (select)
    {
        if (pos == 0 && parent->parent->elements->Count() == 1 && parent->parent->parent->elements->Count() == 1)
            select->Add(parent->parent->parent->id);
        else
            select->Add(parent->id, pos, Count() - pos);
    }
    return GetLastCaretState(caret_state, nullptr);
}

bool StringElements::GetSelectOutCaretState(CaretState& caret_state, Selection* select)
{
    if (!select || !caret_state.IsInsideElement(parent->id))
        return false;
    
    static std::u32string delims = U" \n\t\v\f\r!\"#$%&\'()*+,-./[\\]^`{|}~";
    auto is_delim = 
        [](char32_t ch)
        {
            for (char32_t d : delims)
            {
                if (ch == d)
                    return true;
            }
            return false;
        };

    int p = caret_state.GetPos() - 1;
    while (p >= 0)
    {
        char32_t ch = str[p];
        if (IsOpenDelimiter(ch) || IsCloseDelimiter(ch) || IsDelimiter(ch))
            break;
        --p;
    }
    int s1 = p++ + 1;
    while (p < (int)str.length())
    {
        char32_t ch = str[p];
        if (IsOpenDelimiter(ch) || IsCloseDelimiter(ch) || IsDelimiter(ch))
            break;
        ++p;
    }
    if (p - s1 <= 0)
        return false;
    select->Add(parent->id, s1, p - s1);
    caret_state.SetState(parent->id, p);
    return true;
}

std::string StringElements::ToHtml()
{
    return ToBasicString(str);
}

std::u32string StringElements::ToText()
{
    return str;
}

bool StringElements::IsOpenDelimiter(char32_t ch)
{
    static std::u32string delims = U"«([{";
    for (char32_t d : delims)
    {
        if (ch == d)
            return true;
    }
    return false;
}

bool StringElements::IsCloseDelimiter(char32_t ch)
{
    static std::u32string delims = U"»)]}";
    for (char32_t d : delims)
    {
        if (ch == d)
            return true;
    }
    return false;
}

bool StringElements::IsDelimiter(char32_t ch)
{
    static std::u32string delims = U" \n\t\v\f\r!\"#$%&\'*+,-./\\^`|~";
    for (char32_t d : delims)
    {
        if (ch == d)
            return true;
    }
    return false;
}

void StringElements::UpdateTabs()
{
    tabs.clear();
    size_t p = 0;
    while (p != std::string::npos)
    {
        p = str.find(U'\t', p);
        if (p != std::string::npos)
            tabs.push_back(p++);
    }
}

}
