#include "code_string.h"
#include "result.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//CodeString

CodeString::CodeString(Element* parent) :
    String(parent)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::string str, bool translate) :
    String(parent, str, translate)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::string str, const StringFormatPtr _format, bool translate) :
    String(parent, str, _format, translate)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::u32string str) :
    String(parent, str)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Element* parent, const std::u32string str, const StringFormatPtr _format) :
    String(parent, str, _format)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Document* _document) :
    String(_document)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Document* _document, const std::string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(Document* _document, const std::u32string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

CodeString::CodeString(const String& source) :
    String(source)
{
    type = ElementType::CODE_STRING;
    UpdateGap();
}

Element* CodeString::Clone()
{
    return new CodeString(*this);
}

Element* CodeString::Create(Element* parent)
{
    return new CodeString(parent);
}

Element* CodeString::Create(Element* parent, const std::u32string _str, const StringFormatPtr _format)
{
    return new CodeString(parent, _str, _format);
}

void CodeString::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    String::ToJson(value, alloc);
    value.AddMember("can_merge", can_merge, alloc);
}

Element* CodeString::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    CodeString* r = nullptr;
    bool _can_merge = true;
    if (value.HasMember("can_merge") && value["can_merge"].IsBool())
        _can_merge = value["can_merge"].GetBool();
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
                r = new CodeString(parent, U"", f);
            else
                r = new CodeString(document, "", f);
            r->can_merge = _can_merge;
            return r;
        }
    }

    if (parent)
        r = new CodeString(parent);
    else
        r = new CodeString(document);
    r->can_merge = _can_merge;
    return r;
}

bool CodeString::Remake(bool with_elements)
{
    UpdateGap();
    return String::Remake(with_elements);
}

bool CodeString::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    gap = 0;
    UpdateGap();

    ElementId _changed_element;
    for (auto& el : _elements)
    {
        if (document->IsString(el))
        {
            String* s = dynamic_cast<String*>(el.get());
            if (s->ToText().find(U" ") != std::string::npos)
            {
                if (with_undo)
                    document->StoreUndo(parent->id);
                _changed_element = parent->id;
                break;
            }
        }
    }
    bool r = String::InsertElements(_elements, with_undo, changed_element);
    if (!_changed_element.empty() && _changed_element.size() < changed_element.size())
        changed_element = _changed_element;
    return r;
}

bool CodeString::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    gap = 0;
    UpdateGap();
    return String::DeleteElements(left, with_undo, changed_element);
}

bool CodeString::ChangeStringFormat(const StringFormatPtr format, bool with_undo, ElementId& changed_element)
{
    gap = 0;
    UpdateGap();
    return String::ChangeStringFormat(format, with_undo, changed_element);
}

void CodeString::Draw() const
{
    int start = 0, size = 0;
    std::u32string str = elements->ToText();
    Rect r = GetAbsoluteRect();

    document->selection.Has(id, (uint&)start, (uint&)size);
    if (gap == 0)
    {
        window->DrawText(ToBasicString(str), format, r, format->text_color, format->text_bg_color); //draw the string
        if (size != 0)
        {
            //draw text with selection
            Rect r = GetAbsoluteRect();
            int p = window->GetCharPos(str, format, start);
            std::u32string u_part = str.substr(start, size);
            window->DrawText(ToBasicString(u_part), format, Rect{r.left + p, r.top, r.width - p, r.height}, 
                format->text_bg_color, format->text_bg_selection_color);
        }
    }
    else
    {
        for (int i = 0; i < str.length(); ++i)
        {
            Size s = GetTextSize(i);
            std::u32string p = str.substr(i, 1);
            if (i >= start && i < start + size)
            {
                window->DrawText(ToBasicString(p), format, Rect{r.left + s.width, r.top, r.width - s.width, r.height}, 
                    format->text_bg_color, format->text_bg_selection_color);
            }
            else
            {
                window->DrawText(ToBasicString(p), format, Rect{r.left + s.width, r.top, r.width - s.width, r.height}, 
                    format->text_color, format->text_bg_color);
            }
        }
    }

    if (document->HasErrorMark(id, start, size))
        DrawErrorMark(start, size);
    
    if (elements->Count() == 0)
    {
        auto f = GetFormulaFormat();
        uint start = 0, size = 0;
        if (document->selection.Has(parent->id, start, size))
        {
            auto r = GetAbsoluteRect();
            window->DrawRect(r, f->bg_selection_color);
            window->DrawFillRect(Rect(r.left + 1, r.top + 1, r.width - 2, r.height -2 ), f->bg_selection_color);
        }
        else
        {
            auto row = document->FindParentRow(id);
            if (row->parent->type != ElementType::CODE_PARAGRAPH || parent->elements->Count() > 1)
                window->DrawRect(GetAbsoluteRect(), f->bg_selection_color);
        }
    }
}

void CodeString::UpdateRect(bool with_elements)
{
    String::UpdateRect(with_elements);

    if (elements->Count() == 0)
        rect.SetSize(rect.width + empty_rect_width, rect.height);
    baseline = rect.height / 2;
}

Rect CodeString::GetCaretRect(const uint pos) const
{
    Rect r = String::GetCaretRect(pos);
    if (elements->Count() > 0)
        return r;
    r.left += rect.width / 2;
    return r;
}

Size CodeString::GetTextSize(const uint pos) const
{
    auto it = size_cache.find(pos);
    if (it == size_cache.end())
    {
        auto& str = ((StringElements*)elements.get())->str;
        auto _str = str.substr(0, pos);
        if (gap == 0)
        {
            Size s = window->GetTextSize(_str, format);
            size_cache[pos] = s;
            return s;
        }

        Size s = window->GetTextSize(_str, format);
        int _gap = gap;
        auto point_pos = str.find(U".");
        if (point_pos != std::string::npos)
        {
            if (point_pos < gap)
                _gap = gap;
            else
            {
                _gap = point_pos % gap;
                if (_gap == 0)
                    _gap = gap;
            }
        }
        else
        {
            _gap = str.length() % gap;
            if (_gap == 0)
                _gap = gap;
        }
        
        for (int i = 0; i < pos; ++i)
        {
            if (str[i] == U'.')
            {
                _gap = gap;
                continue;
            }
            if (_gap == 0)
            {
                s.width += gap_width;
                _gap = gap;
            }
            if (i < str.length() - 1 && str[i + 1] != U'.')
                --_gap;
        }

        if (_gap == 0 && pos < str.length())
            s.width += gap_width;
        size_cache[pos] = s;
        return s;
    }
    return it->second;
}

void CodeString::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    const FormulaFormatPtr f = GetFormulaFormat();
    left = f->left_margin;
    top = f->top_margin;
    right = f->right_margin;
    bottom = f->bottom_margin;
}

bool CodeString::IsFormula()
{
    return true;
}

std::string CodeString::ToHtml()
{
    std::string s = "<mi>";
    if (elements->Count() == 0)
        s += "Null";
    else
        s += elements->ToHtml();
    s += "</mi>";
    return s;
}

void CodeString::UpdateGap()
{
    int _gap = gap;
    Config& config = document->config;
    if (!config.use_numbers_gaps)
    {
        _gap = 0;
    }
    else
    {
        auto& str = ((StringElements*)elements.get())->str;
        if (parent && parent->parent && 
            ((parent->parent->type == ElementType::SUBSCRIPT && parent->parent->elements->Count() == 3) || 
            (parent->parent->type == ElementType::INTEGER_RESULT)))
        {
            Notation notation = Notation::None;
            if (parent->parent->type == ElementType::INTEGER_RESULT)
                notation = ((IntegerResult*)parent->parent)->config.result_notation;
            else
            {
                auto last = parent->parent->elements->Get(2)->ToText();
                if (last == U"bin")
                    notation = Notation::Binary;
                else if (last == U"oct")
                    notation = Notation::Octal;
                else if (last == U"hex")
                    notation = Notation::Hexadecimal;
                else
                    notation = Notation::Decimal;
            }

            switch (notation)
            {
            case Notation::None:
                break;
            case Notation::Binary:
                if (str.find_first_not_of(U"01") == string::npos)
                    _gap = document->config.binary_gap;
                else
                    _gap = 0;
                break;
            case Notation::Decimal:
                if (str.find_first_not_of(U"0123456789") == string::npos)
                    _gap = document->config.decimal_gap;
                else
                    _gap = 0;
                break;
            case Notation::Octal:
                if (str.find_first_not_of(U"01234567") == string::npos)
                    _gap = document->config.octal_gap;
                else
                    _gap = 0;
                break;
            case Notation::Hexadecimal:
                if (str.find_first_not_of(U"0123456789abcdefABCDEF") == string::npos)
                    _gap = document->config.hexadecimal_gap;
                else
                    _gap = 0;
                break;
            }
        }
        else
        {
            if (str.find_first_not_of(U"0123456789.") == string::npos)
            {
                if (std::count(str.begin(), str.end(), '.') > 1)
                    _gap = 0;
                else
                    _gap = document->config.decimal_gap;
            }
            else
                _gap = 0;
        }
    }

    if (gap != _gap)
    {
        Size s = window->GetTextSize(U" ", format);
        gap_width = s.width / 2;
        gap = _gap;
        size_cache.clear();
    }
}

}
