#include "fences.h"
#include "../document.h"

namespace yutovo
{

//OpenFence

const std::string OpenFence::family_name = "Arial";

OpenFence::OpenFence(Element* _parent) : 
    OnlyShapeFormula(_parent, '(')
{
    type = ElementType::OPEN_FENCE;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

OpenFence::OpenFence(Document* _document) :
    OnlyShapeFormula(_document, '(')
{
    type = ElementType::OPEN_FENCE;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

OpenFence::OpenFence(const OpenFence& source) :
    OnlyShapeFormula(source)
{
    remake_always = true;
}

Element* OpenFence::Clone()
{
    return new OpenFence(*this);
}

Element* OpenFence::Create(Element* _parent)
{
    return new OpenFence(_parent);
}

Element* OpenFence::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    OpenFence* p = nullptr;
    if (parent)
        p = new OpenFence(parent);
    else
        p = new OpenFence(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

void OpenFence::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText("(", format, r, document->selection.IsSelected(id) ? formula_format->bg_color : formula_format->color, 
                    document->selection.IsSelected(id) ? formula_format->bg_selection_color : formula_format->bg_color);
            }
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool OpenFence::Remake(bool with_elements)
{
    bool changed = OnlyShapeFormula::Remake(with_elements);

    auto empty_brace = 
        [&]()
        {
            Size s = window->GetTextSize(U" ", GetStringFormat());
            int size = window->GetSymbolSize(U'(', (int)lround(s.height * 1.2), family_name, s, baseline);
            rect.SetRect(0, 0, s.width, s.height);
            shape->rect = rect;
            format = document->string_formats->GetFormat(family_name, size, false, false, false, Color::Black(), Color::White(), Color::Blue());
        };

    if (parent->elements->IsLast(id) || (parent->elements->Count() == 2 && parent->elements->Get(1)->type == ElementType::CLOSE_FENCE))
    {
        empty_brace();
        return changed;
    }
    
    //set rect of the shape by the next elements until close fence
    int pos = parent->elements->GetElementPos(id);
    int close_pos = parent->FindElement(id, true, ElementType::CLOSE_FENCE);
    if (close_pos == pos + 1)
    {
        empty_brace();
        return true;
    }
    
    int max_height = 0;
    for (int i = pos + 1; i < (close_pos == -1 ? parent->elements->Count() : close_pos); ++i)
    {
        auto el = parent->elements->Get(i);
        if (el->rect.height > max_height)
            max_height = el->rect.height;
    }

    Size s;
    int size = window->GetSymbolSize(U'(', (int)lround(max_height * 1.2), family_name, s, baseline);
    if (size != 0)
    {
        rect.SetRect(0, 0, s.width, s.height);
        shape->rect = rect;
        format = document->string_formats->GetFormat(family_name, size, false, false, false, Color::Black(), Color::White(), Color::Blue());
    }

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

std::string OpenFence::ToHtml()
{
    return "<mo>(</mo>";
}

//CloseFence

const std::string CloseFence::family_name = "Arial";

CloseFence::CloseFence(Element* _parent) : 
    OnlyShapeFormula(_parent, ')')
{
    type = ElementType::CLOSE_FENCE;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

CloseFence::CloseFence(Document* _document) :
    OnlyShapeFormula(_document, ')')
{
    type = ElementType::CLOSE_FENCE;
    remake_always = true;
    formula_format = document->formula_formats->GetFormat("Formula");
}

CloseFence::CloseFence(const CloseFence& source) :
    OnlyShapeFormula(source)
{
    remake_always = true;
}

Element* CloseFence::Clone()
{
    return new CloseFence(*this);
}

Element* CloseFence::Create(Element* _parent)
{
    return new CloseFence(_parent);
}

Element* CloseFence::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    CloseFence* p = nullptr;
    if (parent)
        p = new CloseFence(parent);
    else
        p = new CloseFence(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

void CloseFence::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            if (format)
            {
                window->DrawText(")", format, r, document->selection.IsSelected(id) ? formula_format->bg_color : formula_format->color, 
                    document->selection.IsSelected(id) ? formula_format->bg_selection_color : formula_format->bg_color);
            }
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool CloseFence::Remake(bool with_elements)
{
    bool changed = OnlyShapeFormula::Remake(with_elements);

    auto empty_brace = 
        [&]()
        {
            Size s = window->GetTextSize(U" ", GetStringFormat());
            int size = window->GetSymbolSize(U')', (int)lround(s.height * 1.2), family_name, s, baseline);
            rect.SetRect(0, 0, s.width, s.height);
            shape->rect = rect;
            format = document->string_formats->GetFormat(family_name, size, false, false, false, Color::Black(), Color::White(), Color::Blue());
        };

    if (parent->elements->IsFirst(id) || (parent->elements->Count() == 2 && parent->elements->Get(0)->type == ElementType::OPEN_FENCE))
    {
        empty_brace();
        return changed;
    }
    
    //set rect of the shape by the previous elements until open fence
    int pos = parent->elements->GetElementPos(id);
    int open_pos = parent->FindElement(id, false, ElementType::OPEN_FENCE);
    if (open_pos == pos - 1)
    {
        empty_brace();
        return true;
    }
    
    int max_height = 0;
    for (int i = (open_pos == -1 ? 0 : open_pos + 1); i < pos; ++i)
    {
        auto el = parent->elements->Get(i);
        if (el->rect.height > max_height)
            max_height = el->rect.height;
    }

    Size s;
    int size = window->GetSymbolSize(U')', (int)lround(max_height * 1.2), family_name, s, baseline);
    if (size != 0)
    {
        rect.SetRect(0, 0, s.width, s.height);
        shape->rect = rect;
        format = document->string_formats->GetFormat(family_name, size, false, false, false, Color::Black(), Color::White(), Color::Blue());
    }

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

std::string CloseFence::ToHtml()
{
    return "<mo>)</mo>";
}

}
