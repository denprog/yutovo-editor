#include "document.h"
#include "str.h"
#include "paragraph.h"
#include "row.h"
#include <assert.h>
#include <chrono>

namespace yutovo
{

using namespace std::chrono_literals;

//Document

Document::Document(Window* _window) :
    window(_window),
    text(new Text(this)),
    caret(_window, (Text*)text.get()),
    logger(Logger::GetInstance())
{
    logger->Info("Document start");

    caret.MoveToDocumentBegin(false);

    main_loop = std::thread(&Document::MainLoop, this);

    Remake(text->id, true);
}

Document::~Document()
{
    exit = true;
    next_circle.notify_one();
    main_loop.join();
    logger->Info("Document end");
}

void Document::MainLoop()
{
    std::vector<TaskPtr> temp_tasks;

    while (!exit)
    {
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            if (tasks.empty() && undos.empty() && redos.empty())
            {
                if (next_circle.wait_for(lock, caret_settings.blink_delay * 1ms) == std::cv_status::timeout) //wait for tasks
                {
                    caret.Blink();
                    continue;
                }
            }
        }

        {
            std::vector<TaskPtr> temp_undo_tasks;
            {
                std::lock_guard<std::mutex> lock(tasks_mutex);
                if (!undos.empty())
                {
                    //execute one undo
                    if (!undo_tasks.empty())
                    {
                        //collect tasks with one id
                        TaskPtr t = undo_tasks.top();
                        uint id = t->id;
                        while (id == t->id)
                        {
                            temp_undo_tasks.push_back(t);
                            undo_tasks.pop();
                            if (undo_tasks.empty())
                                break;
                            t = undo_tasks.top();
                        }
                    }
                }
            }
            if (!temp_undo_tasks.empty())
            {
                caret.Hide();
                for (TaskPtr t : temp_undo_tasks)
                {
                    if (!t->Execute())
                        break;
                }
                {
                    std::lock_guard<std::mutex> lock(tasks_mutex);
                    undos.erase(undos.begin());
                }
                caret.Show();

#ifdef DEBUG
                last_undo_executed = true;
#endif
            }
        }

        {
            std::vector<TaskPtr> temp_redo_tasks;
            {
                std::lock_guard<std::mutex> lock(tasks_mutex);
                if (!redos.empty())
                {
                    //execute one redo: collect all redo tasks for first absent undo
                    uint last_undo_task_id = 0;
                    if (!undo_tasks.empty())
                        last_undo_task_id = undo_tasks.top()->id;
                    
                    int i = 0;
                    for (i = redo_tasks.size() - 1; i >=0; --i)
                    {
                        if (redo_tasks[i]->id == last_undo_task_id)
                            break;
                    }
                    uint redo_task_id = redo_tasks[++i]->id;
                    while (i < redo_tasks.size() && redo_tasks[i]->id == redo_task_id)
                    {
                        temp_redo_tasks.push_back(redo_tasks[i++]);
                    }
                }
            }
            if (!temp_redo_tasks.empty())
            {
                caret.Hide();
                for (TaskPtr t : temp_redo_tasks)
                {
                    cur_task_id = t->id;
                    if (!t->Execute())
                        break;
                }
                {
                    std::lock_guard<std::mutex> lock(tasks_mutex);
                    redos.erase(redos.begin());
                }
                caret.Show();

#ifdef DEBUG
                last_redo_executed = true;
#endif
            }
        }

        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            temp_tasks = tasks;
            tasks.clear();
        }

        if (!temp_tasks.empty())
        {
            caret.Hide();
            //execute all the tasks
            for (auto& t : temp_tasks)
            {
                cur_task_id = t->id;
                uint last_undo_task_id = 0;
                if (!undo_tasks.empty())
                    last_undo_task_id = undo_tasks.top()->id;
                
                if (t->Execute() && t->with_undo)
                {
                    //shrink the redo vector to the size of the undo stack
                    for (int i = redo_tasks.size() - 1; i >= 0; --i)
                    {
                        if (redo_tasks[i]->id == last_undo_task_id)
                            break;
                        redo_tasks.erase(redo_tasks.begin() + i);
                    }

                    redo_tasks.push_back(t);
                }

#ifdef DEBUG
                if (last_task_id == t->id)
                    last_task_executed = true;
#endif
            }
            caret.Show();
        }
    }
}

void Document::InsertPage(bool with_undo)
{
    InsertElement(new Page(nullptr), CaretState(), with_undo);
}

void Document::InsertParagraph(bool with_undo)
{
    InsertElement(new Paragraph(nullptr), CaretState(), with_undo);
}

void Document::InsertText(const std::string& str, bool with_undo)
{
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        InsertElement(new String(nullptr, str, format), CaretState(), with_undo);
}

void Document::InsertText(const std::string& str, const StringFormatPtr string_format, bool with_undo)
{
    InsertElement(new String(nullptr, str, string_format), CaretState(), with_undo);
}

void Document::InsertText(const std::string& str, const StringFormatPtr string_format, const CaretState& before_state, CaretState& after_state, 
    ElementId element_id)
{
    InsertElement(new String(nullptr, str, string_format), before_state, after_state, element_id);
}

void Document::InsertElement(Element* element, const CaretState& caret_state, bool with_undo, bool undo, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    InsertElements(elements, caret_state, with_undo, undo, element_id);
}

void Document::InsertElement(Element* element, const CaretState& before_state, CaretState& after_state, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    InsertElements(elements, before_state, after_state, false, true, element_id);
}

void Document::InsertElements(std::vector<ElementPtr>& elements, const CaretState& caret_state, bool with_undo, bool undo, ElementId element_id)
{
    CaretState c;
    InsertElements(elements, caret_state, c, with_undo, undo, element_id);
}

void Document::InsertElements(std::vector<ElementPtr>& elements, const CaretState& before_state, CaretState& after_state, bool with_undo, bool undo, 
    ElementId element_id)
{
    {
        std::lock_guard<std::mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new InsertElementsTask(text, elements, before_state, after_state, cur_task_id, element_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new InsertElementsTask(text, elements, before_state, after_state, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::DeleteElements(bool left, bool with_undo, bool undo)
{
    DeleteElements(caret.GetCaretState(), left, with_undo, undo);
}

void Document::DeleteElements(const CaretState& caret_state, bool left, bool with_undo, bool undo)
{
    CaretState c;
    DeleteElements(caret_state, c, left, with_undo, undo);
}

void Document::DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new DeleteElementsTask(text, before_state, after_state, left, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new DeleteElementsTask(text, before_state, after_state, left, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

ElementPtr Document::GetElement(const ElementId& _id)
{
    if (_id.size() == 1)
        return text;
    ElementPtr el = text->elements->Get(_id[1]);
    for (uint i = 2; i < _id.size(); ++i)
    {
        if (el->elements->Count() <= _id[i])
            return nullptr;
        el = el->elements->Get(_id[i]);
    }
    return el;
}

ElementPtr Document::GetParent(const ElementId& _id)
{
    if (_id.size() == 2)
        return text;
    ElementPtr el = text->elements->Get(_id[1]);
    for (uint i = 2; i < _id.size() - 1; ++i)
    {
        if (el->elements->Count() <= _id[i])
            return nullptr;
        el = el->elements->Get(_id[i]);
    }
    return el;
}

Rect Document::GetCaretRect(const CaretState& caret_state)
{
    ElementPtr el = GetParent(caret_state.id);
    return el->GetAbsoluteRect(el->GetCaretRect(caret_state.GetPos()));
}

bool Document::GetCurrentStringFormat(StringFormatPtr& format)
{
    CaretState c = caret.GetCaretState();
    ElementPtr el = GetParent(c.id);
    if (!el || el->type != ElementType::STRING)
        return false;
    format = ((String*)el.get())->format;
    return true;
}

void Document::MoveCaret(MoveCaretTask::MoveCaretDir dir, bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, dir, true, selection));
    next_circle.notify_one();

#ifdef DEBUG
    last_caret_moved = false;
#endif
}

void Document::MoveCaretLeft(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::LEFT, selection);
}

void Document::MoveCaretRight(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::RIGHT, selection);
}

void Document::MoveCaretUp(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::UP, selection);
}

void Document::MoveCaretDown(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::DOWN, selection);
}

void Document::MoveCaretHome(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::HOME, selection);
}

void Document::MoveCaretEnd(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::END, selection);
}

void Document::MoveCaretWordLeft(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::WORD_LEFT, selection);
}

void Document::MoveCaretWordRight(bool selection)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::WORD_RIGHT, selection);
}

void Document::SetCaretVisible(bool visible)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::NONE, visible));
    next_circle.notify_one();
}

void Document::Undo()
{
    if (!CanUndo())
        return;
    std::lock_guard<std::mutex> lock(tasks_mutex);
    undos.push_back(true);
    next_circle.notify_one();
}

void Document::Redo()
{
    if (!CanRedo())
        return;
    std::lock_guard<std::mutex> lock(tasks_mutex);
    redos.push_back(true);
    next_circle.notify_one();
}

bool Document::CanUndo()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return !undo_tasks.empty();
}

bool Document::CanRedo()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return !redo_tasks.empty();
}

void Document::Resize(uint width, uint height)
{
    {
        std::lock_guard<std::mutex> lock(tasks_mutex);
        tasks.emplace_back(new ResizeTask(text, width, height));
        next_circle.notify_one();
    }
    Remake(text->id, true);
}

void Document::Redraw(const ElementId& id)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    if (!tasks.empty())
    {
        TaskPtr last = tasks[tasks.size() - 1];
        RedrawTask* t = dynamic_cast<RedrawTask*>(last.get());
        if (!t || t->element_id != id)
            tasks.emplace_back(new RedrawTask(text, id));
    }
    else
    {
        tasks.emplace_back(new RedrawTask(text, id));
    }
    next_circle.notify_one();
}

void Document::Redraw()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new RedrawTask(text, text->id));
    next_circle.notify_one();
}

void Document::Remake(const ElementId& id, bool with_elements)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    if (!tasks.empty())
    {
        TaskPtr last = tasks[tasks.size() - 1];
        RemakeTask* t = dynamic_cast<RemakeTask*>(last.get());
        if (!t || t->element_id != id || t->with_elements != with_elements)
            tasks.emplace_back(new RemakeTask(text, id, with_elements));
    }
    else
    {
        tasks.emplace_back(new RemakeTask(text, id, with_elements));
    }
    next_circle.notify_one();
}

std::string Document::ToHtml()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return text->ToHtml();
}

std::string Document::ToText()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return text->ToText();
}

TextFormatPtr Document::GetDefaultTextFormat()
{
    return TextFormats::GetFormat(TextFormat::Paging::ONE_PAGE);
}

PageFormatPtr Document::GetDefaultPageFormat()
{
    return PageFormats::GetFormat(20, 20, 20, 20);
}

ParagraphFormatPtr Document::GetDefaultParagraphFormat()
{
    return ParagraphFormats::GetFormat(ParagraphFormat::Alignment::Left, ParagraphFormat::WordWrap::Normal, 5, 10, 10, 0, 10, 10);
}

StringFormatPtr Document::GetDefaultStringFormat()
{
    return StringFormats::GetFormat("Arial", 22, false, false, false);
}

#ifdef DEBUG
void Document::WaitMainLoop()
{
    while (!last_task_executed && !last_undo_executed && !last_redo_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_task_id = 0;
    last_task_executed = false;
    last_undo_executed = false;
    last_redo_executed = false;
}

void Document::WaitCaretMoving()
{
    while (!last_caret_moved)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_caret_moved = false;
}
#endif

}
