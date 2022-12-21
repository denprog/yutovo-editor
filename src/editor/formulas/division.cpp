#include "division.h"
#include "code_row.h"
#include "shape.h"

namespace yutovo
{

//Division

Division::Division(Element* _parent) : 
    Formula(_parent)
{
    type = ElementType::DIVISION;
    Init();
}

Division::Division(Document* _document) :
    Formula(_document)
{
    type = ElementType::DIVISION;
    Init();
}

Division::Division(const Division& source) :
    Formula(source),
    upper(elements->Get(0).get()),
    shape((Shape*)elements->Get(1).get()),
    lower(elements->Get(2).get())
{
}

void Division::Init()
{
    upper = new CodeRow(this);
    elements->Add(ElementPtr(upper));
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
    lower = new CodeRow(this);
    elements->Add(ElementPtr(lower));
}

Element* Division::Clone()
{
    return new Division(*this);
}

Element* Division::Create(Element* parent)
{
    return new Division(parent);
}

void Division::Draw() const
{
    shape->draw_func = 
        [p = parent](const Rect& rect)
        {
            if (rect.height == 0)
                p->window->DrawLine(rect.left, rect.top, rect.left + rect.width, rect.top, Color::Black());
            else
                p->window->DrawFillRect(rect.left, rect.top, rect.width, rect.height, Color::Black());
        };
    Formula::Draw();
}

void Division::Remake(bool with_elements)
{
    if (with_elements)
        elements->Remake();
    
    int w = std::max(upper->rect.width + 2, lower->rect.width + 2);
    if (w < 200)
        shape->rect.SetRect(0, 0, w, 0);
    else
        shape->rect.SetRect(0, 0, w, w / 200 > 2 ? 2 : w / 200);
    upper->rect.Move((w - upper->rect.width) / 2, 0);
    shape->rect.Move(0, upper->rect.height + shape->rect.height + 4);
    lower->rect.Move((w - lower->rect.width) / 2, upper->rect.height + shape->rect.height + (shape->rect.height + 4) * 2);
    baseline = upper->rect.height + 4;

    UpdateRect();

    document->Remake(parent->id, false);
}

bool Division::AfterInsert(bool with_undo)
{
    if (upper->elements->Count() == 1)
    {
        String* str = dynamic_cast<String*>(upper->elements->Get(0).get());
        if (str && str->elements->Count() == 0)
        {
            CaretState c;
            if (caret->current_element)
            {
                c = caret->GetCaretState();
                //move the current element in the upper one
                if (c.id != id && c.IsInsideElement(parent->id) && c.GetPosInElement(parent->id) < parent->elements->Count())
                {
                    if (with_undo)
                    {
                        document->InsertFormula(caret->current_element->Clone(), false, true);
                        document->PushEditorState(CaretState(caret->current_element->id), true);
                    }
                    upper->elements->RemoveAt(0, 1);
                    upper->elements->Move(document->GetElement(caret->current_element->id), 0);
                }
            }
            if (upper->elements->Get(0)->elements->Count() == 0)
            {
                if (upper->GetFirstCaretState(c, nullptr))
                    caret->SetState(c);
            }
            else
            {
                if (lower->GetFirstCaretState(c, nullptr))
                    caret->SetState(c);
            }
            return true;
        }
    }
    return false;
}

bool Division::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (lower->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(lower->id))
        return lower->GetTopCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(shape->id))
        return shape->GetTopCaretState(x, y, caret_state, select);
    if (upper->GetAbsoluteRect().GetBottom() <= y && !caret->IsOnElement(upper->id))
        return upper->GetTopCaretState(x, y, caret_state, select);
    return parent->GetTopCaretState(x, y, caret_state, select);
}

bool Division::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    if (upper->GetAbsoluteRect().top >= y)
        return upper->GetBottomCaretState(x, y, caret_state, select);
    if (shape->GetAbsoluteRect().top >= y)
        return shape->GetBottomCaretState(x, y, caret_state, select);
    if (lower->GetAbsoluteRect().top >= y)
        return lower->GetBottomCaretState(x, y, caret_state, select);
    return parent->GetBottomCaretState(x, y, caret_state, select);
}

std::string Division::ToHtml()
{
    std::string s = "<mfrac>";
    s += upper->ToHtml();
    s += lower->ToHtml();
    s += "</mfrac>";
    return s;
}

}
