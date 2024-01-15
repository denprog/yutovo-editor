#include "style.h"
#include "document.h"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

//StringFormat

StringFormat::StringFormat(const boost::uuids::uuid _id, const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, 
    Color _text_color, Color _text_bg_color, Color _text_bg_selection_color) :
    id(_id),
    family(_family), 
    size(_size),
    bold(_bold),
    italic(_italic),
    underline(_underline),
    text_color(_text_color),
    text_bg_color(_text_bg_color),
    text_bg_selection_color(_text_bg_selection_color)
{
}

StringFormat::StringFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, Color _text_color, 
    Color _text_bg_color, Color _text_bg_selection_color) :
    StringFormat(boost::uuids::random_generator()(), _family, _size, _bold, _italic, _underline, _text_color, _text_bg_color, _text_bg_selection_color)
{
}

bool StringFormat::operator==(const StringFormat& f) const
{
    return family == f.family && size == f.size && bold == f.bold && italic == f.italic && underline == f.underline && 
        text_color == f.text_color && text_bg_color == f.text_bg_color && text_bg_selection_color == f.text_bg_selection_color;
}

bool StringFormat::operator!=(const StringFormat& f) const
{
    return !operator==(f);
}

void StringFormat::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Value _uuid(boost::uuids::to_string(id).c_str(), alloc);
    obj.AddMember("id", _uuid, alloc);
    rapidjson::Value _family(family.c_str(), alloc);
    obj.AddMember("family", _family, alloc);
    obj.AddMember("size", size, alloc);
    obj.AddMember("bold", bold, alloc);
    obj.AddMember("italic", italic, alloc);
    obj.AddMember("underline", underline, alloc);
    obj.AddMember("text_color", text_color.ToInt(), alloc);
    obj.AddMember("text_bg_color", text_bg_color.ToInt(), alloc);
    obj.AddMember("text_bg_selection_color", text_bg_selection_color.ToInt(), alloc);
    value.PushBack(obj, alloc);
}

bool StringFormat::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("id") || !value["id"].IsString())
        return false;
    auto id_str = value["id"].GetString();
    try
    {
        id = boost::lexical_cast<boost::uuids::uuid>(id_str);
    }
    catch (std::bad_cast& ex)
    {
        return false;
    }

    if (!value.HasMember("family") || !value["family"].IsString())
        return false;
    family = value["family"].GetString();

    if (!value.HasMember("size"))
        return false;
    if (value["size"].IsFloat())
        size = (int)value["size"].GetFloat();
    else if (value["size"].IsInt())
        size = value["size"].GetInt();
    else
        return false;

    if (!value.HasMember("bold") || !value["bold"].IsBool())
        return false;
    bold = value["bold"].GetBool();

    if (!value.HasMember("italic") || !value["italic"].IsBool())
        return false;
    italic = value["italic"].GetBool();

    if (!value.HasMember("underline") || !value["underline"].IsBool())
        return false;
    underline = value["underline"].GetBool();

    if (!value.HasMember("text_color") || !value["text_color"].IsUint())
        text_color = Color::Black();
    else
        text_color = Color::FromInt(value["text_color"].GetUint());

    if (!value.HasMember("text_bg_color") || !value["text_bg_color"].IsUint())
        text_bg_color = Color::White();
    else
        text_bg_color = Color::FromInt(value["text_bg_color"].GetUint());

    if (!value.HasMember("text_bg_selection_color") || !value["text_bg_selection_color"].IsUint())
        text_bg_selection_color = Color::Blue();
    else
        text_bg_selection_color = Color::FromInt(value["text_bg_selection_color"].GetUint());

    return true;
}

void StringFormat::Reset()
{
    family = "";
    size = 0;
    bold = false;
    italic = false;
    underline = false;
}

//StringFormats

StringFormats::~StringFormats()
{
}

StringFormatPtr StringFormats::GetFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, Color _text_color, 
    Color _text_bg_color, Color _text_bg_selection_color)
{
    StringFormatPtr format(new StringFormat(_family, _size, _bold, _italic, _underline, _text_color, _text_bg_color, _text_bg_selection_color));
    //return the present format
    for (StringFormatPtr& f : string_formats)
    {
        if (*f == *format)
            return f;
    }

    //or create a new one
    string_formats.push_back(format);
    return format;
}

StringFormatPtr StringFormats::GetFormat(const StringFormat& source)
{
    //return the present format
    for (StringFormatPtr& f : string_formats)
    {
        if (*f == source)
            return f;
    }

    //or create a new one
    StringFormatPtr f(new StringFormat(source.family, source.size, source.bold, source.italic, source.underline, Color::Black(), Color::White(), Color::Blue()));
    string_formats.push_back(f);
    return f;
}

StringFormatPtr StringFormats::GetFormat(const boost::uuids::uuid& _id)
{
    auto it = std::find_if(string_formats.begin(), string_formats.end(), 
        [_id](auto& f)
        {
            return f->id == _id;
        });
    if (it != string_formats.end())
        return *it;
    return nullptr;
}

void StringFormats::AddFormats(const StringFormats& source)
{
    for (auto& f : source.string_formats)
    {
        auto it = std::find_if(string_formats.begin(), string_formats.end(), 
            [f](const StringFormatPtr format)
            {
                return format->id == f->id;
            });
        if (it == string_formats.end())
            string_formats.push_back(f);
    }
}

void StringFormats::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    for (auto& f : string_formats)
        f->ToJson(value, alloc);
}

bool StringFormats::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.IsArray())
        return false;
    std::vector<StringFormatPtr> _string_formats;
    rapidjson::GenericArray arr = value.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return false;
        rapidjson::Value value = arr[i].GetObject();
        StringFormatPtr s(new StringFormat());
        if (!s->FromJson(value, alloc))
            return false;
        auto it = std::find_if(string_formats.begin(), string_formats.end(), 
            [s](auto& f)
            {
                return f->id == s->id;
            });
        if (it == string_formats.end())
            string_formats.push_back(s);
    }
    return true;
}

//ParagraphFormat

ParagraphFormat::ParagraphFormat(std::string _name, Alignment _alignment, WordWrap _word_wrap, uint _line_spacing, uint _indent_before, uint _indent_after, 
    uint _indent_first_line, uint _spacing_before, uint _spacing_after, StringFormatPtr _string_format) :
    name(_name),
    alignment(_alignment),
    word_wrap(_word_wrap),
    line_spacing(_line_spacing),
    indent_before(_indent_before),
    indent_after(_indent_after),
    indent_first_line(_indent_first_line), 
    spacing_before(_spacing_before),
    spacing_after(_spacing_after),
    default_string_format(_string_format)
{
}

bool ParagraphFormat::operator==(const ParagraphFormat& f) const
{
    return name == f.name && alignment == f.alignment && word_wrap == f.word_wrap && line_spacing == f.line_spacing && indent_before == f.indent_before && 
        indent_after == f.indent_after && indent_first_line == f.indent_first_line && spacing_before == f.spacing_before && spacing_after == f.spacing_after && 
        *default_string_format == *f.default_string_format;
}

void ParagraphFormat::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Value _name(name.c_str(), alloc);
    obj.AddMember("name", _name, alloc);
    obj.AddMember("alignment", (int)alignment, alloc);
    obj.AddMember("word_wrap", (int)word_wrap, alloc);
    obj.AddMember("line_spacing", line_spacing, alloc);
    obj.AddMember("indent_before", indent_before, alloc);
    obj.AddMember("indent_after", indent_after, alloc);
    obj.AddMember("indent_first_line", indent_first_line, alloc);
    obj.AddMember("spacing_before", spacing_before, alloc);
    obj.AddMember("spacing_after", spacing_after, alloc);
    rapidjson::Value _uuid(boost::uuids::to_string(default_string_format->id).c_str(), alloc);
    obj.AddMember("default_string_format", _uuid, alloc);
    value.PushBack(obj, alloc);
}

bool ParagraphFormat::FromJson(Document* document, rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("default_string_format") || !value["default_string_format"].IsString())
        return false;
    
    auto id_str = value["default_string_format"].GetString();
    try
    {
        auto id = boost::lexical_cast<boost::uuids::uuid>(id_str);
        default_string_format = document->string_formats->GetFormat(id);
    }
    catch (std::bad_cast& ex)
    {
        return false;
    }

    if (!default_string_format)
        default_string_format = document->string_formats->GetFormat("Arial", 14, false, false, false, Color::Black(), Color::White(), Color::Blue());

    if (!value.HasMember("name") || !value["name"].IsString())
        return false;
    name = value["name"].GetString();

    if (!value.HasMember("alignment") || !value["alignment"].IsInt())
        return false;
    alignment = (Alignment)value["alignment"].GetInt();

    if (!value.HasMember("word_wrap") || !value["word_wrap"].IsInt())
        return false;
    word_wrap = (WordWrap)value["word_wrap"].GetInt();

    if (!value.HasMember("line_spacing") || !value["line_spacing"].IsInt())
        return false;
    line_spacing = value["line_spacing"].GetInt();

    if (!value.HasMember("indent_before") || !value["indent_before"].IsInt())
        return false;
    indent_before = value["indent_before"].GetInt();

    if (!value.HasMember("indent_after") || !value["indent_after"].IsInt())
        return false;
    indent_after = value["indent_after"].GetInt();

    if (!value.HasMember("indent_first_line") || !value["indent_first_line"].IsInt())
        return false;
    indent_first_line = value["indent_first_line"].GetInt();

    if (!value.HasMember("spacing_before") || !value["spacing_before"].IsInt())
        return false;
    spacing_before = value["spacing_before"].GetInt();

    if (!value.HasMember("spacing_after") || !value["spacing_after"].IsInt())
        return false;
    spacing_after = value["spacing_after"].GetInt();

    return true;
}

//ParagraphFormats

ParagraphFormats::ParagraphFormats(StringFormatsPtr _string_formats) :
    string_formats(_string_formats)
{
    //those are predefined paragraph styles
    GetFormat("Text body", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Arial", 14, false, false, false, Color::Black(), Color::White(), Color::Blue()));
    GetFormat("Header 1", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Arial", 30, true, false, false, Color::Black(), Color::White(), Color::Blue()));
    GetFormat("Header 2", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Arial", 26, true, false, false, Color::Black(), Color::White(), Color::Blue()));
    GetFormat("Header 3", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10,
        string_formats->GetFormat("Arial", 22, true, false, false, Color::Black(), Color::White(), Color::Blue()));
    GetFormat("Monospace", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Courier New", 12, false, false, false, Color::Black(), Color::White(), Color::Blue()));
    GetFormat("Code", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::None, 2, 2, 2, 0, 2, 2, 
        string_formats->GetFormat("Courier New", 14, false, false, false, Color::Black(), Color::White(), Color::Blue()));
}

ParagraphFormatPtr ParagraphFormats::GetFormat(std::string _name, ParagraphFormat::Alignment _alignment, ParagraphFormat::WordWrap _word_wrap, 
    uint _line_spacing, uint _indent_before, uint _indent_after, uint _indent_first_line, uint _spacing_before, uint _spacing_after, 
    StringFormatPtr _string_format)
{
    //return the present format
    for (auto& p : paragraph_formats)
    {
        if (p->name == _name && p->alignment == _alignment && p->word_wrap == _word_wrap && p->line_spacing == _line_spacing && 
            p->indent_before == _indent_before && p->indent_after == _indent_after && p->indent_first_line == _indent_first_line && 
            p->spacing_before == _spacing_before && p->spacing_after == _spacing_after && *p->default_string_format == *_string_format)
            return p;
    }

    //or create a new one
    ParagraphFormatPtr p(new ParagraphFormat(_name, _alignment, _word_wrap, _line_spacing, _indent_before, _indent_after, _indent_first_line, 
        _spacing_before, _spacing_after, _string_format));
    paragraph_formats.push_back(p);
    return p;
}

ParagraphFormatPtr ParagraphFormats::GetFormat(const std::string& name)
{
    auto it = std::find_if(paragraph_formats.begin(), paragraph_formats.end(), 
        [name](auto& p)
        {
            return p->name == name;
        });
    if (it == paragraph_formats.end())
        return nullptr;
    return *it;
}

void ParagraphFormats::GetFormats(std::vector<ParagraphFormatPtr>& formats)
{
    formats = paragraph_formats;
}

void ParagraphFormats::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    for (auto& f : paragraph_formats)
        f->ToJson(value, alloc);
}

bool ParagraphFormats::FromJson(Document* document, rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.IsArray())
        return false;
    std::vector<ParagraphFormatPtr> _paragraph_formats;
    rapidjson::GenericArray arr = value.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return false;
        rapidjson::Value value = arr[i].GetObject();
        ParagraphFormatPtr p(new ParagraphFormat());
        if (!p->FromJson(document, value, alloc))
            return false;
        auto it = std::find_if(paragraph_formats.begin(), paragraph_formats.end(), 
            [p](auto& f)
            {
                return *f == *p;
            });
        if (it == paragraph_formats.end())
            paragraph_formats.push_back(p);
    }
    return true;
}

//FormulaFormat

FormulaFormat::FormulaFormat(const std::string& _name, StringFormatPtr _string_format, uint _inter_spacing, 
    int _left_margin, int _top_margin, int _right_margin, int _bottom_margin, Color _color, Color _bg_color, Color _bg_selection_color) : 
    name(_name),
    string_format(_string_format),
    inter_spacing(_inter_spacing),
    left_margin(_left_margin),
    top_margin(_top_margin),
    right_margin(_right_margin),
    bottom_margin(_bottom_margin),
    color(_color),
    bg_color(_bg_color),
    bg_selection_color(_bg_selection_color)
{
}

bool FormulaFormat::operator==(const FormulaFormat& f) const
{
    return name == f.name && *string_format == *f.string_format && inter_spacing == f.inter_spacing &&
        left_margin == f.left_margin &&  top_margin == f.top_margin && right_margin == f.right_margin && bottom_margin == f.bottom_margin &&
        color == f.color && bg_color == f.bg_color && bg_selection_color == f.bg_selection_color;
}

//FormulaFormats

FormulaFormats::FormulaFormats(StringFormatsPtr _string_formats) :
    string_formats(_string_formats)
{
    GetFormat("Code", string_formats->GetFormat("Courier New", 14, false, false, false, Color::Black(), Color::White(), Color::Blue()), 
        2, 5, 2, 5, 2, Color::Black(), Color::White(), Color::Blue());
    GetFormat("Formula", string_formats->GetFormat("Courier New", 14, false, false, false, Color::Black(), Color::White(), Color::Blue()), 
        2, 2, 2, 2, 2, Color::Black(), Color::White(), Color::Blue());
    GetFormat("OnlyShapeFormula", string_formats->GetFormat("Courier New", 14, false, false, false, Color::Black(), Color::White(), Color::Blue()), 
        0, 0, 0, 0, 0, Color{0xff, 0x40, 0x40, 0x40}, Color::White(), Color::Blue());
}

FormulaFormatPtr FormulaFormats::GetFormat(const std::string& name)
{
    for (auto f : formula_formats)
    {
        if (f->name == name)
            return f;
    }
    return nullptr;
}

FormulaFormatPtr FormulaFormats::GetFormat(const std::string& name, StringFormatPtr string_format, uint inter_spacing, 
    int left_margin, int top_margin, int right_margin, int bottom_margin, Color color, Color bg_color, Color bg_selection_color)
{
    FormulaFormatPtr format(new FormulaFormat(name, string_format, inter_spacing, 
        left_margin, top_margin, right_margin, bottom_margin, color, bg_color, bg_selection_color));
    
    //return the present format
    for (auto f : formula_formats)
    {
        if (*f == *format)
            return f;
    }

    //or create a new one
    formula_formats.emplace_back(format);
    return format;
}

//CodeFormat

CodeFormat::CodeFormat(const std::string& _name, uint _left_indent, uint _top_indent, uint _right_indent, uint _bottom_indent, 
    uint _left_margin, uint _top_margin, uint _right_margin, uint _bottom_margin, uint _paragraph_spacing, Color _border_color) :
    name(_name), 
    left_indent(_left_indent),
    top_indent(_top_indent),
    right_indent(_right_indent),
    bottom_indent(_bottom_indent),
    left_margin(_left_margin),
    top_margin(_top_margin),
    right_margin(_right_margin),
    bottom_margin(_bottom_margin), 
    paragraph_spacing(_paragraph_spacing),
    border_color(_border_color)
{
}

bool CodeFormat::operator==(const CodeFormat& c) const
{
    return name == c.name && left_indent == c.left_indent && top_indent == c.top_indent && bottom_indent == c.bottom_indent && 
        paragraph_spacing == c.paragraph_spacing && border_color == c.border_color;
}

//CodeFormats

CodeFormats::CodeFormats()
{
}

CodeFormatPtr CodeFormats::GetFormat(const std::string& name)
{
    for (auto c : code_formats)
    {
        if (c->name == name)
            return c;
    }
    return nullptr;
}

CodeFormatPtr CodeFormats::GetFormat(const std::string& name, uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, 
    uint left_margin, uint top_margin, uint right_margin, uint bottom_margin, uint paragraph_spacing, Color border_color)
{
    CodeFormatPtr format(new CodeFormat(name, left_indent, top_indent, right_indent, bottom_indent, 
        left_margin, top_margin, right_margin, bottom_margin, paragraph_spacing, border_color));
    
    //return the present format
    for (auto c : code_formats)
    {
        if (*c == *format)
            return c;
    }

    //or create a new one
    code_formats.emplace_back(format);
    return format;
}

//PageFormats

bool PageFormat::operator==(const PageFormat& p) const
{
    return left_indent == p.left_indent && top_indent == p.top_indent && right_indent == p.right_indent && 
        bottom_indent == p.bottom_indent && paragraph_spacing == p.paragraph_spacing;
}

std::vector<PageFormatPtr> PageFormats::page_formats;

PageFormatPtr PageFormats::GetFormat(uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, uint paragraph_spacing)
{
    for (auto& p : page_formats)
    {
        if (p->left_indent == left_indent && p->top_indent == top_indent && p->right_indent == right_indent && p->bottom_indent == bottom_indent && 
            p->paragraph_spacing == paragraph_spacing)
            return p;
    }

    PageFormatPtr p(new PageFormat{left_indent, top_indent, right_indent, bottom_indent, paragraph_spacing});
    page_formats.push_back(p);
    return p;
}

//TextFormat

bool TextFormat::operator==(const TextFormat& t) const
{
    return paging == t.paging;
}

std::vector<TextFormatPtr> TextFormats::text_formats;

TextFormatPtr TextFormats::GetFormat(TextFormat::Paging paging)
{
    for (auto& t : text_formats)
    {
        if (t->paging == paging)
            return t;
    }

    TextFormatPtr t(new TextFormat{paging});
    text_formats.push_back(t);
    return t;
}

}
