#include "document.h"
#include "str.h"
#include "paragraph.h"
#include "row.h"
#include "page.h"
#include "formulas/code.h"
#include "formulas/code_string.h"
#include "formulas/plus.h"
#include "formulas/minus.h"
#include "formulas/multiply.h"
#include "formulas/division.h"
#include "formulas/power.h"
#include "formulas/nth_root.h"
#include "formulas/square_root.h"
#include "formulas/equation.h"
#include "util.h"
#include <assert.h>
#include <chrono>
#include <sstream>

namespace yutovo
{

using namespace std::chrono_literals;

//Document

Document::Document(Window* _window) :
    window(_window),
    string_formats(new StringFormats()),
    paragraph_formats(new ParagraphFormats(string_formats)),
    formula_formats(new FormulaFormats(string_formats)),
    current_paragraph_format(paragraph_formats->GetFormat("Text body")),
    current_formula_format(formula_formats->GetFormat("Code")),
    selection(this),
    last_selection(this),
    solver(this),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true))
{
    logger->Debug("Document start");
}

Document::~Document()
{
    exit = true;
    next_circle.notify_one();
    main_loop.join();
    logger->Debug("Document end");
}

void Document::Start()
{
    caret.reset(new Caret(this));
    text.reset(new Text(this));

    caret->MoveToDocumentBegin(nullptr);

    main_loop = std::thread(&Document::MainLoop, this);

    Remake(text->id, true, false, false);
}

void Document::MainLoop()
{
    std::vector<TaskPtr> temp_tasks;

    while (!exit)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(tasks_mutex);
            if (tasks.empty() && undos.empty() && redos.empty())
            {
                if (next_circle.wait_for(lock, caret_settings.blink_delay * 1ms) == std::cv_status::timeout) //wait for tasks
                {
                    caret->Blink();
                    continue;
                }
            }
        }

        {
            std::vector<TaskPtr> temp_undo_tasks;
            {
                std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
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
                    undos.clear();
                }
            }
            if (!temp_undo_tasks.empty())
            {
                caret->Hide(); //caret will be shown on Redraw or caret moving
                for (TaskPtr t : temp_undo_tasks)
                {
                    if (!t->Execute())
                        break;
                }
#ifdef DEBUG
                last_undo_executed = true;
#endif
            }
        }

        {
            std::vector<TaskPtr> temp_redo_tasks;
            {
                std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
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

                    if (++i < redo_tasks.size())
                    {
                        uint redo_task_id = redo_tasks[i]->id;
                        while (i < redo_tasks.size() && redo_tasks[i]->id == redo_task_id)
                        {
                            temp_redo_tasks.push_back(redo_tasks[i++]);
                        }
                    }
                    redos.clear();
                }
            }
            if (!temp_redo_tasks.empty())
            {
                caret->Hide(); //caret will be shown on Redraw or caret moving
                for (TaskPtr t : temp_redo_tasks)
                {
                    cur_task_id = t->id;
                    if (!t->Execute())
                        break;
                }
#ifdef DEBUG
                last_redo_executed = true;
#endif
            }
        }

        {
            std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
            temp_tasks = tasks;
            tasks.clear();
        }

        if (!temp_tasks.empty())
        {
            caret->Hide(); //caret will be shown on Redraw or caret moving
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
                if (last_task_id > 0)
                {
                    if (last_task_id == t->id)
                        last_task_executed = true;
                }
                if (last_load_task_id == t->id)
                    last_load_executed = true;
                if (last_solver_task_id == t->id)
                    last_solver_executed = true;
                
                std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
                if (last_tasks.size() > 1000)
                    last_tasks.clear();
                last_tasks.push_back(t->id);
#endif
            }
        }
    }
}

void Document::InsertPage(bool with_undo)
{
    InsertElement(new Page(text.get()), with_undo);
}

void Document::InsertParagraph(bool with_undo, bool undo)
{
    InsertElement(new Paragraph(this), with_undo, undo);
}

void Document::InsertString(const std::string& str, bool with_undo)
{
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        InsertElement(new String(this, str, format), with_undo);
}

void Document::InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo)
{
    InsertElement(new String(this, str, string_format), with_undo);
}

void Document::InsertString(const std::string& str, const StringFormatPtr string_format, ElementId element_id)
{
    InsertElement(new String(this, str, string_format), element_id);
}

void Document::InsertElement(Element* element, bool with_undo, bool undo, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    InsertElements(elements, with_undo, undo, element_id);
}

void Document::InsertElement(Element* element, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    InsertElements(elements, false, true, element_id);
}

void Document::InsertElement(ElementPtr element, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.push_back(element);
    InsertElements(elements, false, true, element_id);
}

void Document::InsertElements(std::vector<ElementPtr>& elements, bool with_undo, bool undo, ElementId element_id)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new InsertElementsTask(text, elements, cur_task_id, element_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new InsertElementsTask(text, elements, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::DeleteElements(bool left, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new DeleteElementsTask(text, left, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new DeleteElementsTask(text, left, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::ClearElements(ElementId element_id, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new DeleteElementsTask(text, element_id, with_undo, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new DeleteElementsTask(text, element_id, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::InsertCode(bool with_undo)
{
    InsertFormula(new Code(this), with_undo, false);
}

void Document::InsertCodeString(const std::string& str, bool with_undo)
{
    FormulaFormatPtr format;
    if (GetCurrentFormulaFormat(format))
        InsertFormula(new CodeString(this, str, format->string_format), with_undo, false);
}

void Document::InsertPlus(bool with_undo)
{
    InsertFormula(new Plus(this), with_undo, false);
}

void Document::InsertMinus(bool with_undo)
{
    InsertFormula(new Minus(this), with_undo, false);
}

void Document::InsertMultiply(bool with_undo)
{
    InsertFormula(new Multiply(this), with_undo, false);
}

void Document::InsertDivision(bool with_undo)
{
    InsertFormula(new Division(this), with_undo, false);
}

void Document::InsertPower(bool with_undo)
{
    InsertFormula(new Power(this), with_undo, false);
}

void Document::InsertNthRoot(bool with_undo)
{
    InsertFormula(new NthRoot(this), with_undo, false);
}

void Document::InsertSquareRoot(bool with_undo)
{
    InsertFormula(new SquareRoot(this), with_undo, false);
}

void Document::InsertEquation(ResultType result_type, bool with_undo)
{
    InsertFormula(new Equation(this, result_type), with_undo, false);
}

void Document::InsertFormula(Element* element, bool with_undo, bool undo)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    InsertFormulas(elements, with_undo, undo);
}

void Document::InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new InsertFormulasTask(text, elements, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new InsertFormulasTask(text, elements, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, 
    bool with_undo, bool undo)
{
    ChangeStringFormat(string_formats->GetFormat(family, size, bold, italic, underline), with_undo, undo);
}

void Document::ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
    bool with_undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangeStringFormatTask(text, format, set_family, set_size, set_bold, set_italic, set_underline, with_undo));
        last_task_id = tasks[tasks.size() - 1]->id;
    }
    next_circle.notify_one();
}

void Document::ChangeStringFormat(const StringFormatPtr format, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new ChangeStringFormatTask(text, format, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new ChangeStringFormatTask(text, format, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new ChangeParagraphFormatTask(text, format, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new ChangeParagraphFormatTask(text, format, with_undo));
            last_task_id = tasks[tasks.size() - 1]->id;
        }
    }
    next_circle.notify_one();
}

void Document::PushEditorState(bool undo)
{
    PushEditorState(caret->GetCaretState(), selection.GetState(), undo);
}

void Document::PushEditorState(const EditorState& editor_state, bool undo)
{
    PushEditorState(editor_state.caret_state, editor_state.selection_state, undo);
}

void Document::PushEditorState(const CaretState& caret_state, bool undo)
{
    PushEditorState(caret_state, selection.GetState(), undo);
}

void Document::PushEditorState(const SelectionState& selection_state, bool undo)
{
    PushEditorState(caret->GetCaretState(), selection_state, undo);
}

void Document::PushEditorState(const CaretState& caret_state, const SelectionState& selection_state, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
            undo_tasks.push(TaskPtr(new SetEditorStateTask(text, caret_state, selection_state, cur_task_id)));
        else
            tasks.emplace_back(new SetEditorStateTask(text, caret_state, selection_state, cur_task_id));
    }
    next_circle.notify_one();
}

void Document::CallFunc(const ElementId& _id, std::function<void (const ElementId id)> func, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
            undo_tasks.push(TaskPtr(new CallFuncTask(text, _id, func, cur_task_id)));
        else
            tasks.emplace_back(new CallFuncTask(text, _id, func, cur_task_id));
    }
    next_circle.notify_one();
}

void Document::ResetTasks()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.clear();
    undo_tasks = std::stack<TaskPtr>();
    redo_tasks.clear();
}

ElementPtr Document::GetElement(const ElementId& _id)
{
    if (_id.size() == 1)
        return text;
    ElementPtr el = text->elements->Get(_id[1]);
    for (uint i = 2; i < _id.size(); ++i)
    {
        if (!el || el->elements->Count() < _id[i])
            return nullptr;
        el = el->elements->Get(_id[i]);
    }
    return el;
}

ElementPtr Document::GetParent(const ElementId& _id)
{
    if (_id.size() == 1)
        return nullptr;
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

bool Document::GetElementAtCoords(const int x, const int y, ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return text->GetElementAtCoords(x, y, id);
}

ElementPtr Document::FindParent(const ElementId& id, const ElementType type)
{
    ElementPtr el = GetElement(id);
    if (el && el->type == type)
        return el;
    el = GetParent(id);
    while (el && el->type != type)
        el = GetParent(el->id);
    return el;
}

Rect Document::GetCaretRect(const CaretState& caret_state)
{
    ElementPtr el = GetParent(caret_state.id);
    return el->GetAbsoluteRect(el->GetCaretRect(caret_state.GetPos()));
}

bool Document::GetCurrentStringFormat(StringFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        format = current_string_format;
        return true;
    }
    return false;
}

void Document::SetCurrentStringFormat(StringFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    current_string_format = format;
}

bool Document::GetCurrentParagraphFormat(ParagraphFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_paragraph_format)
    {
        format = current_paragraph_format;
        return true;
    }
    return false;
}

bool Document::GetCurrentFormulaFormat(FormulaFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_formula_format)
    {
        format = current_formula_format;
        return true;
    }
    return false;
}

void Document::UpdateFormats()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    CaretState c = caret->GetCaretState();
    ElementPtr el = GetParent(c.id);
    if (!el)
    {
        current_string_format.reset();
        return;
    }
    StringFormat f;
    if (GetStringFormat(el->id, f))
        current_string_format = string_formats->GetFormat(f);
}

void Document::SetFontFamily(const std::string& family)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        if (!selection.IsEmpty())
        {
            ChangeStringFormat(string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline), true, false);
        }
        else
        {
            current_string_format = string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline);
        }
    }
}

void Document::SetFontSize(const uint size)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, size, current_string_format->bold, current_string_format->italic, 
            current_string_format->underline);
        if (!selection.IsEmpty())
            ChangeStringFormat(f, false, true, false, false, false, true);
        else
            current_string_format = f;
    }
}

void Document::SetBold(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, enabled, current_string_format->italic, 
            current_string_format->underline);
        if (!selection.IsEmpty())
            ChangeStringFormat(f, false, false, true, false, false, true);
        else
            current_string_format = f;
    }
}

void Document::SetItalic(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, enabled, 
            current_string_format->underline);
        if (!selection.IsEmpty())
            ChangeStringFormat(f, true, false);
        else
            current_string_format = f;
    }
}

void Document::SetUnderline(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, enabled);
        if (!selection.IsEmpty())
            ChangeStringFormat(f, true, false);
        else
            current_string_format = f;
    }
}

void Document::SetCurrentParagraphFormat(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    current_paragraph_format = paragraph_formats->GetFormat(name);
    if (current_paragraph_format)
    {
        ChangeParagraphFormat(current_paragraph_format, true, false);
    }
}

ElementType Document::GetElementType(const ElementId id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
        return ElementType::NONE;
    return el->type;
}

bool Document::IsString(ElementPtr el)
{
    return el && (el->type == ElementType::STRING || el->type == ElementType::CODE_STRING);
}

bool Document::IsString(ElementId id)
{
    auto el = GetElement(id);
    return IsString(el);
}

bool Document::IsRow(ElementPtr el)
{
    return el && (el->type == ElementType::ROW || el->type == ElementType::CODE_ROW);
}

bool Document::IsRow(ElementId id)
{
    auto el = GetElement(id);
    return IsRow(el);
}

bool Document::GetStringFormat(const ElementId id, StringFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    ElementPtr el = GetElement(id);
    if (IsString(el))
    {
        format = *((String*)el.get())->format;
        return true;
    }
    else if (IsRow(el))
    {
        el = FindParent(el->id, ElementType::PARAGRAPH);
        if (el)
        {
            format = *((Paragraph*)el.get())->format->string_format;
            return true;
        }
    }
    return false;
}

bool Document::GetParagraphFormat(const ElementId id, ParagraphFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    auto el = FindParent(id, ElementType::PARAGRAPH);
    if (!el)
        return false;
    format = *((Paragraph*)el.get())->format;
    return true;
}

void Document::MoveCaret(MoveCaretTask::MoveCaretDir dir, bool select)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, dir, true, select));
    }
    next_circle.notify_one();

#ifdef DEBUG
    last_caret_moved = false;
#endif
}

void Document::MoveCaretLeft(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::LEFT, select);
}

void Document::MoveCaretRight(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::RIGHT, select);
}

void Document::MoveCaretUp(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::UP, select);
}

void Document::MoveCaretDown(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::DOWN, select);
}

void Document::MoveCaretHome(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::HOME, select);
}

void Document::MoveCaretEnd(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::END, select);
}

void Document::MoveCaretWordLeft(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::WORD_LEFT, select);
}

void Document::MoveCaretWordRight(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::WORD_RIGHT, select);
}

void Document::MoveCaretToDocumentBegin(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_BEGIN, select);
}

void Document::MoveCaretToDocumentEnd(bool select)
{
    MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_END, select);
}

void Document::MoveCaret(const int x, const int y)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, Point{x, y}));
    }
    next_circle.notify_one();

#ifdef DEBUG
    last_caret_moved = false;
#endif
}

void Document::SetCaretVisible(bool visible)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, MoveCaretTask::MoveCaretDir::NONE, visible));
    }
    next_circle.notify_one();
}

void Document::Undo()
{
    if (!CanUndo())
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        undos.push_back(true);
    }
    next_circle.notify_one();
}

void Document::Redo()
{
    if (!CanRedo())
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        redos.push_back(true);
    }
    next_circle.notify_one();
}

bool Document::CanUndo()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return !undo_tasks.empty();
}

bool Document::CanRedo()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    uint last_undo_task_id = 0;
    if (!undo_tasks.empty())
        last_undo_task_id = undo_tasks.top()->id;
    
    int i = 0;
    for (i = redo_tasks.size() - 1; i >=0; --i)
    {
        if (redo_tasks[i]->id == last_undo_task_id)
            break;
    }

    if (++i < redo_tasks.size())
    {
        uint redo_task_id = redo_tasks[i]->id;
        if (i < redo_tasks.size() && redo_tasks[i]->id == redo_task_id)
            return true;
    }
    return false;
}

void Document::RollbackUndo()
{
    //remove last undo tasks with one id
    if (undo_tasks.empty())
        return;
    TaskPtr t = undo_tasks.top();
    uint id = t->id;
    while (id == t->id)
    {
        undo_tasks.pop();
        if (undo_tasks.empty())
            break;
        t = undo_tasks.top();
    }
}

void Document::Resize(uint width, uint height)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ResizeTask(text, width, height));
    }
#ifdef DEBUG
    last_task_id = tasks[tasks.size() - 1]->id;
#endif
    next_circle.notify_one();
    Remake(text->id, true, false, false);
}

void Document::Redraw(const ElementId& id, bool move_into_view)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (!tasks.empty())
        {
            if (!WillRedraw(id, move_into_view))
            {
                TaskPtr last = tasks[tasks.size() - 1];
                RedrawTask* t = dynamic_cast<RedrawTask*>(last.get());
                if (!t || t->element_id != id)
                    tasks.emplace_back(new RedrawTask(text, id, move_into_view));
                else if (!t->move_into_view)
                    t->move_into_view = move_into_view;
            }
        }
        else
        {
            tasks.emplace_back(new RedrawTask(text, id, move_into_view));
        }
    }
    next_circle.notify_one();
}

void Document::Redraw()
{
    Redraw(text->id, false);
}

void Document::Remake(const ElementId& id, bool with_elements, bool with_undo, bool undo, bool move_into_view)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            undo_tasks.push(TaskPtr(new RemakeTask(text, id, with_elements, with_undo, move_into_view, cur_task_id)));
        }
        else
        {
            if (!tasks.empty())
            {
                TaskPtr last = tasks[tasks.size() - 1];
                RemakeTask* t = dynamic_cast<RemakeTask*>(last.get());
                if (!t || t->element_id != id || t->with_elements != with_elements)
                    tasks.emplace_back(new RemakeTask(text, id, with_elements, with_undo, move_into_view, cur_task_id));
            }
            else
            {
                tasks.emplace_back(new RemakeTask(text, id, with_elements, with_undo, move_into_view, cur_task_id));
            }
        }
    }
    next_circle.notify_one();
}

bool Document::WillRedraw(const ElementId& id, bool move_into_view)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    for (int i = tasks.size() - 1; i >= 0; --i)
    {
        TaskPtr t = tasks[i];
        RedrawTask* redraw_task = dynamic_cast<RedrawTask*>(t.get());
        if (redraw_task && IsChild(redraw_task->element_id, id))
        {
            if (!redraw_task->move_into_view)
                redraw_task->move_into_view = move_into_view;
            return true;
        }
        RemakeTask* remake_task = dynamic_cast<RemakeTask*>(t.get());
        if (remake_task && IsChild(remake_task->element_id, id))
        {
            if (remake_task->move_into_view == move_into_view)
                return true;
        }
    }
    return false;
}

uint Document::New()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new NewTask(text));
#ifdef DEBUG
    last_task_id = tasks[tasks.size() - 1]->id;
#endif
    return tasks[tasks.size() - 1]->id;
}

uint Document::Save(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SaveTask(text, filename));
#ifdef DEBUG
    last_task_id = tasks[tasks.size() - 1]->id;
#endif
    return tasks[tasks.size() - 1]->id;
}

uint Document::Load(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new LoadTask(text, filename));
#ifdef DEBUG
    last_load_task_id = tasks[tasks.size() - 1]->id;
#endif
    return tasks[tasks.size() - 1]->id;
}

void Document::Copy(std::stringstream& out_array, std::string& out_text)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new CopyTask(text, out_array, out_text, false));
#ifdef DEBUG
    last_task_id = tasks[tasks.size() - 1]->id;
#endif
}

void Document::Paste(std::stringstream& in_array)
{
    DocumentUserData user_data{this};
    UserDataAdapter<DocumentUserData, boost::archive::binary_iarchive> iarchive(user_data, in_array);
    std::vector<ElementPtr> elements;
    RegisterTypes(iarchive);
    
    try
    {
        iarchive >> elements;
    }
    catch (boost::archive::archive_exception& ex)
    {
        window->OnPasteResult(PasteResult::PasteError);
        return;
    }

    if (!elements.empty())
        InsertElements(elements, true);
    window->OnPasteResult(PasteResult::Success);
}

void Document::Paste(const std::string& str)
{
    if (str.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return;
    }

    InsertString(str, true);
    window->OnPasteResult(PasteResult::Success);
}

void Document::Cut(std::stringstream& out_array, std::string& out_text)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new CopyTask(text, out_array, out_text, true));
#ifdef DEBUG
    last_task_id = tasks[tasks.size() - 1]->id;
#endif
}

std::string Document::ToHtml()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return text->ToHtml();
}

std::string Document::ToText()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return text->ToText();
}

TextFormatPtr Document::GetDefaultTextFormat()
{
    return TextFormats::GetFormat(TextFormat::Paging::ONE_PAGE);
}

PageFormatPtr Document::GetDefaultPageFormat()
{
    return PageFormats::GetFormat(20, 20, 20, 20, 10);
}

StringFormatPtr Document::GetStringFormat(const std::string family, uint size, bool bold, bool italic, bool underline)
{
    return string_formats->GetFormat(family, size, bold, italic, underline);
}

StringFormatPtr Document::GetStringFormat(const uint id)
{
    return string_formats->GetFormat(id);
}

EditorState Document::GetEditorState()
{
    return {caret->GetCaretState(), selection.GetState()};
}

void Document::SetEditorState(EditorState& state)
{
    caret->SetState(state.caret_state);
    selection.Set(state.selection_state);
}

void Document::Solve(ElementId _id, ExpressionType expression_type, ResultType result_type, const uint precision, AngleMeasure angle_measure, 
    Notation notation, const std::string& expression)
{
    solver.Solve(_id, expression_type, result_type, precision, angle_measure, notation, expression);
}

void Document::PutResult(ElementId _id, Result result)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResultTask(text, _id, result));
#ifdef DEBUG
    last_solver_task_id = tasks[tasks.size() - 1]->id;
#endif
}

#ifdef DEBUG
void Document::WaitMainLoop()
{
    while (!last_task_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_task_id = 0;
    last_task_executed = false;
}

void Document::WaitUndo()
{
    while (!last_undo_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_undo_executed = false;
}

void Document::WaitRedo()
{
    while (!last_redo_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

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

void Document::WaitLoad()
{
    while (!last_load_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    last_load_task_id = 0;
    last_load_executed = false;
}

void Document::WaitSolver()
{
    while (!last_solver_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    last_solver_task_id = 0;
    last_solver_executed = false;
}

void Document::WaitTask(uint task_id)
{
    while (true)
    {
        std::this_thread::sleep_for(100ms);

        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (std::find(last_tasks.begin(), last_tasks.end(), task_id) != last_tasks.end())
            return;
        last_tasks.clear();
    }
}
#endif

void Document::UpdateCaretView()
{
    Element* element = caret->GetElement();
    if (!element)
        return;
    Rect r = element->GetAbsoluteRect(element->GetCaretRect(caret->GetPos()));
    Rect view_port = window->GetViewPort(0);
    Point p = window->GetDocumentPoint();

    if (r.height > view_port.height || r.width > view_port.width)
        return;
    
    //move view port in the view if the caret is outside of it
    if (r.left < p.x + view_port.left)
    {
        caret->Hide(); //caret will be shown on Redraw
        window->MoveDocument(r.left - view_port.left - 1, p.y);
        Redraw(text->id, false);
    }
    else if (r.GetRight() > view_port.GetRight() + p.x)
    {
        caret->Hide();
        window->MoveDocument(r.GetRight() - view_port.GetRight(), p.y);
        Redraw(text->id, false);
    }

    if (r.top < p.y + view_port.top)
    {
        caret->Hide();
        window->MoveDocument(p.x, r.top - view_port.top - 1);
        Redraw(text->id, false);
    }
    else if (r.GetBottom() > view_port.GetBottom() + p.y)
    {
        caret->Hide();
        window->MoveDocument(p.x, r.GetBottom() - view_port.GetBottom());
        Redraw(text->id, false);
    }
}

void Document::UpdateLastSelection()
{
    if (selection != last_selection)
    {
        for (auto& s : selection.selection)
            Redraw(s.element->id, false);
        for (auto& s : last_selection.selection)
            Redraw(s.element->id, false);
        last_selection = selection;
    }
}

}
