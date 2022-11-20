#include "style.h"
#include <algorithm>

namespace yutovo
{

//StringFormat

StringFormat::StringFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline) :
    family(_family), 
    size(_size),
    bold(_bold),
    italic(_italic),
    underline(_underline)
{
}

bool StringFormat::operator==(const StringFormat& f)
{
    return family == f.family && size == f.size && bold == f.bold && italic == f.italic && underline == f.underline;
}

//StringFormats

StringFormatPtr StringFormats::GetFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline)
{
    //return the present format
    for (auto& f : string_formats)
    {
        if (f->family == _family && f->size == _size && f->bold == _bold && f->italic == _italic && f->underline == _underline)
            return f;
    }

    //or create a new one
    StringFormatPtr f(new StringFormat(_family, _size, _bold, _italic, _underline));
    string_formats.push_back(f);
    return f;
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

ParagraphFormats::ParagraphFormats(StringFormats& _string_formats) :
    string_formats(_string_formats)
{
    //those are predefined paragraph styles
    GetFormat("Text body", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats.GetFormat("Arial", 22, false, false, false));
    GetFormat("Header 1", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats.GetFormat("Arial", 34, true, false, false));
    GetFormat("Header 2", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats.GetFormat("Arial", 30, true, false, false));
    GetFormat("Header 3", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10,
        string_formats.GetFormat("Arial", 26, true, false, false));
    GetFormat("Monospace", ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10, 
        string_formats.GetFormat("Courier New", 12, false, false, false));
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
