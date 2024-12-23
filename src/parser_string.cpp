#include "parser_string.h"

namespace yutovo
{

//ParserString

ParserString& ParserString::operator=(const ParserString& str)
{
    text = str.text;
    annotates = str.annotates;
    return *this;
}

bool ParserString::operator==(const ParserString& str) const
{
    return text == str.text;
}

bool ParserString::operator!=(const ParserString& str) const
{
    return text != str.text;
}

void ParserString::Add(const ElementId id, const std::u32string& _text)
{
    if (_text.empty())
        return;
    int start = text.length();
    text += _text;
    annotates.emplace_back(StringAnnotate{id, start, (int)_text.length()});
}

void ParserString::Add(const ParserString& str)
{
    text += str.text;
    annotates.insert(annotates.end(), str.annotates.begin(), str.annotates.end());
}

void ParserString::Reset()
{
    text = U"";
    annotates.clear();
}

void ParserString::Annotate(const ElementId id, const int start, const int size)
{
    annotates.emplace_back(StringAnnotate{id, start, size});
}

std::u32string& ParserString::Text()
{
    return text;
}

int ParserString::Length()
{
    return text.length();
}

ElementId ParserString::GetElement(const int pos)
{
    if (text.empty() && pos == 0 && annotates.size() == 1 && annotates[0].pos == 0)
        return annotates[0].id;

    ElementId res;
    int size = text.length() + 1;
    //find ElementId at pos with minimal size
    for (auto it = annotates.begin(); it != annotates.end(); ++it)
    {
        auto& a = *it;
        if (pos >= a.pos && pos < a.pos + a.size)
        {
            if (size > a.size)
            {
                res = a.id;
                size = a.size;
            }
        }
    }

    return res;
}

}
