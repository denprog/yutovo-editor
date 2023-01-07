#include "middle_shape_formula.h"
#include "code_row.h"
#include "../caret.h"
#include "../document.h"

namespace yutovo
{

//MiddleShapeFormula

MiddleShapeFormula::MiddleShapeFormula(Element* _parent) :
    Formula(_parent)
{
    Init();
}

MiddleShapeFormula::MiddleShapeFormula(Document* _document) :
    Formula(_document)
{
    Init();
}

MiddleShapeFormula::MiddleShapeFormula(const MiddleShapeFormula& source) :
    Formula(source),
    first(elements->Get(0).get()),
    shape((Shape*)elements->Get(1).get()),
    last(elements->Get(2).get())
{
}

void MiddleShapeFormula::Init()
{
    first = new CodeRow(this);
    elements->Add(ElementPtr(first));
    shape = new Shape(this);
    elements->Add(ElementPtr(shape));
    last = new CodeRow(this);
    elements->Add(ElementPtr(last));
}

bool MiddleShapeFormula::DeleteElements(bool left, bool with_undo)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start == 0 && size == 1)
        {
            first->elements->Clear();
            Normalize(with_undo);
            return true;
        }
        if (start == 2 && size == 1)
        {
            last->elements->Clear();
            Normalize(with_undo);
            return true;
        }
        return false;
    }
    else if (!selection->IsEmpty() || left || caret->GetPos() != 1)
        return false;
    
    //remove this element by deleting its shape
    first->UpdateLevel(level);
    last->UpdateLevel(level);
    int p = parent->elements->GetElementPos(id);
    uint c1 = elements->Get(0)->elements->Count();
    uint c2 = elements->Get(2)->elements->Count();
    Element* undo_el = nullptr;
    if (with_undo)
        undo_el = Clone();

    caret->SetState(id);
    parent->elements->Move(*elements->Get(0)->elements, p);
    parent->elements->Move(*elements->Get(2)->elements, p + 1);
    parent->elements->Remove(id);
    CaretState c;
    if (parent->elements->Get(p + 1)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    if (with_undo)
    {
        document->InsertElement(undo_el, false, true);
        document->PushEditorState(CaretState(parent->id, p), true);
        document->DeleteElements(false, false, true);
        document->PushEditorState(SelectionState(parent->id, p, c1 + c2), true);
    }

    parent->Normalize(with_undo);
    document->Remake(parent->id, true, with_undo, false);
    return true;
}

bool MiddleShapeFormula::AfterInsert(ElementPtr el1, ElementPtr el2, bool with_undo)
{
    if (first->elements->Count() != 1 || last->elements->Count() != 1 || !caret)
        return false;
    
    String* str1 = dynamic_cast<String*>(first->elements->Get(0).get());
    String* str2 = dynamic_cast<String*>(last->elements->Get(0).get());
    if (str1 && str1->elements->Count() == 0)
    {
        if (el2 && str2 && str2->elements->Count() == 0)
        {
            //move the second element in the last element
            last->elements->RemoveAt(0, 1);
            last->elements->Move(document->GetElement(el2->id), 0);
            if (with_undo)
            {
                auto _el2 = el2->Clone();
                _el2->dont_normalize = true;
                document->InsertFormula(_el2, false, true);
                document->PushEditorState(CaretState(parent->id, parent->elements->GetElementPos(id)), true);
            }
        }
        if (el1)
        {
            //move the first element in the upper element
            first->elements->RemoveAt(0, 1);
            first->elements->Move(document->GetElement(el1->id), 0);
            if (with_undo)
            {
                auto _el1 = el1->Clone();
                _el1->dont_normalize = true;
                document->InsertFormula(_el1, false, true);
                document->PushEditorState(CaretState(parent->id, parent->elements->GetElementPos(id)), true);
            }
        }

        UpdateFormat(GetFormulaFormat()->string_format);

        CaretState c;
        if (first->elements->Get(0)->elements->Count() == 0)
        {
            if (first->GetFirstCaretState(c, nullptr))
                caret->SetState(c);
        }
        else
        {
            if (last->GetFirstCaretState(c, nullptr))
                caret->SetState(c);
        }

        UpdateLevel(level);
        return true;
    }

    UpdateLevel(level);
    return false;
}

bool MiddleShapeFormula::GetLeftCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        CaretState c;
        if ((last->GetFirstCaretState(c, nullptr) && caret_state == c) || (first->GetFirstCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(id);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetLeftCaretState(caret_state, select);
}

bool MiddleShapeFormula::GetRightCaretState(CaretState& caret_state, Selection* select)
{
    if (select)
    {
        CaretState c;
        if ((last->GetLastCaretState(c, nullptr) && caret_state == c) || (first->GetLastCaretState(c, nullptr) && caret_state == c))
        {
            caret_state.SetState(parent->id, parent->elements->GetChildPos(id) + 1);
            select->Clear();
            select->Add(parent->id, parent->elements->GetChildPos(id), 1);
            return true;
        }
    }
    return Formula::GetRightCaretState(caret_state, select);
}

}
