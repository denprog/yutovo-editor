#include "square_root.h"
#include "code_row.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

SquareRoot::SquareRoot(Element* _parent) :
    Formula(_parent)
{
    Init();
}

SquareRoot::SquareRoot(Document* _document) :
    Formula(_document)
{
    Init();
}

SquareRoot::SquareRoot(const SquareRoot& source) :
    Formula(source),
    shape((Shape*)elements->Get(0).get()),
    last(elements->Get(1).get())
{
}

void SquareRoot::Init()
{
    type = ElementType::SQUARE_ROOT;
    shape = new Shape(this);
    last = new CodeRow(this);
    elements->Add(ElementPtr(shape));
    elements->Add(ElementPtr(last));
}

Element* SquareRoot::Clone()
{
    return new SquareRoot(*this);
}

Element* SquareRoot::Create(Element* _parent)
{
    return new SquareRoot(_parent);
}

void SquareRoot::Draw() const
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
            window->DrawFillPath(path, document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    Formula::Draw();
}

void SquareRoot::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    if (with_elements)
        elements->Remake(with_parent, with_undo);

    //recalc shape rect
    shape->rect.SetRect(0, 0, round(last->rect.height * 5 / 11), std::max(last->rect.height + ROOT_Y_OFFSET * 2, shape->rect.height));

    //recalc item's rect
    int x_offset = ROOT_X_LEFT_OFFSET + shape->rect.width;
    int y_offset = 0;
    if (shape->rect.height > last->rect.height)
        y_offset = ROOT_Y_OFFSET + round((shape->rect.height - last->rect.height) / 2);
    else
        y_offset = ROOT_Y_OFFSET + round(shape->rect.height * 0.01);
    last->rect.Move(x_offset, y_offset);
    baseline = last->rect.top + last->baseline;

    UpdateRect();

    if (rect != last_rect && with_parent)
        parent->Remake(false, true, with_undo);
    last_rect = rect;
}

bool SquareRoot::AfterInsert(bool with_undo)
{
    CaretState c;
    last->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    return true;
}

bool SquareRoot::DeleteElements(bool left, bool with_undo)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start == 1 && size == 1)
        {
            last->elements->Clear();
            Normalize(with_undo);
            return true;
        }
        return false;
    }
    else if (!selection->IsEmpty() || left || caret->GetPos() != 0)
        return false;
    
    //remove this element by deleting its shape
    int p = parent->elements->GetElementPos(id);
    uint c1 = elements->Get(1)->elements->Count();
    Element* undo_el = nullptr;
    if (with_undo)
        undo_el = Clone();

    caret->SetState(id);
    parent->elements->Move(*elements->Get(1)->elements, p);
    CaretState c;
    if (parent->elements->Get(p)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    if (with_undo)
    {
        document->InsertElement(undo_el, false, true);
        document->PushEditorState(CaretState(parent->id, p), true);
        document->DeleteElements(false, false, true);
        document->PushEditorState(CaretState(parent->id, p), SelectionState(parent->id, p, c1), true);
    }

    auto t = parent->elements->Get(p + c1); //for not removing this element until this function ends
    parent->elements->Remove(id);
    parent->Normalize(with_undo);
    parent->Remake(true, true, with_undo);
    return true;
}

std::string SquareRoot::ToHtml()
{
    std::string s = "<msqrt>";
    s += last->ToHtml();
    s += "</msqrt>";
    return s;
}

std::u32string SquareRoot::ToText()
{
    return U"sqrt(" + last->ToText() + U")";
}

void SquareRoot::ToParserString(ParserString& str)
{
    str.Add(id, U"sqrt(");
    last->ToParserString(str);
    str.Add(id, U")");
}

}
