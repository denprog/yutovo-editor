#include "nth_root.h"
#include "code_row.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

NthRoot::NthRoot(Element* _parent, bool with_init) :
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::NTH_ROOT;
    UpdateLevel(level);
}

NthRoot::NthRoot(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::NTH_ROOT;
    UpdateLevel(level);
}

NthRoot::NthRoot(const NthRoot& source) :
    MiddleShapeFormula(source)
{
}

Element* NthRoot::Clone()
{
    return new NthRoot(*this);
}

Element* NthRoot::Create(Element* _parent)
{
    return new NthRoot(_parent);
}

Element* NthRoot::FromJson(Element* parent, Document* document, const rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new NthRoot(parent, false);
    return new NthRoot(document, false);
}

void NthRoot::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            std::list<Point> path;
            path.push_back(Point{r.GetRight(), (int)lround(r.top + (float)r.height * 0.01)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.722), r.GetBottom()});
            path.push_back(Point{(int)lround(r.left + r.width * 0.281), (int)lround(r.top + r.height * 0.495)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.061), (int)lround(r.top + r.height * 0.544)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.075), (int)lround(r.top + r.height * 0.510)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.343), (int)lround(r.top + r.height * 0.429)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.703), (int)lround(r.top + r.height * 0.869)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.934), r.top});
            path.push_back(Point{r.GetRight() + last->rect.width, r.top});
            path.push_back(Point{r.GetRight() + last->rect.width, (int)lround(r.top + r.height * 0.001)});
            path.push_back(Point{r.GetRight(), (int)lround(r.top + r.height * 0.001)});
            window->DrawFillPath(path, document->selection.IsSelected(id) ? formula_format->bg_color : formula_format->color);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, formula_format->bg_selection_color);
    }

    Formula::Draw();
}

bool NthRoot::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);

    //recalc shape rect
    int left_up = first->baseline;
    int left_down = first->rect.height - left_up;
    int bottom = 0;
    if (left_up > left_down)
        bottom = left_up * 2 + ROOT_Y_OFFSET * 2;
    else
        bottom = left_down * 2 + ROOT_Y_OFFSET * 2;
    shape->rect.SetRect(0, 0, round(last->rect.height * 5 / 11), std::max(last->rect.height + ROOT_Y_OFFSET * 2, bottom));

    //recalc items' rects
    int x_offset = ROOT_X_LEFT_OFFSET;
    first->rect.Move(x_offset, (shape->rect.height - first->rect.height) / 2);
    x_offset += first->rect.width;
    shape->rect.Move(x_offset, 0);
    x_offset += shape->rect.width + ROOT_X_RIGHT_OFFSET;
    int y_offset = 0;
    if (shape->rect.height > last->rect.height)
        y_offset = ROOT_Y_OFFSET + round((shape->rect.height - last->rect.height) / 2);
    else
        y_offset = ROOT_Y_OFFSET + shape->rect.height * 0.01;
    last->rect.Move(x_offset, y_offset);
    baseline = last->rect.top + last->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void NthRoot::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (first)
        first->UpdateLevel(_level + 1);
}

std::string NthRoot::ToHtml()
{
    std::string s = "<mroot>";
    s += last->ToHtml();
    s += first->ToHtml();
    s += "</mroot>";
    return s;
}

std::u32string NthRoot::ToText()
{
    if (!first || !last)
        return U"";
    return U"root(" + last->ToText() + U"," + first->ToText() + U")";
}

void NthRoot::ToParserString(ParserString& str)
{
    str.Add(id, U"root(");
    last->ToParserString(str);
    str.Add(id, U",");
    first->ToParserString(str);
    str.Add(id, U")");
}

}
