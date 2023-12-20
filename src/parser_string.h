#ifndef __PARSER_STRING_H__
#define __PARSER_STRING_H__

#include "caret_state.h"
#include <vector>
#include <string>

namespace yutovo
{

class ParserString
{
public:
    ParserString& operator=(const ParserString& str);

    bool operator==(const ParserString& str) const;
    bool operator!=(const ParserString& str) const;

    void Add(const ElementId id, const std::u32string& _text);
    void Add(const ParserString& str);
    void Reset();

    void Annotate(const ElementId id, const int start, const int size);

    std::u32string& Text();

    int Length();

    ElementId GetElement(const int pos);

private:
    std::u32string text;

    struct StringAnnotate
    {
        ElementId id;
        int pos = 0;
        int size = 0;
    };

    std::vector<StringAnnotate> annotates;
};

}

#endif
