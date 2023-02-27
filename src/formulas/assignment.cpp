#include "assignment.h"
#include "code_block.h"

namespace yutovo
{

//Assignment

Assignment::Assignment(Element* _parent) :
    MiddleShapeFormula(_parent)
{
    type = ElementType::ASSIGNMENT;
}

Assignment::Assignment(Document* _document) :
    MiddleShapeFormula(_document)
{
    type = ElementType::ASSIGNMENT;
}

Element* Assignment::Clone()
{
    return new Assignment(*this);
}

Element* Assignment::Create(Element* _parent)
{
    return new Assignment(_parent);
}

void Assignment::Draw() const
{
    shape->draw_func = 
        [&](const Rect& r)
        {
            Color c = document->selection.IsSelected(id) ? formula_format->selection_color : formula_format->color;
            float w = float(r.width);

            window->DrawFillEllipse((int)lround(r.left + w * 0.25), (int)lround(r.GetBottom() - r.height * 0.3), (int)lround(w * 0.05), (int)lround(w * 0.05), c);
            window->DrawFillEllipse((int)lround(r.left + w * 0.25), (int)lround(r.GetBottom() - r.height * 0.543), (int)lround(w * 0.05), (int)lround(w * 0.05), c);

            Rect s{(int)lround(r.left + w * 0.5), (int)lround(r.GetBottom() - r.height * 0.35), (int)lround(r.width * 0.45), (int)lround(r.height * 0.1)};
            if (s.height == 0)
                s.height = 1;
            if (s.width == 0)
                s.width = 1;
            window->DrawFillRect(s, c);
            s = Rect{(int)lround(r.left + w * 0.5), (int)lround(r.GetBottom() - r.height * 0.593), (int)lround(r.width * 0.45), (int)lround(r.height * 0.1)};
            if (s.height == 0)
                s.height = 1;
            if (s.width == 0)
                s.width = 1;
            window->DrawFillRect(s, c);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, Color::Blue());
    }

    MiddleShapeFormula::Draw();
}

void Assignment::UpdateRect(bool with_elements)
{
    Size s = parent->window->GetTextSize(std::u32string(U":="), GetStringFormat());
    shape->rect.SetSize(s.width, s.height);
    shape->baseline = shape->rect.height / 2;

    MiddleShapeFormula::UpdateRect(false);
}

void Assignment::Remake(bool with_elements, bool with_parent, bool with_undo)
{
    MiddleShapeFormula::Remake(with_elements, with_parent, with_undo);

    baseline = 0;
    for (uint i = 0; i < elements->Count(); ++i)
    {
        auto el = elements->Get(i);
        if (el->baseline > baseline)
            baseline = el->baseline;
    }

    first->rect.Move(0, baseline - first->baseline);
    shape->rect.Move(first->rect.width, baseline - shape->baseline);
    last->rect.Move(first->rect.width + shape->rect.width, baseline - last->baseline);

    if (rect != last_rect && with_parent)
        parent->Remake(false, true, with_undo);
    last_rect = rect;

    std::u32string expr = first->ToText() + U"=" + last->ToText();
    if (last_expression != expr)
    {
        auto code = document->FindParent(id, ElementType::CODE_BLOCK);
        if (last_identifier != U"")
            document->RemoveIdentifier(id, ((CodeBlock*)code.get())->code_id, last_identifier);
        document->SetUserIdentifier(id, ((CodeBlock*)code.get())->code_id, expr);
        last_identifier = first->ToText();
        last_expression = expr;
    }
}

bool Assignment::DeleteElements(bool left, bool with_undo)
{
    if (caret->IsOnElement(shape->id))
    {
        last = nullptr;
        elements->RemoveAt(2, 1);
    }
    return MiddleShapeFormula::DeleteElements(left, with_undo);
}

bool Assignment::AfterInsert(bool with_undo)
{
    int pos = parent->elements->GetElementPos(id);
    if (pos > 0)
        first->elements->Clear();
    for (int i = 0; i < pos; ++i)
    {
        auto el = parent->elements->Get(0);
        if (with_undo)
        {
            document->InsertElement(el->Clone(), false, true);
            document->PushEditorState(CaretState(parent->id, 0), true);
        }
        first->elements->Move(el, i);
    }
    CaretState c;
    last->GetFirstCaretState(c, nullptr);
    caret->SetState(c);
    return true;
}

void Assignment::ReSolve()
{
    last_expression = U"";
    document->Remake(id, true, false, false);
}

std::string Assignment::ToHtml()
{
    std::string s = first->ToHtml();
    s += "<mo>=</mo>";
    if (last)
        s += last->ToHtml();
    return s;
}

std::u32string Assignment::ToText()
{
    std::u32string s = first->ToText();
    s += U"=";
    if (last)
        s += last->ToText();
    return s;
}

}
