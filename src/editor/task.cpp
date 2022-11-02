#include "task.h"
#include "text.h"
#include "document.h"
#include <assert.h>

namespace yutovo
{

//Task

uint Task::next_id = 1;

Task::Task(ElementPtr _text) : 
    text(_text),
    logger(Logger::GetInstance()),
    id(next_id++)
{
}

Task::Task(ElementPtr _text, const uint _id) :
    text(_text),
    logger(Logger::GetInstance()),
    id(_id)
{
}

//InsertElementsTask

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, 
    bool _with_undo) :
    Task(_text),
    elements(_elements),
    before_state(_before_state),
    after_state(_after_state)
{
    with_undo = _with_undo;
}

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, 
    uint _id) :
    Task(_text, _id),
    elements(_elements),
    before_state(_before_state),
    after_state(_after_state)
{
    with_undo = false;
}

bool InsertElementsTask::Execute()
{
    logger->Debug("Execute InsertElementsTask");
    if (before_state.IsEmpty())
        before_state = text->document->caret.GetCaretState();
    ElementPtr el = text->document->GetParent(before_state.id);
    assert(el != nullptr);
    if (el->InsertElements(elements, before_state, after_state, with_undo))
    {
        text->document->caret.SetState(after_state, true);
        text->document->Remake(el->parent->id, true);
        return true;
    }
    return false;
}

//DeleteElementsTask

DeleteElementsTask::DeleteElementsTask(ElementPtr _text, const CaretState& _before_state, CaretState& _after_state, bool _left, bool _with_undo) :
    Task(_text),
    before_state(_before_state),
    after_state(_after_state),
    left(_left)
{
    with_undo = _with_undo;
}

DeleteElementsTask::DeleteElementsTask(ElementPtr _text, const CaretState& _before_state, CaretState& _after_state, bool _left, uint _id) :
    Task(_text, _id),
    before_state(_before_state),
    after_state(_after_state),
    left(_left)
{
    with_undo = false;
}

bool DeleteElementsTask::Execute()
{
    logger->Debug("Execute DeleteElementsTask");
    ElementPtr el = text->document->GetParent(before_state.id);
    assert(el != nullptr);
    if (el->DeleteElements(before_state, after_state, left, with_undo))
    {
        text->document->caret.SetState(after_state, true);
        text->document->Remake(el->parent->id, true);
        return true;
    }
    return false;
}

//SplitElementTask

SplitElementTask::SplitElementTask(ElementPtr _text, ElementId _id, ElementId _remake_id, const uint _max_left_width) :
    Task(_text),
    id(_id),
    remake_id(_remake_id),
    max_left_width(_max_left_width)
{
}

SplitElementTask::SplitElementTask(ElementPtr _text, ElementId _id, ElementId _remake_id, const int _pos) :
    Task(_text),
    id(_id),
    remake_id(_remake_id),
    pos(_pos)
{
}

bool SplitElementTask::Execute()
{
    ElementPtr el = text->document->GetElement(id);
    if (max_left_width > 0)
    {
        CaretState caret_state = text->document->caret.GetCaretState();
        if (el->Split(max_left_width, caret_state))
        {
            text->document->caret.SetState(caret_state, true);
            text->document->Remake(remake_id, true);
            return true;
        }
    }
    return false;
}

//MergeElementsTask

MergeElementsTask::MergeElementsTask(ElementPtr _text, ElementId _id1, ElementId _id2, ElementId _remake_id) :
    Task(_text),
    id1(_id1),
    id2(_id2),
    remake_id(_remake_id)
{
}

bool MergeElementsTask::Execute()
{
    ElementPtr el1 = text->document->GetElement(id1);
    ElementPtr el2 = text->document->GetElement(id2);
    if (!el1 || !el2)
        return false;
    CaretState caret_state = text->document->caret.GetCaretState();
    if (el1->Merge(el2, caret_state))
    {
        text->document->caret.SetState(caret_state, true);
        text->document->Remake(remake_id, true);
        return true;
    }
    return false;
}

//RemakeTask

RemakeTask::RemakeTask(ElementPtr _text, const ElementId& _id, bool _with_elements) : 
    Task(_text), 
    id(_id),
    with_elements(_with_elements)
{
}

bool RemakeTask::Execute()
{
    logger->Debug("Execute RemakeTask id={}", ToString(id));
    CaretState c = text->document->caret.GetCaretState();
    text->document->GetElement(id)->Remake(c, with_elements);
    text->document->caret.SetState(c, true);
    text->document->Redraw(id);
    return true;
}

//RedrawTask

RedrawTask::RedrawTask(ElementPtr _text, const ElementId& _id) :
    Task(_text),
    id(_id)
{
}

bool RedrawTask::Execute()
{
    logger->Debug("Execute RedrawTask id={}", ToString(id));
    ElementPtr element = text->document->GetElement(id);
    text->window->DrawFillRect(element->GetAbsoluteRect(), Color::White());
    CaretState cur = text->document->caret.GetCaretState();
    element->Draw(cur.selections);
    text->window->Update(element->GetAbsoluteRect());
    return true;
}

//ResizeTask

ResizeTask::ResizeTask(ElementPtr _text, const uint _width, const uint _height) : 
    Task(_text),
    width(_width),
    height(_height)
{
}

bool ResizeTask::Execute()
{
    logger->Debug("Execute ResizeTask width={}, height={}", width, height);
    text->window->Resize(width, height);
    return true;
}

//MoveCaretTask

MoveCaretTask::MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible) : 
    Task(_text),
    caret(_caret),
    dir(_dir),
    visible(_visible)
{
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible, bool _selection) :
    MoveCaretTask(_text, _caret, _dir, _visible)
{
    selection = _selection;
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, Caret* _caret, Point _point) :
    Task(_text),
    caret(_caret),
    point(_point)
{
}

bool MoveCaretTask::Execute()
{
    switch (dir)
    {
    case MoveCaretDir::NONE:
        caret->SetVisible(visible);
        return true;
    case MoveCaretDir::LEFT:
        caret->MoveLeft(selection);
        break;
    case MoveCaretDir::RIGHT:
        caret->MoveRight(selection);
        break;
    case MoveCaretDir::UP:
        caret->MoveUp(selection);
        break;
    case MoveCaretDir::DOWN:
        caret->MoveDown(selection);
        break;
    case MoveCaretDir::HOME:
        caret->MoveHome(selection);
        break;
    case MoveCaretDir::END:
        caret->MoveEnd(selection);
        break;
    }

    Element* element = caret->current_element;
    Rect r = element->GetAbsoluteRect(element->GetCaretRect(caret->current_pos));
    Rect view_port = text->window->GetViewPort(0);

    //move view port in the view if the caret is outside of it
    if (r.left < text->window->GetDocumentPoint().x + view_port.left)
    {
        caret->SetVisible(false);
        text->window->MoveDocument(r.left - view_port.left - 1, 0);
        text->document->Redraw(text->id);
        text->document->SetCaretVisible(true);
    }
    else if (r.GetRight() > view_port.GetRight() + text->window->GetDocumentPoint().x)
    {
        caret->SetVisible(false);
        text->window->MoveDocument(r.GetRight() - view_port.GetRight(), 0);
        text->document->Redraw(text->id);
        text->document->SetCaretVisible(true);
    }
    else
    {
        CaretState cur = caret->GetCaretState();
        if (caret->last_selections != cur.selections)
        {
            //redraw elements with selection
            for (auto& s : cur.selections.selections)
                text->document->Redraw(s.id);
            for (auto& s : caret->last_selections.selections)
                text->document->Redraw(s.id);

            caret->last_selections = cur.selections;
        }
    }

    return true;
}

}
