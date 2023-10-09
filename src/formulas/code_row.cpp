#include "code_row.h"
#include "code_string.h"

namespace yutovo
{

CodeRow::CodeRow(Document* _document) :
    Row(_document)
{
    type = ElementType::CODE_ROW;
}

CodeRow::CodeRow(Element* parent, bool with_string) :
    Row(parent, false)
{
    type = ElementType::CODE_ROW;
    if (with_string)
        AddEmptyElement();
}

Element* CodeRow::Clone()
{
    return new CodeRow(*this);
}

Element* CodeRow::Create(Element* parent)
{
    return new CodeRow(parent);
}

Element* CodeRow::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new CodeRow(parent, false);
    return new CodeRow(document);
}

void CodeRow::Normalize()
{
    Row::Normalize();

    for (size_t i = 0; i < elements->Count();)
    {
        auto el = (*elements)[i];
        if (document->IsString(el) && el->editable)
        {
            std::u32string str = el->ToText();
            if (!str.empty())
            {
                //remove lead whitespaces
                size_t pos = str.find_first_not_of(U' ');
                if (pos == std::string::npos)
                {
                    el->elements->RemoveAt(0, str.length());
                    str = ToText();
                }
                else if (pos != 0)
                {
                    el->elements->RemoveAt(0, pos);
                    str = ToText();
                    ElementPtr _el(new CodeString(this, U"", ((CodeString*)el.get())->format));
                    int p = elements->GetElementPos(el->id);
                    elements->Insert(_el, p);
                    _el->can_merge = false;
                }

                for (size_t i = 1; i < str.length();)
                {
                    int j = i;
                    int s = 0;
                    while (j < str.length() && str[j++] == U' ')
                        ++s;
                    if (s > 0)
                    {
                        //split this element
                        if (el->SplitAt(i))
                        {
                            int p = elements->GetElementPos(el->id);
                            auto n = elements->Get(p + 1);
                            n->can_merge = false;
                            n->elements->RemoveAt(0, s);
                            el = n;
                            str = el->ToText();
                            i = 1;
                            continue;
                        }
                    }
                    ++i;
                }
            }
        }
        ++i;
    }
}

bool CodeRow::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    for (auto el : _elements)
    {
        if (el->type == ElementType::CODE_BLOCK)
        {
            return false;
        }
        if (el->type == ElementType::CODE_PARAGRAPH && 
            (parent->type != ElementType::CODE_PARAGRAPH && parent->type != ElementType::ASSIGNMENT && parent->type != ElementType::EQUATION))
        {
            return false;
        }
    }
    return Row::InsertElements(_elements, with_undo, changed_element);
}

bool CodeRow::GetBeginCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (Row::GetBeginCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetBeginCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

bool CodeRow::GetEndCaretState(CaretState& caret_state, Selection* select)
{
    CaretState c;
    if (Row::GetEndCaretState(c, select))
    {
        if (c == caret_state)
            return parent->GetEndCaretState(caret_state, select);
        caret_state = c;
        return true;
    }
    return false;
}

void CodeRow::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeString(this)));
}

bool CodeRow::IsFormula()
{
    return true;
}

std::string CodeRow::ToHtml()
{
    std::string s = "<mrow>";
    s += elements->ToHtml();
    s += "</mrow>";
    return s;
}

}
