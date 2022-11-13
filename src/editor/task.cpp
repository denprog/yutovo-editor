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

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, 
    uint _id, ElementId _element_id) :
    InsertElementsTask(_text, _elements, _before_state, _after_state, _id)
{
    element_id = _element_id;
}

bool InsertElementsTask::Execute()
{
    logger->Debug("Execute InsertElementsTask");
    if (before_state.IsEmpty())
        before_state = text->document->caret.GetCaretState();

    ElementPtr el;
    if (element_id.empty())
        el = text->document->GetParent(before_state.id);
    else
    {
        el = text->document->GetParent(element_id);
        before_state.id = element_id;
    }
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

    auto DeleteElements = [&](ElementPtr el, CaretState& _after_state)
    {
        assert(el != nullptr);
        if (el->DeleteElements(before_state, _after_state, left, with_undo))
        {
            text->document->Remake(el->parent->id, true);
            return true;
        }
        return false;
    };

    if (before_state.selections.IsEmpty())
    {
        if (DeleteElements(text->document->GetParent(before_state.id), after_state))
        {
            text->document->caret.SetState(after_state, true);
            return true;
        }
    }
    else
    {
        CaretState c;
        for (int i = before_state.selections.selections.size() - 1; i >= 0; --i)
        {
            Selection& s = before_state.selections.selections[i];
            CaretState _after_state;
            if (!DeleteElements(text->document->GetElement(s.id), after_state.IsEmpty() ? _after_state : after_state))
                return false; //todo transaction fix?
            if (!_after_state.IsEmpty())
                c.MergeState(_after_state);
        }
        if (!c.IsEmpty())
            after_state = c;
        text->document->caret.SetState(after_state, true);
        return true;
    }
    return false;
}

//RemakeTask

RemakeTask::RemakeTask(ElementPtr _text, const ElementId& _id, bool _with_elements) : 
    Task(_text), 
    element_id(_id),
    with_elements(_with_elements)
{
}

bool RemakeTask::Execute()
{
    logger->Debug("Execute RemakeTask element_id={}", IdToString(element_id));
    CaretState c = text->document->caret.GetCaretState();
    text->document->GetElement(element_id)->Remake(c, with_elements);
    text->document->caret.SetState(c, true);
    text->document->Redraw(element_id);
    return true;
}

//RedrawTask

RedrawTask::RedrawTask(ElementPtr _text, const ElementId& _id) :
    Task(_text),
    element_id(_id)
{
}

bool RedrawTask::Execute()
{
    logger->Debug("Execute RedrawTask element_id={}", IdToString(element_id));
    ElementPtr element = text->document->GetElement(element_id);
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

#ifdef DEBUG
    text->document->last_caret_moved = true;
#endif

    return true;
}

}
