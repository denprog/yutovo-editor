#include "code_string.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//CodeString

CodeString::CodeString(Element* parent) :
    String(parent)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Element* parent, const std::string str, bool translate) :
    String(parent, str, translate)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Element* parent, const std::string str, const StringFormatPtr _format, bool translate) :
    String(parent, str, _format, translate)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Element* parent, const std::u32string str) :
    String(parent, str)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Element* parent, const std::u32string str, const StringFormatPtr _format) :
    String(parent, str, _format)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Document* _document) :
    String(_document)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(Document* _document, const std::string str, const StringFormatPtr _format) :
    String(_document, str, _format)
{
    type = ElementType::CODE_STRING;
}

CodeString::CodeString(const String& source) :
    String(source)
{
    type = ElementType::CODE_STRING;
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

bool CodeString::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
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

void CodeString::Draw() const
{
    String::Draw();
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

}
