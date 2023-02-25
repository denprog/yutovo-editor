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
}

OpenFence::OpenFence(Document* _document) :
    OnlyShapeFormula(_document, '(')
{
    type = ElementType::OPEN_FENCE;
    remake_always = true;
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
            path.push_back(Point{r.GetRight(), (int)lround(r.top + r.height * 0.977)});
            path.push_back(Point{r.GetRight(), r.GetBottom()});

            window->DrawBezierPath(path, document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    Formula::Draw();
}

void OpenFence::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    OnlyShapeFormula::Remake(with_elements, with_parent, with_undo);

    if (parent->elements->IsLast(id))
    {
        Size s = window->GetTextSize(" ", GetStringFormat());
        shape->rect.SetRect(0, 0, 1 + (int)lround(s.height / 5), (int)lround((1 + 2 * BRACES_Y_OFFSET) * s.height));
        rect = shape->rect;
        return;
    }
    
    //set rect of the shape by the next elements until close fence
    int pos = parent->elements->GetElementPos(id);
    int close_pos = parent->FindElement(id, true, ElementType::CLOSE_FENCE);
    if (close_pos == pos + 1)
        return;
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
        if (with_parent)
            document->Remake(parent->id, false, with_undo, false);
        document->Redraw(id, false);
    }
    last_rect = rect;
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
}

CloseFence::CloseFence(Document* _document) :
    OnlyShapeFormula(_document, ')')
{
    type = ElementType::CLOSE_FENCE;
    remake_always = true;
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

            window->DrawBezierPath(path, document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color);
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        parent->window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    Formula::Draw();
}

void CloseFence::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    OnlyShapeFormula::Remake(with_elements, with_parent, with_undo);

    if (parent->elements->IsFirst(id))
    {
        Size s = window->GetTextSize(" ", GetStringFormat());
        shape->rect.SetRect(0, 0, 1 + (int)lround(s.height / 5), (int)lround((1 + 2 * BRACES_Y_OFFSET) * s.height));
        rect = shape->rect;
        return;
    }
    
    //set rect of the shape by the previous elements until open fence
    int pos = parent->elements->GetElementPos(id);
    int open_pos = parent->FindElement(id, false, ElementType::OPEN_FENCE);
    if (open_pos == pos - 1)
        return;
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
        if (with_parent)
            document->Remake(parent->id, false, with_undo, false);
        document->Redraw(id, false);
    }
    last_rect = rect;
}

std::string CloseFence::ToHtml()
{
    return "<mo>)</mo>";
}

}
