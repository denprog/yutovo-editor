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
            TaskPtr t;
            {
                std::lock_guard<std::mutex> lock(tasks_mutex);
                if (!undos.empty())
                {
                    //execute one undo
                    if (!undo_tasks.empty())
                        t = undo_tasks.top();
                }
            }
            if (t)
            {
                caret.Hide();
                if (t->Execute())
                {
                    std::lock_guard<std::mutex> lock(tasks_mutex);
                    undo_tasks.pop();
                    undos.erase(undos.begin());
                }
                caret.Show();
            }
        }

        {
            TaskPtr t;
            {
                std::lock_guard<std::mutex> lock(tasks_mutex);
                if (!redos.empty())
                {
                    //execute one redo
                    if (redo_tasks.size() > undo_tasks.size())
                        t = redo_tasks[undo_tasks.size()];
                }
            }
            if (t)
            {
                caret.Hide();
                if (t->Execute())
                {
                    std::lock_guard<std::mutex> lock(tasks_mutex);
                    redos.erase(redos.begin());
                }
                caret.Show();
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
                if (t->Execute() && t->with_undo)
                {
                    redo_tasks.erase(redo_tasks.begin() + undo_tasks.size() - 1, redo_tasks.end()); //shrink to the size of undo
                    redo_tasks.push_back(t);
                }
            }
            caret.Show();
        }
    }
}

void Document::InsertPage(bool with_undo)
{
    InsertElement(new Page(nullptr), caret.GetCaretState(), with_undo);
}

void Document::InsertParagraph(bool with_undo)
{
    InsertElement(new Paragraph(nullptr), caret.GetCaretState(), with_undo);
}

void Document::InsertText(const std::string& str, bool with_undo)
{
    InsertElement(new String(nullptr, str), caret.GetCaretState(), with_undo);
}

void Document::InsertText(const std::string& str, const StringFormatPtr string_format, bool with_undo)
{
    InsertElement(new String(nullptr, str, string_format), CaretState(), with_undo);
}

void Document::InsertElement(Element* element, const CaretState& caret_state, bool with_undo, bool undo)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    InsertElements(elements, caret_state, with_undo, undo);
}

void Document::InsertElements(std::vector<ElementPtr>& elements, const CaretState& caret_state, bool with_undo, bool undo)
{
    CaretState c;
    InsertElements(elements, caret_state, c, with_undo, undo);
}

void Document::InsertElements(std::vector<ElementPtr>& elements, const CaretState& before_state, CaretState& after_state, bool with_undo, bool undo)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    if (undo)
        undo_tasks.push(TaskPtr(new InsertElementsTask(text, elements, before_state, after_state, with_undo)));
    else
        tasks.emplace_back(new InsertElementsTask(text, elements, before_state, after_state, with_undo));
    next_circle.notify_one();
}

void Document::DeleteElements(const CaretState& caret_state, bool with_undo, bool undo)
{
    CaretState c;
    DeleteElements(caret_state, c, with_undo, undo);
}

void Document::DeleteElements(const CaretState& before_state, CaretState& after_state, bool with_undo, bool undo)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    if (undo)
        undo_tasks.push(TaskPtr(new DeleteElementsTask(text, before_state, after_state, with_undo)));
    else
        tasks.emplace_back(new DeleteElementsTask(text, before_state, after_state, with_undo));
    next_circle.notify_one();
}

void Document::SplitElement(const ElementId& id, const ElementId& remake_id, const uint max_left_width)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new SplitElementTask(text, id, remake_id, max_left_width));
    next_circle.notify_one();
}

void Document::MergeElement(const ElementId& id1, const ElementId& id2, ElementId remake_id)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MergeElementsTask(text, id1, id2, remake_id));
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
        el = el->elements->Get(_id[i]);
    return el;
}

Rect Document::GetCaretRect(const CaretState& caret_state)
{
    ElementPtr el = GetElement(caret_state.id);
    return el->GetAbsoluteRect(el->GetCaretRect(caret_state.GetPos()));
}

void Document::MoveCaretLeft(bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::LEFT, true, selection));
    next_circle.notify_one();
}

void Document::MoveCaretRight(bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::RIGHT, true, selection));
    next_circle.notify_one();
}

void Document::MoveCaretUp(bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::UP, true, selection));
    next_circle.notify_one();
}

void Document::MoveCaretDown(bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::DOWN, true, selection));
    next_circle.notify_one();
}

void Document::MoveCaretHome(bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::HOME, true, selection));
    next_circle.notify_one();
}

void Document::MoveCaretEnd(bool selection)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::END, true, selection));
    next_circle.notify_one();
}

void Document::SetCaretVisible(bool visible)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new MoveCaretTask(text, &caret, MoveCaretTask::MoveCaretDir::NONE, visible));
    next_circle.notify_one();
}

void Document::Undo()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    undos.push_back(true);
    next_circle.notify_one();
}

void Document::Redo()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    redos.push_back(true);
    next_circle.notify_one();
}

bool Document::CanUndo()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return !undos.empty();
}

bool Document::CanRedo()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return !redos.empty();
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
        if (!t || t->id != id)
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
        if (!t || t->id != id || t->with_elements != with_elements)
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
    return text->ToHtml();
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

}
