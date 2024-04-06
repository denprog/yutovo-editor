#include "code_block.h"
#include "code_paragraph.h"
#include "formula.h"

namespace yutovo
{

//CodeBlock

CodeBlock::CodeBlock(Document* _document, uint _code_id, bool add_empty) :
    Block(_document),
    code_id(_code_id)
{
    type = ElementType::CODE_BLOCK;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //code block has to have at least one code paragraph
    
    document->SetLocale(document->config.language);
    document->ListIdentifiers(code_id);
}

CodeBlock::CodeBlock(Element* parent, uint _code_id, bool add_empty) :
    Block(parent),
    code_id(_code_id)
{
    type = ElementType::CODE_BLOCK;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //code block has to have at least one code paragraph

    document->SetLocale(document->config.language);   
    document->ListIdentifiers(code_id);
}

CodeBlock::CodeBlock(Document* _document, Element* parent, uint _code_id, bool add_empty) :
    Block(parent),
    code_id(_code_id)
{
    type = ElementType::CODE_BLOCK;
    document = _document;
    window = document->window;
    caret = document->caret;
    selection = &document->selection;
    code_format = document->code_formats->GetFormat("Calculator");
    paragraph_format = document->paragraph_formats->GetFormat("Code");
    formula_format = document->formula_formats->GetFormat("Formula");
    if (add_empty)
        AddEmptyElement(); //code block has to have at least one code paragraph
    
    document->SetLocale(document->config.language);
    document->ListIdentifiers(code_id);
}

Element* CodeBlock::Clone()
{
    return new CodeBlock(*this);
}

Element* CodeBlock::Create(Element* parent)
{
    return new CodeBlock(parent, parent->document->cur_code_id);
}

void CodeBlock::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    Block::ToJson(value, alloc);
    value.AddMember("code_id", code_id, alloc);
}

Element* CodeBlock::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (!value.HasMember("code_id") || !value["code_id"].IsInt())
        return nullptr;
    auto code_id = value["code_id"].GetInt();
    if (parent)
        return new CodeBlock(parent, code_id, false);
    return new CodeBlock(document, code_id, false);
}

void CodeBlock::Draw() const
{
    Element::Draw();
    if (document->config.formula_border)
        window->DrawRect(GetAbsoluteRect(), code_format->border_color);
}

bool CodeBlock::Remake(bool with_elements)
{
    bool changed = Element::Remake(with_elements);

    int left_m = 0, top_m = 0, right_m = 0, bottom_m = 0;
    int h = code_format->top_indent;
    for (int i = 0; i < elements->Count(); ++i) //arrange paragraphs
    {
        ElementPtr p = elements->Get(i);
        p->GetMargin(left_m, top_m, right_m, bottom_m); //consider the margins
        p->rect.Move(left_m + code_format->left_indent, h + top_m);
        h = p->rect.GetBottom() + bottom_m;
    }

    Element::UpdateRect(false);

    UpdateRect();
    rect.width += code_format->right_indent;
    rect.height += code_format->bottom_indent;

    //align the baseline
    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

bool CodeBlock::AfterInsert(bool with_undo)
{
    CaretState c;
    if (GetFirstCaretState(c, nullptr))
        caret->SetState(c);
    return true;
}

void CodeBlock::GetMargin(int& left, int& top, int& right, int& bottom) const
{
    left = code_format->left_margin;
    top = code_format->top_margin;
    right = code_format->right_margin;
    bottom = code_format->bottom_margin;
}

bool CodeBlock::HasCaretState()
{
    return true;
}

bool CodeBlock::HasLastCaretState()
{
    return true;
}

bool CodeBlock::CanContinueVerticalMoving()
{
    return true;
}

StringFormatPtr CodeBlock::GetStringFormat() const
{
    return formula_format->string_format;
}

FormulaFormatPtr CodeBlock::GetFormulaFormat() const
{
    return formula_format;
}

ParagraphFormatPtr CodeBlock::GetParagraphFormat()
{
    return paragraph_format;
}

void CodeBlock::AddEmptyElement()
{
    AddElement(ElementPtr(new CodeParagraph(this)));
}

bool CodeBlock::IsFormula()
{
    return true;
}

}
