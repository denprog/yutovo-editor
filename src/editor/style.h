#ifndef __STYLE_H__
#define __STYLE_H__

#include <string>
#include <memory>
#include <vector>

namespace yutovo
{

struct StringFormat
{
    StringFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline);
 
    bool operator==(const StringFormat& f);

    std::string family;
    uint size;
    bool bold;
    bool italic;
    bool underline;
};

typedef std::shared_ptr<StringFormat> StringFormatPtr;

//Collection of string formats
class StringFormats
{
public:
    StringFormatPtr GetFormat(const std::string _family, uint _size, bool _bold, bool _italic, bool _underline);

private:
    std::vector<StringFormatPtr> string_formats;
};

struct ParagraphFormat
{
    enum class Alignment
    {
        Left = 0,
        Right,
        Center,
        Justified
    };

    enum class WordWrap
    {
        None = 0,
        Normal
    };

    ParagraphFormat(std::string _name, Alignment _alignment, WordWrap _word_wrap, uint _line_spacing, uint _indent_before, uint _indent_after, 
        uint _indent_first_line, uint _spacing_before, uint _spacing_after, StringFormatPtr _string_format);

    std::string name;
    Alignment alignment = Alignment::Left;
    WordWrap word_wrap = WordWrap::Normal;
    uint line_spacing;
    uint indent_before;
    uint indent_after;
    uint indent_first_line;
    uint spacing_before;
    uint spacing_after;

    StringFormatPtr string_format;
};

typedef std::shared_ptr<ParagraphFormat> ParagraphFormatPtr;

//Collection of paragraph formats
class ParagraphFormats
{
public:
    ParagraphFormats(StringFormats& _string_formats);

    ParagraphFormatPtr GetFormat(std::string _name, ParagraphFormat::Alignment _alignment, ParagraphFormat::WordWrap _word_wrap, uint _line_spacing, 
        uint _indent_before, uint _indent_after, uint _indent_first_line, uint _spacing_before, uint _spacing_after, StringFormatPtr _string_format);
    ParagraphFormatPtr GetFormat(const std::string& name);
    void GetFormats(std::vector<ParagraphFormatPtr>& formats);

private:
    StringFormats& string_formats;
    std::vector<ParagraphFormatPtr> paragraph_formats;
};

struct PageFormat
{
    uint left_indent;
    uint top_indent;
    uint right_indent;
    uint bottom_indent;
    uint paragraph_spacing;
};

typedef std::shared_ptr<PageFormat> PageFormatPtr;

class PageFormats
{
public:
    static PageFormatPtr GetFormat(uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, uint paragraph_spacing);

private:
    static std::vector<PageFormatPtr> page_formats;
};

struct TextFormat
{
    enum class Paging
    {
        ONE_PAGE = 0 //there is only one variant for now
    };

    Paging paging = Paging::ONE_PAGE;
};

typedef std::shared_ptr<TextFormat> TextFormatPtr;

class TextFormats
{
public:
    static TextFormatPtr GetFormat(TextFormat::Paging paging);

private:
    static std::vector<TextFormatPtr> text_formats;
};

}

#endif
