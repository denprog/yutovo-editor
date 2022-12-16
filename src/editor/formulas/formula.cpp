#include "formula.h"
#include "../document.h"

namespace yutovo
{

Formula::Formula(Element* _parent) : 
    Element(_parent)
{
    string_format = document->GetStringFormat("Courier New", 14, false, false, false);
}

Formula::Formula(Document* _document) :
    Element(_document)
{
    string_format = document->GetStringFormat("Courier New", 14, false, false, false);
}

bool Formula::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    return false;
}

bool Formula::DeleteElements(bool left, bool with_undo)
{
    return false;
}

bool Formula::ChangeStringFormat(const StringFormatPtr format, bool with_undo)
{
    return false;
}

bool Formula::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo)
{
    return false;
}

bool Formula::Split(const uint max_left_width)
{
    return false;
}

bool Formula::SplitAt(const uint pos)
{
    return false;
}

bool Formula::Merge(const ElementPtr with_element)
{
    return false;
}

bool Formula::HasCaretState()
{
    return true;
}

bool Formula::HasLastCaretState()
{
    return true;
}

StringFormatPtr Formula::GetStringFormat()
{
    return string_format;
}

}
