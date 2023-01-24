#include "style.h"
#include "document.h"
#include <algorithm>

namespace yutovo
{

//StringFormat

uint StringFormat::next_id = 1;

StringFormat::StringFormat(const uint _id, const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, Color _color, Color _selection_color) :
    id(_id),
    family(_family), 
    size(_size),
    bold(_bold),
    italic(_italic),
    underline(_underline),
    color(_color),
    selection_color(_selection_color)
{
}

StringFormat::StringFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, Color _color, Color _selection_color) :
    StringFormat(next_id++, _family, _size, _bold, _italic, _underline, _color, _selection_color)
{
}

bool StringFormat::operator==(const StringFormat& f)
{
    return family == f.family && size == f.size && bold == f.bold && italic == f.italic && underline == f.underline && 
        color == f.color && selection_color == f.selection_color;
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

StringFormatPtr StringFormats::GetFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline)
{
    return GetFormat(_family, _size, _bold, _italic, _underline, Color::Black(), Color::White());
}

StringFormatPtr StringFormats::GetFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline, Color _color, Color _selection_color)
{
    StringFormatPtr format(new StringFormat(_family, _size, _bold, _italic, _underline, _color, _selection_color));
    //return the present format
    for (auto& f : string_formats)
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
    for (auto& f : string_formats)
    {
        if (*f == source)
            return f;
    }

    //or create a new one
    StringFormatPtr f(new StringFormat(source.family, source.size, source.bold, source.italic, source.underline, Color::Black(), Color::White()));
    string_formats.push_back(f);
    return f;
}

StringFormatPtr StringFormats::GetFormat(const uint _id)
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
    string_format(_string_format)
{
}

//ParagraphFormats

ParagraphFormats::ParagraphFormats(StringFormatsPtr _string_formats) :
    string_formats(_string_formats)
{
    //those are predefined paragraph styles
    GetFormat("Text body", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Arial", 22, false, false, false));
    GetFormat("Header 1", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Arial", 34, true, false, false));
    GetFormat("Header 2", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Arial", 30, true, false, false));
    GetFormat("Header 3", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10,
        string_formats->GetFormat("Arial", 26, true, false, false));
    GetFormat("Monospace", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats->GetFormat("Courier New", 12, false, false, false));
    GetFormat("Code", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::None, 2, 2, 2, 0, 2, 2, 
        string_formats->GetFormat("Courier New", 14, false, false, false));
}

ParagraphFormatPtr ParagraphFormats::GetFormat(std::string _name, ParagraphFormat::Alignment _alignment, ParagraphFormat::WordWrap _word_wrap, 
    uint _line_spacing, uint _indent_before, uint _indent_after, uint _indent_first_line, uint _spacing_before, uint _spacing_after, 
    StringFormatPtr _string_format)
{
    //return the present format
    for (auto& p : paragraph_formats)
    {
        if (p->name == _name && p->alignment == _alignment && p->word_wrap == _word_wrap && p->line_spacing == _line_spacing && 
            p->indent_before == _indent_before, p->indent_after == _indent_after && p->indent_first_line == _indent_first_line && 
            p->spacing_before == _spacing_before && p->spacing_after == _spacing_after && *p->string_format == *_string_format)
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

//FormulaFormat

FormulaFormat::FormulaFormat(const std::string& _name, StringFormatPtr _string_format, uint _inter_spacing, 
    int _left_margin, int _top_margin, int _right_margin, int _bottom_margin, Color _color, Color _selection_color) : 
    name(_name),
    string_format(_string_format),
    inter_spacing(_inter_spacing),
    left_margin(_left_margin),
    top_margin(_top_margin),
    right_margin(_right_margin),
    bottom_margin(_bottom_margin),
    color(_color),
    selection_color(_selection_color)
{
}

bool FormulaFormat::operator==(const FormulaFormat& f)
{
    return name == f.name && *string_format == *f.string_format && inter_spacing == f.inter_spacing &&
        left_margin == f.left_margin &&  top_margin == f.top_margin && right_margin == f.right_margin && bottom_margin == f.bottom_margin &&
        color == f.color && selection_color == f.selection_color;
}

//FormulaFormats

FormulaFormats::FormulaFormats(StringFormatsPtr _string_formats) :
    string_formats(_string_formats)
{
    GetFormat("Code", string_formats->GetFormat("Courier New", 14, false, false, false), 2, 5, 2, 5, 2, Color::Black(), Color::White());
    GetFormat("Formula", string_formats->GetFormat("Courier New", 14, false, false, false), 2, 2, 2, 2, 2, Color::Black(), Color::White());
    GetFormat("OnlyShapeFormula", string_formats->GetFormat("Courier New", 14, false, false, false), 0, 0, 0, 0, 0, Color{0xff, 0x40, 0x40, 0x40}, Color::White());
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
    int left_margin, int top_margin, int right_margin, int bottom_margin, Color color, Color selection_color)
{
    FormulaFormatPtr format(new FormulaFormat(name, string_format, inter_spacing, 
        left_margin, top_margin, right_margin, bottom_margin, color, selection_color));
    
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
    uint _left_margin, uint _top_margin, uint _right_margin, uint _bottom_margin, uint _paragraph_spacing) :
    name(_name), 
    left_indent(_left_indent),
    top_indent(_top_indent),
    right_indent(_right_indent),
    bottom_indent(_bottom_indent),
    left_margin(_left_margin),
    top_margin(_top_margin),
    right_margin(_right_margin),
    bottom_margin(_bottom_margin), 
    paragraph_spacing(_paragraph_spacing)
{
}

bool CodeFormat::operator==(const CodeFormat& c)
{
    return name == c.name && left_indent == c.left_indent && top_indent == c.top_indent && bottom_indent == c.bottom_indent && 
        paragraph_spacing == c.paragraph_spacing;
}

//CodeFormats

CodeFormats::CodeFormats()
{
    GetFormat("Calculator", 2, 2, 2, 2, 2, 2, 2, 2, 2);
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
    uint left_margin, uint top_margin, uint right_margin, uint bottom_margin, uint paragraph_spacing)
{
    CodeFormatPtr format(new CodeFormat(name, left_indent, top_indent, right_indent, bottom_indent, 
        left_margin, top_margin, right_margin, bottom_margin, paragraph_spacing));
    
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

namespace boost
{
namespace serialization
{

template <>
void load_construct_data(boost::archive::binary_iarchive& ar, yutovo::StringFormat* t, const unsigned int version)
{
    uint id;
    std::string family;
    uint size;
    bool bold;
    bool italic;
    bool underline;
    ar >> id;
    ar >> family;
    ar >> size;
    ar >> bold;
    ar >> italic;
    ar >> underline;
    uint32_t c;
    ar >> c;
    yutovo::Color color = yutovo::Color::FromInt(c);
    ar >> c;
    yutovo::Color selection_color = yutovo::Color::FromInt(c);
    ::new(t)yutovo::StringFormat(id, family, size, bold, italic, underline, color, selection_color);
}

template <>
void load_construct_data(boost::archive::binary_iarchive& ar, yutovo::ParagraphFormat* t, const unsigned int version)
{
    std::string name;
    yutovo::ParagraphFormat::Alignment alignment;
    yutovo::ParagraphFormat::WordWrap word_wrap;
    uint line_spacing;
    uint indent_before;
    uint indent_after;
    uint indent_first_line;
    uint spacing_before;
    uint spacing_after;
    ar >> name;
    ar >> alignment;
    ar >> word_wrap;
    ar >> line_spacing;
    ar >> indent_before;
    ar >> indent_after;
    ar >> indent_first_line;
    ar >> spacing_before;
    ar >> spacing_after;
    uint string_format_id;
    ar >> string_format_id;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    auto string_format = user_data.document->string_formats->GetFormat(string_format_id);

    ::new(t)yutovo::ParagraphFormat(name, alignment, word_wrap, line_spacing, indent_before, indent_after, indent_first_line, spacing_before, 
        spacing_after, string_format);
}

template <>
void load_construct_data(boost::archive::binary_iarchive& ar, yutovo::ParagraphFormats* t, const unsigned int version)
{
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::ParagraphFormats(user_data.document->string_formats);
}

}
}
