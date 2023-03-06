#include "block.h"
#include "caret.h"
#include "document.h"
#include "str.h"

namespace yutovo
{

//Block

Block::Block(Element* _parent) :
    Element(_parent)
{
}

Block::Block(Document* _document) :
    Element(_document)
{
}

bool Block::InsertElements(std::vector<ElementPtr>& _elements, bool with_undo)
{
    if (_elements.size() != 1 || !document->IsParagraph(_elements[0]))
    {
        if (!parent)
            return false;
        return parent->InsertElements(_elements, with_undo);
    }

    CaretState before_state = caret->GetCaretState();
    ElementPtr insert_element(_elements[0]->Clone());
    ElementPtr el = document->GetParent(before_state.id);
    ElementPtr paragraph = document->FindParentParagraph(el->id);
    ElementPtr row = document->FindParentRow(el->id);

    int k = elements->GetElementPos(paragraph->id);
    int p = row->elements->GetElementPos(el->id);
    bool caret_next_row = false;
    CaretState c;
    row->GetFirstCaretState(c, nullptr);

    ElementPtr clone;
    if (with_undo)
        clone.reset(paragraph->Clone());

    if (caret->GetCaretState() == c)
    {
        elements->Insert(insert_element, k);
    }
    else
    {
        elements->Insert(insert_element, k + 1);
        caret_next_row = true;
    }

    ElementPtr new_row = insert_element->elements->Get(0); //move elements into this one row, which will be splitted during paragraph formatting
    if (new_row->elements->Count() == 0)
        new_row->AddEmptyElement();

    if (p >= 0)
    {
        if (!el->SplitAt(before_state.GetPos()) && before_state.GetPos() == 0) //try to split current element
            --p;
        if (before_state.GetPos() != 0 || p >= 0)
        {
            for (int i = p + 1; i < row->elements->Count();) //move all elements at the right side of the row
                new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
        }
    }
    else
    {
        el = document->GetElement(before_state.id);
        if (el)
        {
            p = row->elements->GetElementPos(el->id);
            if (p >= 0)
            {
                for (int i = p; i < row->elements->Count();) //move all elements at the right side of the row
                    new_row->elements->Move(row->elements->Get(i), new_row->elements->Count());
            }
        }
    }

    int r_pos = paragraph->elements->GetElementPos(row->id);
    for (int i = r_pos + 1; i < paragraph->elements->Count();) //move the rest rows of the paragraph
    {
        ElementPtr r = paragraph->elements->Get(i);
        for (int j = 0; j < r->elements->Count();)
            new_row->elements->Move(r->elements->Get(j), new_row->elements->Count());
        paragraph->elements->RemoveAt(i, 1);
    }

    if (with_undo)
    {
        document->CallFunc(id, 
            [d = document, clone](const ElementId id)
            {
                d->GetElement(clone->id)->Normalize(false);
            },
            true);
        document->CallFunc(ElementId{}, 
            [d = document](const ElementId id)
            {
                d->can_normalize = true;
            },
            true);
        for (int i = 0; i < clone->elements->Count(); ++i)
        {
            document->InsertElement(clone->elements->Get(i)->Clone());
            document->PushEditorState(CaretState(clone->id, 0), true);
        }
        document->CallFunc(ElementId{}, 
            [d = document](const ElementId id)
            {
                d->can_normalize = false;
            },
            true);
        document->CallFunc(id, 
            [d = document, clone](const ElementId id)
            {
                d->GetElement(clone->id)->Normalize(false);
            },
            true);
        document->CallFunc(ElementId{}, 
            [d = document](const ElementId id)
            {
                d->can_normalize = true;
            },
            true);
        document->ClearElements(clone->id, false, true);
        document->DeleteElements(false, false, true);
        document->PushEditorState(insert_element->id, true);
        document->ClearElements(insert_element->id, false, true);
        document->CallFunc(ElementId{}, 
            [d = document](const ElementId id)
            {
                d->can_normalize = false;
            },
            true);
    }

    paragraph->Normalize(with_undo);
    new_row->parent->Normalize(with_undo);

    CaretState after;
    if (caret_next_row)
    {
        if (new_row->GetFirstCaretState(after, nullptr))
            document->caret->SetState(after);
    }
    else
    {
        if (row->GetFirstCaretState(after, nullptr))
            document->caret->SetState(after);
    }
    
    document->Remake(id, true, with_undo, false);

    new_row->Normalize(with_undo);
    
#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Block::DeleteElements(bool left, bool with_undo)
{
    CaretState before_state = caret->GetCaretState();
    ElementPtr el = document->GetElement(before_state.id);
    if (!el)
    {
        if (!parent)
            return false;
        return parent->DeleteElements(left, with_undo);
    }
    int p = 0;
    ElementPtr dest_row;
    if (document->IsParagraph(el))
    {
        p = elements->GetElementPos(el->id) - 1;
    }
    else
    {
        ElementPtr paragraph = document->FindParentParagraph(el->id);

        p = elements->GetElementPos(paragraph->id);
        if (left && p <= 0)
            return false;
        if (!left && p == elements->Count() - 1)
            return false;
        
        //merge current paragraph with the above one
        auto dest_p = left ? elements->Get(p - 1) : elements->Get(p);
        dest_row = dest_p->elements->Get(dest_p->elements->Count() - 1);
        if (dest_row)
        {
            auto source_p = left ? elements->Get(p) : elements->Get(p + 1);
            for (uint i = 0; i < source_p->elements->Count(); ++i)
            {
                auto row = source_p->elements->Get(i);
                for (uint j = 0; j < row->elements->Count();)
                    dest_row->elements->Move(row->elements->Get(j), dest_row->elements->Count());
            }
        }
    }

    elements->RemoveAt(left ? p : p + 1, 1);
    document->Remake(id, true, with_undo, false);

    if (with_undo)
        document->InsertParagraph(true, true);
    
    if (dest_row)
        dest_row->Normalize(with_undo);

#ifdef DEBUG
    to_str = ToText();
#endif
    return true;
}

bool Block::GetTopCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr p;
    //find nearest paragraph
    for (int i = 0; i < elements->Count(); ++i)
    {
        ElementPtr el = elements->Get(i);
        if (y < el->GetAbsoluteRect().GetBottom() || el->rect.height == 0)
            break;
        p = el;
    }
    if (!p)
    {
        if (!parent)
            return false;
        return parent->GetTopCaretState(x, y, caret_state, select);
    }
    return p->GetTopCaretState(x, y, caret_state, select);
}

bool Block::GetBottomCaretState(const int x, const int y, CaretState& caret_state, Selection* select)
{
    ElementPtr p;
    //find nearest paragraph
    for (int i = elements->Count() - 1; i >= 0; --i)
    {
        ElementPtr el = elements->Get(i);
        if (y > el->GetAbsoluteRect().top || el->rect.height == 0)
            break;
        p = el;
    }
    if (!p)
    {
        if (!parent)
            return false;
        return parent->GetBottomCaretState(x, y, caret_state, select);
    }
    return p->GetBottomCaretState(x, y, caret_state, select);
}

}
