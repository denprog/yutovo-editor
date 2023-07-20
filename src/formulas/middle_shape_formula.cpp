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
    first((CodeRow*)elements->Get(0).get()),
    shape((Shape*)elements->Get(1).get()),
    last((CodeRow*)elements->Get(2).get())
{
    if (!last)
    {
        last = new CodeRow(this);
        elements->Add(ElementPtr(last));
    }
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

bool MiddleShapeFormula::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo, ElementId& changed_element)
{
    if (caret->GetPos() == 1 && caret->GetElement()->id == id)
        return false;
    for (auto el : _elements)
    {
        if (el->type == ElementType::CODE_PARAGRAPH && type != ElementType::ASSIGNMENT && type != ElementType::EQUATION)
            return false;
    }
    return Formula::InsertElements(_elements, with_undo, changed_element);
}

bool MiddleShapeFormula::DeleteElements(bool left, bool with_undo, ElementId& changed_element)
{
    uint start, size;
    if (selection->Has(id, start, size))
    {
        if (start == 0 && size == 1)
        {
            first->elements->Clear();
            Normalize();
            changed_element = id;
            return true;
        }
        if (start == 2 && size == 1)
        {
            last->elements->Clear();
            Normalize();
            changed_element = id;
            return true;
        }
        return false;
    }
    else if (!selection->IsEmpty() || left || caret->GetPos() != 1)
        return false;
    
    if (with_undo)
        document->StoreUndo(parent->id);

    //remove this element by deleting its shape
    first->UpdateLevel(level);
    int p = parent->elements->GetElementPos(id);
    uint c1 = elements->Get(0)->elements->Count();

    uint c2 = 0;
    if (last)
    {
        last->UpdateLevel(level);
        c2 = elements->Get(2)->elements->Count();
    }

    caret->SetState(id);
    parent->elements->Move(*elements->Get(0)->elements, p);
    if (last)
        parent->elements->Move(*elements->Get(2)->elements, p + c1);
    CaretState c;
    if (parent->elements->Get(p + c1)->GetFirstCaretState(c, nullptr))
        caret->SetState(c);

    auto t = parent->elements->Get(p + c1 + c2); //for not removing this element until this function ends
    parent->elements->Remove(id);
    parent->Normalize();
    changed_element = id;
    return true;
}

bool MiddleShapeFormula::AfterInsert(bool with_undo)
{
    if (first->elements->Count() != 1 || last->elements->Count() != 1 || !caret)
        return false;
    
    String* str1 = dynamic_cast<String*>(first->elements->Get(0).get());
    String* str2 = dynamic_cast<String*>(last->elements->Get(0).get());
    if (str1 && str1->elements->Count() == 0)
    {
        int pos = parent->elements->GetElementPos(id);
        ElementPtr el1 = (pos == 0 ? nullptr : parent->elements->Get(pos - 1));
        ElementPtr el2 = (parent->elements->IsLast(id) ? nullptr : parent->elements->Get(pos + 1));

        if (!selection->IsEmpty())
        {
            ElementSelection& select = selection->selection[0];
            if (select.start > 0)
                select.element->SplitAt(select.start);
            if (selection->selection.size() > 1)
            {
                select = selection->selection[selection->selection.size()];
                select.element->SplitAt(select.start);
            }

            //move the selected elements in the first element
            first->elements->RemoveAt(0, 1);
            for (int i = selection->selection.size() - 1; i >= 0; --i)
            {
                ElementSelection& el_s = selection->selection[i];
                for (int j = el_s.size - 1; j >= 0; --j)
                    first->elements->Move(document->GetElement(GetChild(el_s.element->id, el_s.start + j)), 0);
            }
        }
        else
        {
            if (el2 && dynamic_cast<String*>(el2.get()) && str2 && str2->elements->Count() == 0)
            {
                //move the second element in the last element
                last->elements->RemoveAt(0, 1);
                last->elements->Move(document->GetElement(el2->id), 0);
            }
            if (el1 && dynamic_cast<String*>(el1.get()))
            {
                String* str = dynamic_cast<String*>(el1.get());
                if (str->elements->Count() > 0)
                {
                    //move the first element in the upper element
                    first->elements->RemoveAt(0, 1);
                    first->elements->Move(document->GetElement(el1->id), 0);
                }
            }
        }

        selection->Clear();

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
        parent->Normalize();
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
