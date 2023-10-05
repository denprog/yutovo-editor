#include "fences.h"
#include "../document.h"

namespace yutovo
{

//OpenFence

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
            std::list<Point> path;
            path.push_back(Point{r.GetRight(), r.GetBottom()});
            path.push_back(Point{(int)lround(r.left + r.width * 0.590), (int)lround(r.top + r.height * 0.941)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.132), (int)lround(r.top + r.height * 0.66)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.132), (int)lround(r.top + r.height * 0.5)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.132), (int)lround(r.top + r.height * 0.333)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.613), (int)lround(r.top + r.height * 0.059)});
            path.push_back(Point{r.GetRight(), r.top});

            path.push_back(Point{r.GetRight(), r.top});
            path.push_back(Point{r.GetRight(), (int)lround(r.top + r.height * 0.023)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.807), (int)lround(r.top + r.height * 0.059)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.558), (int)lround(r.top + r.height * 0.190)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.436), (int)lround(r.top + r.height * 0.380)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.436), (int)lround(r.top + r.height * 0.486)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.436), (int)lround(r.top + r.height * 0.606)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.549), (int)lround(r.top + r.height * 0.726)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.781), (int)lround(r.top + r.height * 0.926)});
            path.push_back(Point{r.GetRight(), (int)lround(r.top + r.height * 0.977)});
            path.push_back(Point{r.GetRight(), r.GetBottom()});

            window->DrawBezierPath(path, document->selection.IsSelected(id) ? formula_format->bg_color : formula_format->color);
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
            shape->rect.SetRect(0, 0, 1 + (int)lround(s.height / 5), (int)lround((1 + 2 * BRACES_Y_OFFSET) * s.height));
            rect = shape->rect;
            baseline = shape->rect.height / 2;
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
    shape->rect.SetRect(0, 0, 1 + (int)lround(max_height / 5), (int)lround((1 + 2 * BRACES_Y_OFFSET) * max_height));
    rect = shape->rect;

    baseline = shape->rect.height / 2;

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
            std::list<Point> path;
            path.push_back(Point{r.left, r.GetBottom()});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.590), (int)lround(r.top + r.height * 0.941)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.132), (int)lround(r.top + r.height * 0.66)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.132), (int)lround(r.top + r.height * 0.5)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.132), (int)lround(r.top + r.height * 0.333)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.613), (int)lround(r.top + r.height * 0.059)});
            path.push_back(Point{r.left, r.top});

            path.push_back(Point{r.left, (int)lround(r.top + r.height * 0.023)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.807), (int)lround(r.top + r.height * 0.059)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.558), (int)lround(r.top + r.height * 0.190)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.436), (int)lround(r.top + r.height * 0.380)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.436), (int)lround(r.top + r.height * 0.486)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.436), (int)lround(r.top + r.height * 0.606)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.549), (int)lround(r.top + r.height * 0.726)});
            path.push_back(Point{(int)lround(r.GetRight() - r.width * 0.781), (int)lround(r.top + r.height * 0.926)});
            path.push_back(Point{r.left, (int)lround(r.top + r.height * 0.977)});
            path.push_back(Point{r.left, r.GetBottom()});

            window->DrawBezierPath(path, document->selection.IsSelected(id) ? formula_format->bg_color : formula_format->color);
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
            shape->rect.SetRect(0, 0, 1 + (int)lround(s.height / 5), (int)lround((1 + 2 * BRACES_Y_OFFSET) * s.height));
            rect = shape->rect;
            baseline = shape->rect.height / 2;
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
    shape->rect.SetRect(0, 0, 1 + (int)lround(max_height / 5), (int)lround((1 + 2 * BRACES_Y_OFFSET) * max_height));
    rect = shape->rect;
    
    baseline = shape->rect.height / 2;

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
