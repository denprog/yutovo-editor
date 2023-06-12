#include "document.h"
#include "str.h"
#include "paragraph.h"
#include "row.h"
#include "formulas/code_block.h"
#include "formulas/code_string.h"
#include "formulas/plus.h"
#include "formulas/minus.h"
#include "formulas/multiply.h"
#include "formulas/division.h"
#include "formulas/power.h"
#include "formulas/nth_root.h"
#include "formulas/square_root.h"
#include "formulas/equation.h"
#include "formulas/fences.h"
#include "formulas/assignment.h"
#include "formulas/subscript.h"
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
    selection(this),
    last_selection(this),
    solver(this),
    undo_base(this),
    logger(Logger::GetInstance(".", "yutovo", true, true))
{
    string_formats.reset(new StringFormats());
    paragraph_formats.reset(new ParagraphFormats(string_formats));
    code_formats.reset(new CodeFormats());
    formula_formats.reset(new FormulaFormats(string_formats));
    current_paragraph_format = paragraph_formats->GetFormat("Text body");
    current_code_format = code_formats->GetFormat("Calculator");
    current_formula_format = formula_formats->GetFormat("Code");

    logger->Debug("Document start");
}

Document::~Document()
{
    exit = true;
    next_circle = true;
    main_loop.join();
    logger->Debug("Document end");
}

void Document::Start(Config& _config)
{
    config = _config;

    window->Init();

    caret.reset(new Caret(this));
    text.reset(new Text(this));

    caret->MoveToDocumentBegin(nullptr);

    main_loop = std::thread(&Document::MainLoop, this);

    text->Remake(true);
    Redraw(text->id, false);
}

void Document::GetConfig(Config& _config)
{
    std::unique_lock<std::recursive_mutex> lock(tasks_mutex);
    _config = config;
}

void Document::SetConfig(const Config& _config)
{
    std::unique_lock<std::recursive_mutex> lock(tasks_mutex);
    config = _config;
}

void Document::MainLoop()
{
    std::vector<TaskPtr> temp_tasks;

    while (!exit)
    {
        {
            bool empty = false;
            {
                std::unique_lock<std::recursive_mutex> lock(tasks_mutex);
                empty = tasks.empty() && undos.empty() && redos.empty();
            }
            if (empty)
            {
                auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                auto next = now;
                while (!next_circle && next - now < config.caret_blink_delay * 1ms) //wait for tasks
                {
                    std::this_thread::sleep_for(1ms);
                    next = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                }
                if (!next_circle)
                {
                    caret->Blink();
                    continue;
                }
                next_circle = false;
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
                        TaskPtr t = undo_tasks.back();
                        uint id = t->id;
                        while (id == t->id)
                        {
                            temp_undo_tasks.push_back(t);
                            undo_tasks.pop_back();
                            if (undo_tasks.empty())
                                break;
                            t = undo_tasks.back();
                        }
                    }
                    undos.clear();
                }
            }
            if (!temp_undo_tasks.empty())
            {
                std::lock_guard<std::recursive_mutex> lock(edit_mutex);
                caret->Hide(); //caret will be shown on Redraw or caret moving
                selection.can_optimize = false;
                for (size_t i = 0; i < temp_undo_tasks.size(); ++i)
                {
                    changed_elements.clear();
                    resolve_elements.clear();
                    TaskPtr& t = temp_undo_tasks[i];
                    if (!t->Execute())
                        break;
                }
                selection.can_optimize = true;
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
                        last_undo_task_id = undo_tasks.back()->id;
                    
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
                std::lock_guard<std::recursive_mutex> lock(edit_mutex);
                caret->Hide();
                for (size_t i = 0; i < temp_redo_tasks.size(); ++i)
                {
                    changed_elements.clear();
                    resolve_elements.clear();
                    TaskPtr& t = temp_redo_tasks[i];
                    cur_task_id = t->id;
                    if (!t->Execute())
                        break;
                }
                caret->Show();
#ifdef DEBUG
                last_redo_executed = true;
#endif
            }
        }

        {
            std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
            for (auto it = tasks.begin(); it != tasks.end();) //firstly get elements with high priority
            {
                TaskPtr& t = *it;
                if (t->priority == 1)
                {
                    temp_tasks.push_back(t);
                    it = tasks.erase(it);
                }
                else
                    ++it;
            }
            if (temp_tasks.empty() && !tasks.empty()) //if there are no high priority tasks, get first element with low priority
            {
                temp_tasks.push_back(*tasks.begin());
                tasks.erase(tasks.begin());
            }
        }

        if (!temp_tasks.empty())
        {
            std::lock_guard<std::recursive_mutex> lock(edit_mutex);
            caret->Hide();
            //execute all the tasks
            for (size_t i = 0; i < temp_tasks.size(); ++i)
            {
                TaskPtr& t = temp_tasks[i];
                cur_task_id = t->id;
                uint last_undo_task_id = 0;
                if (!undo_tasks.empty())
                    last_undo_task_id = undo_tasks.back()->id;
                
                changed_elements.clear();
                resolve_elements.clear();
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
                
                if (last_tasks.size() > 1000)
                    last_tasks.clear();
                last_tasks.push_back(t->id);
#endif
            }

            caret->Show();

            temp_tasks.clear();
        }
    }
}

uint Document::InsertParagraph(bool with_undo, bool undo)
{
    return InsertElement(new Paragraph(this), with_undo, undo);
}

uint Document::InsertString(const std::string& str, bool with_undo)
{
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new String(this, str, format), with_undo);
    return 0;
}

uint Document::InsertString(const std::u32string& str, bool with_undo)
{
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new String(this, str, format), with_undo);
    return 0;
}

uint Document::InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo)
{
    return InsertElement(new String(this, str, string_format), with_undo);
}

uint Document::InsertString(const std::string& str, const StringFormatPtr string_format, ElementId element_id)
{
    return InsertElement(new String(this, str, string_format), element_id);
}

uint Document::InsertElement(Element* element, bool with_undo, bool undo, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    return InsertElements(elements, with_undo, undo, element_id);
}

uint Document::InsertElement(Element* element, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    return InsertElements(elements, false, true, element_id);
}

uint Document::InsertElement(ElementPtr element, ElementId element_id)
{
    std::vector<ElementPtr> elements;
    elements.push_back(element);
    return InsertElements(elements, false, true, element_id);
}

uint Document::InsertElements(std::vector<ElementPtr>& elements, bool with_undo, bool undo, ElementId element_id, bool pasting)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new InsertElementsTask(text, elements, cur_task_id, element_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new InsertElementsTask(text, elements, with_undo, pasting));
            last_task_id = tasks.back()->id;
        }
    }
    next_circle = true;
    return last_task_id;
}

uint Document::DeleteElements(bool left, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new DeleteElementsTask(text, left, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new DeleteElementsTask(text, left, with_undo));
            last_task_id = tasks.back()->id;
        }
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ClearElements(ElementId element_id, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new DeleteElementsTask(text, element_id, with_undo, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new DeleteElementsTask(text, element_id, with_undo));
            last_task_id = tasks.back()->id;
        }
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertCode(bool next_code_id, bool with_undo)
{
    if (next_code_id)
        ++cur_code_id;
    return InsertFormula(new CodeBlock(this, cur_code_id), with_undo, false);
}

uint Document::InsertCodeString(const std::string& str, bool with_undo)
{
    FormulaFormatPtr format;
    if (GetCurrentFormulaFormat(format))
        return InsertFormula(new CodeString(this, str, format->string_format), with_undo, false);
    return 0;
}

uint Document::InsertPlus(bool with_undo)
{
    return InsertFormula(new Plus(this), with_undo, false);
}

uint Document::InsertMinus(bool with_undo)
{
    return InsertFormula(new Minus(this), with_undo, false);
}

uint Document::InsertMultiply(bool with_undo)
{
    return InsertFormula(new Multiply(this), with_undo, false);
}

uint Document::InsertDivision(bool with_undo)
{
    return InsertFormula(new Division(this), with_undo, false);
}

uint Document::InsertPower(bool with_undo)
{
    return InsertFormula(new Power(this), with_undo, false);
}

uint Document::InsertNthRoot(bool with_undo)
{
    return InsertFormula(new NthRoot(this), with_undo, false);
}

uint Document::InsertSquareRoot(bool with_undo)
{
    return InsertFormula(new SquareRoot(this), with_undo, false);
}

uint Document::InsertEquation(yutovo_service::ResultType result_type, bool with_undo)
{
    return InsertFormula(new Equation(this, result_type), with_undo, false);
}

uint Document::InsertOpenFence(bool with_undo)
{
    return InsertFormula(new OpenFence(this), with_undo, false);
}

uint Document::InsertCloseFence(bool with_undo)
{
    return InsertFormula(new CloseFence(this), with_undo, false);
}

uint Document::InsertAssignment(bool with_undo)
{
    return InsertFormula(new Assignment(this), with_undo, false);
}

uint Document::InsertSubscript(bool with_undo)
{
    return InsertFormula(new Subscript(this), with_undo, false);
}

uint Document::InsertFences(bool with_undo)
{
    InsertFormula(new OpenFence(this), with_undo, false, false);
    uint r = InsertFormula(new CloseFence(this), with_undo, false, true);
    MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertFunction(const std::string& name, bool with_undo)
{
    InsertCodeString(name, true);
    InsertFormula(new OpenFence(this), with_undo, false, true);
    uint r = InsertFormula(new CloseFence(this), with_undo, false, true);
    MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertSubscriptFunction(const std::string& name, bool with_undo)
{
    InsertCodeString(name, true);
    return InsertFormula(new Subscript(this), with_undo, false, true);
}

uint Document::InsertFormula(Element* element, bool with_undo, bool undo, bool with_last_task_id)
{
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    return InsertFormulas(elements, with_undo, undo, with_last_task_id);
}

uint Document::InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool undo, bool with_last_task_id, bool pasting)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new InsertFormulasTask(text, cur_task_id, elements, false)));
            last_task_id = cur_task_id;
        }
        else
        {
            if (with_last_task_id)
                tasks.emplace_back(new InsertFormulasTask(text, last_task_id, elements, with_undo));
            else
                tasks.emplace_back(new InsertFormulasTask(text, elements, with_undo, pasting));
            last_task_id = tasks.back()->id;
        }
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, 
    bool with_undo, bool undo)
{
    return ChangeStringFormat(string_formats->GetFormat(family, size, bold, italic, underline), with_undo, undo);
}

uint Document::ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
    bool with_undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangeStringFormatTask(text, format, set_family, set_size, set_bold, set_italic, set_underline, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeStringFormat(const StringFormatPtr format, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new ChangeStringFormatTask(text, format, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new ChangeStringFormatTask(text, format, with_undo));
            last_task_id = tasks.back()->id;
        }
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, bool undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (undo)
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new ChangeParagraphFormatTask(text, format, cur_task_id)));
            last_task_id = cur_task_id;
        }
        else
        {
            tasks.emplace_back(new ChangeParagraphFormatTask(text, format, with_undo));
            last_task_id = tasks.back()->id;
        }
    }
    next_circle = true;
    return last_task_id;
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
        {
            RestrictUndo();
            undo_tasks.push_back(TaskPtr(new SetEditorStateTask(text, caret_state, selection_state, cur_task_id)));
        }
        else
            tasks.emplace_back(new SetEditorStateTask(text, caret_state, selection_state, cur_task_id));
    }
    next_circle = true;
}

bool Document::StoreUndo(const ElementId& _id)
{
    if (_id.size() == 1)
        return StoreUndo(_id, 0, text->elements->Count());
    RestrictUndo();

    int undo_id = undo_base.Store(_id);
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, _id, 0, cur_task_id)));
    return true;
}

bool Document::StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size)
{
    RestrictUndo();

    int undo_id;
    ElementId _id;
    if (IsRow(parent_id))
    {
        auto p = GetParent(parent_id);
        undo_id = undo_base.Store(p->id, 0, p->elements->Count());
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->id, p->elements->Count(), cur_task_id)));
        return true;
    }
    else
    {
        undo_id = undo_base.Store(parent_id, pos, size);
        _id = yutovo::GetChild(parent_id, pos);
    }
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, _id, delete_size, cur_task_id)));
    return true;
}

bool Document::StoreUndo(const ElementId& parent_id, const int pos, const int size, UndoTask::UndoOperation undo_operation)
{
    RestrictUndo();

    int undo_id;
    ElementId _id;
    auto p = GetParent(parent_id);
    if (IsRow(parent_id))
    {
        undo_id = undo_base.Store(p->id, 0, p->elements->Count());
        _id = yutovo::GetChild(p->id, 0);
    }
    else if (IsParagraph(parent_id))
    {
        int _pos = p->elements->GetChildPos(parent_id);
        undo_id = undo_base.Store(p->id, _pos, 1);
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->id, _pos, 1, UndoTask::UndoOperation::CHANGE, cur_task_id)));
        return true;
    }
    else
    {
        undo_id = undo_base.Store(parent_id, pos, size);
        _id = yutovo::GetChild(parent_id, pos);
    }
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, parent_id, pos, size, undo_operation, cur_task_id)));
    return true;
}

bool Document::RestoreUndo(const int undo_id, std::vector<ElementPtr>& elements)
{
    return undo_base.Restore(undo_id, elements);
}

void Document::ResetTasks()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.clear();
    undo_tasks.clear();
    redo_tasks.clear();
}

ElementPtr Document::GetElement(const ElementId& _id)
{
    if (_id.empty())
        return nullptr;
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

ElementPtr Document::GetLogicalElement(const LogicalId& _id)
{
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    if (elements.empty())
        return nullptr;
    return elements[0];
}

void Document::GetElements(const LogicalId& _id, std::vector<ElementPtr>& elements)
{
    if (_id.empty())
        return;
    if (_id.size() == 1)
    {
        elements.push_back(text);
        return;
    }
    ElementPtr el = text->elements->Get(_id[1]);
    if (_id.size() == 2)
    {
        elements.push_back(el);
        return;
    }
    
    auto find_in_paragraph = 
        [_id](ElementPtr el, int pos)
        {
            ElementPtr res;
            LogicalId part_id(_id.begin(), _id.begin() + pos);

            for (size_t i = 0; i < el->elements->Count(); ++i)
            {
                auto row = el->elements->Get(i);
                if (row->elements->Count() == 0)
                    continue;
                if (row->elements->Get(0)->logical_id == part_id)
                {
                    res = row->elements->Get(0);
                    break;
                }
                if (row->elements->Get(row->elements->Count() - 1)->logical_id[2] >= part_id[2])
                {
                    for (size_t j = 1; j < row->elements->Count(); ++j)
                    {
                        auto ch = row->elements->Get(j);
                        if (ch->logical_id == part_id)
                        {
                            res = ch;
                            break;
                        }
                    }
                    break;
                }
            }
            return res;
        };
    
    el = find_in_paragraph(el, 3);
    if (!el)
        return;
    
    assert(!IsParagraph(el));

    for (uint i = 3; i < _id.size(); ++i)
    {
        if (!el || el->elements->Count() < _id[i])
            return;
        if (_id[i] > 0 && _id[i] == el->elements->Count())
        {
            auto ch = el->elements->Get(_id[i - 1]);
            if (!ch->HasLastCaretState())
                return;
            el = el->elements->Get(_id[i - 1]);
        }
        else
            el = el->elements->Get(_id[i]);
        if (el->type == ElementType::PARAGRAPH)
        {
            el = find_in_paragraph(el, i);
            ++i;
        }
    }

    elements.push_back(el);

    int pos = GetChildPos(el->id);
    bool found = true;
    auto p = el->parent;

    if (IsParagraph(p->parent->id))
    {
        for (int i = pos - 1; i >= 0; --i) //look for the same logical ids backward
        {
            auto ch = p->elements->Get(i);
            if (ch->logical_id != el->logical_id)
            {
                found = false;
                break;
            }
            elements.insert(elements.begin(), ch);
        }
        if (found)
        {
            int p_pos = GetChildPos(p->id);
            for (int i = p_pos - 1; i >= 0; --i)
            {
                auto r = p->parent->elements->Get(i);
                for (int j = r->elements->Count() - 1; j >= 0; --j)
                {
                    auto ch = r->elements->Get(j);
                    if (ch->logical_id != el->logical_id)
                    {
                        found = false;
                        break;
                    }
                    elements.insert(elements.begin(), ch);
                }
                if (!found)
                    break;
            }
        }
    }

    pos = GetChildPos(el->id) + 1;
    found = true;
    if (IsParagraph(p->parent->id))
    {
        for (int i = pos; i < p->elements->Count(); ++i) //look for the same logical ids forward
        {
            auto ch = p->elements->Get(i);
            if (ch->logical_id != el->logical_id)
            {
                found = false;
                break;
            }
            elements.push_back(ch);
        }
        if (!found)
            return;

        int p_pos = GetChildPos(p->id);
        for (int i = p_pos + 1; i < p->parent->elements->Count(); ++i)
        {
            auto r = p->parent->elements->Get(i);
            for (int j = 0; j < r->elements->Count(); ++j)
            {
                auto ch = r->elements->Get(j);
                if (ch->logical_id != el->logical_id)
                {
                    found = false;
                    break;
                }
                elements.push_back(ch);
            }
            if (!found)
                break;
        }
    }
    else
    {
        while (pos + 1 < p->elements->Count())
        {
            auto ch = p->elements->Get(pos + 1);
            if (ch->logical_id != el->logical_id)
                break;
            elements.push_back(ch);
        }
    }
}

ElementPtr Document::GetParent(const ElementId& _id)
{
    if (_id.size() == 1 || _id.empty())
        return nullptr;
    if (_id.size() == 2)
        return text;
    ElementPtr el = text->elements->Get(_id[1]);
    for (uint i = 2; i < _id.size() - 1; ++i)
    {
        if (!el || el->elements->Count() <= _id[i])
            return nullptr;
        el = el->elements->Get(_id[i]);
    }
    return el;
}

ElementPtr Document::GetLogicalParent(const LogicalId& _id)
{
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    if (elements.empty())
        return nullptr;
    return GetElement(elements[0]->parent->id);
}

bool Document::GetElementAtCoords(const int x, const int y, ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return text->GetElementAtCoords(x, y, id);
}

LogicalId Document::GetLogicalId(const ElementId& _id)
{
    if (_id.size() <= 2)
        return _id;
    return GetLogicalId(yutovo::GetParent(_id), yutovo::GetChildPos(_id));
}

LogicalId Document::GetLogicalId(const ElementId& _id, const int pos)
{
    LogicalId res;
    auto el = GetElement(_id);
    if (IsString(el))
    {
        int _pos = pos;
        auto r = el->parent;
        auto p = r->parent;
        int r_p = GetChildPos(el->id);
        for (int i = r_p - 1; i >= 0; --i)
        {
            auto ch = r->elements->Get(i);
            if (ch->logical_id == el->logical_id)
                _pos += ch->elements->Count();
            else
                break;
        }
        int p_p = GetChildPos(p->id, r->id);
        for (int i = p_p - 1; i >= 0; --i)
        {
            auto r = p->elements->Get(i);
            for (int j = r->elements->Count() - 1; j >= 0; --j)
            {
                auto ch = r->elements->Get(j);
                if (ch->logical_id == el->logical_id)
                    _pos += ch->elements->Count();
                else
                {
                    res = el->logical_id;
                    res.push_back(_pos);
                    return res;
                }
            }
        }
        res = el->logical_id;
        res.push_back(_pos);
        return res;
    }
    else if (el->type == ElementType::PARAGRAPH)
    {
        return el->logical_id;
    }

    if (pos > 0 && pos == el->elements->Count() && el->elements->Get(el->elements->Count() - 1)->HasLastCaretState())
    {
        res = GetElement(GetChild(_id, pos - 1))->logical_id;
        ++res[res.size() - 1];
        return res;
    }
    return GetElement(GetChild(_id, pos))->logical_id;
}

ElementId Document::GetElementId(const LogicalId& _id, bool& last_pos)
{
    return GetElementId(yutovo::GetParent(_id), yutovo::GetChildPos(_id), last_pos);
}

ElementId Document::GetElementId(const LogicalId& _id, const int pos, bool& last_pos)
{
    ElementId res;
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    int p = 0;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        auto& _el = elements[i];
        res = _el->id;
        if (_el->type == ElementType::PARAGRAPH)
        {
            LogicalId last_id;
            for (size_t j = 0; j < _el->elements->Count(); ++j)
            {
                auto r = _el->elements->Get(j);
                for (int k = 0; k < r->elements->Count(); ++k)
                {
                    auto ch = r->elements->Get(k);
                    if (last_id.empty())
                        last_id = ch->logical_id;
                    if (pos - p <= r->elements->Count())
                    {
                        res = r->id;
                        res.push_back(pos - p);
                        if (pos - p == r->elements->Count())
                            last_pos = true;
                        break;
                    }
                    if (ch->logical_id != last_id)
                    {
                        last_id = ch->logical_id;
                        ++p;
                    }
                }
            }
        }
        else if (pos - p <= _el->elements->Count())
        {
            res.push_back(pos - p);
            break;
        }
        p += _el->elements->Count();
    }
    return res;
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

ElementId Document::FindCurrentParentByType(const ElementType type)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    auto el = caret->GetElement();
    if (!el)
        return ElementId{};
    auto p = FindParent(el->id, type);
    return p ? p->id : ElementId{};
}

ElementPtr Document::FindParentParagraph(const ElementId& id)
{
    ElementPtr el = GetElement(id);
    if (el && (el->type == ElementType::PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH))
        return el;
    el = GetParent(id);
    while (el && el->type != ElementType::PARAGRAPH && el->type != ElementType::CODE_PARAGRAPH)
        el = GetParent(el->id);
    return el;
}

ElementPtr Document::FindParentRow(const ElementId& id)
{
    ElementPtr el = GetElement(id);
    if (el && (el->type == ElementType::ROW || el->type == ElementType::CODE_ROW))
        return el;
    el = GetParent(id);
    while (el && el->type != ElementType::ROW && el->type != ElementType::CODE_ROW)
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
    if (GetStringFormat(c.id, f))
    {
        current_string_format = string_formats->GetFormat(f);
        window->OnFormatChanged(GetEditorState());
    }
}

uint Document::SetFontFamily(const std::string& family)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        if (!selection.IsEmpty())
        {
            return ChangeStringFormat(string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline), true, false);
        }
        else
        {
            current_string_format = string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline);
        }
    }
    return 0;
}

uint Document::SetFontSize(const uint size)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, size, current_string_format->bold, current_string_format->italic, 
            current_string_format->underline);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, true, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetBold(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, enabled, current_string_format->italic, 
            current_string_format->underline);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, true, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetItalic(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, enabled, 
            current_string_format->underline);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, true, false);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetUnderline(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, enabled);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, true, false);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetCurrentParagraphFormat(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    current_paragraph_format = paragraph_formats->GetFormat(name);
    if (current_paragraph_format)
        return ChangeParagraphFormat(current_paragraph_format, true, false);
    return 0;
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

bool Document::IsParagraph(ElementPtr el)
{
    return el && (el->type == ElementType::PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH);
}

bool Document::IsParagraph(ElementId id)
{
    auto el = GetElement(id);
    return IsParagraph(el);
}

bool Document::IsFormula(ElementPtr el)
{
    return dynamic_cast<Formula*>(el.get());
}

bool Document::GetStringFormat(const ElementId id, StringFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    ElementPtr p = GetParent(id);
    if (IsString(el))
    {
        auto f = el->GetStringFormat();
        format = *f;
        return true;
    }
    else if (IsString(p))
    {
        auto f = p->GetStringFormat();
        format = *f;
        return true;
    }

    el = FindParentRow(id);
    if (el)
    {
        CaretState c;
        if (el->GetLastCaretState(c, nullptr) && c.id == id)
        {
            //find previous string format
            for (int i = el->elements->Count() - 1; i >= 0; --i)
            {
                auto s = el->elements->Get(i);
                if (IsString(s))
                {
                    auto f = s->GetStringFormat();
                    format = *f;
                    return true;
                }
            }
        }
        else if (el->GetFirstCaretState(c, nullptr) && c.id == id)
        {
            //find next string format
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                auto s = el->elements->Get(i);
                if (IsString(s))
                {
                    auto f = s->GetStringFormat();
                    format = *f;
                    return true;
                }
            }
        }

        auto f = el->GetStringFormat();
        format = *f;
        return true;
    }
    return false;
}

bool Document::GetParagraphFormat(const ElementId id, ParagraphFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    auto el = GetParent(id);
    if (!el)
        return false;
    ParagraphFormatPtr p = el->GetParagraphFormat();
    if (!p)
        return false;
    format = *p;
    return true;
}

uint Document::MoveCaret(MoveCaretTask::MoveCaretDir dir, bool select, bool with_last_task_id, bool move_into_view)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        MoveCaretTask* t;
        if (with_last_task_id)
            t = new MoveCaretTask(text, caret, dir, true, select, last_task_id);
        else
            t = new MoveCaretTask(text, caret, dir, true, select);
        t->move_into_view = move_into_view;
        tasks.emplace_back(t);
        last_task_id = tasks.back()->id;
    }
    next_circle = true;

#ifdef DEBUG
    last_caret_moved = false;
#endif
    return last_task_id;
}

uint Document::MoveCaretLeft(bool select, bool with_last_task_id)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::LEFT, select, with_last_task_id);
}

uint Document::MoveCaretRight(bool select, bool with_last_task_id)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::RIGHT, select, with_last_task_id);
}

uint Document::MoveCaretUp(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::UP, select);
}

uint Document::MoveCaretDown(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOWN, select);
}

uint Document::MoveCaretHome(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::HOME, select);
}

uint Document::MoveCaretEnd(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::END, select);
}

uint Document::MoveCaretPageUp(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::PAGE_UP, select);
}

uint Document::MoveCaretPageDown(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::PAGE_DOWN, select);
}

uint Document::MoveCaretWordLeft(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::WORD_LEFT, select);
}

uint Document::MoveCaretWordRight(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::WORD_RIGHT, select);
}

uint Document::MoveCaretToDocumentBegin(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_BEGIN, select);
}

uint Document::MoveCaretToDocumentEnd(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_END, select);
}

uint Document::MoveCaretToDocumentEnd(bool select, bool move_into_view)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_END, select, false, move_into_view);
}

uint Document::MoveCaret(const int x, const int y)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, Point{x, y}));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
#ifdef DEBUG
    last_caret_moved = false;
#endif
    return last_task_id;
}

uint Document::SelectAll()
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::SELECT_ALL, true, false, false);
}

void Document::SetCaretVisible(bool visible)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, MoveCaretTask::MoveCaretDir::NONE, visible));
    }
    next_circle = true;
}

void Document::Undo()
{
    if (!CanUndo())
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        undos.push_back(true);
    }
    next_circle = true;
}

void Document::Redo()
{
    if (!CanRedo())
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        redos.push_back(true);
    }
    next_circle = true;
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
        last_undo_task_id = undo_tasks.back()->id;
    
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
    TaskPtr t = undo_tasks.back();
    uint id = t->id;
    while (id == t->id)
    {
        undo_tasks.pop_back();
        if (undo_tasks.empty())
            break;
        t = undo_tasks.back();
    }
}

uint Document::Resize(uint width, uint height)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ResizeTask(text, width, height));
    }
#ifdef DEBUG
    last_task_id = tasks.back()->id;
#endif
    next_circle = true;
    return last_task_id;
}

void Document::Redraw(const ElementId& id, bool move_into_view)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (!tasks.empty())
        {
            if (!WillRedraw(id, move_into_view))
            {
                TaskPtr last = tasks.back();
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
    next_circle = true;
}

void Document::Redraw()
{
    Redraw(text->id, false);
}

bool Document::WillRedraw(const ElementId& id, bool move_into_view)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    int i = 0;
    for (auto it = tasks.rbegin(); it != tasks.rend() && i < 10; ++it, ++i)
    {
        TaskPtr& t = *it;
        RedrawTask* redraw_task = dynamic_cast<RedrawTask*>(t.get());
        if (redraw_task && IsChild(redraw_task->element_id, id))
        {
            if (!redraw_task->move_into_view)
                redraw_task->move_into_view = move_into_view;
            return true;
        }
    }
    return false;
}

bool Document::WillResize()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    for (auto it = tasks.rbegin(); it != tasks.rend(); ++it)
    {
        TaskPtr& t = *it;
        if (dynamic_cast<ResizeTask*>(t.get()))
            return true;
    }
    return false;
}

uint Document::New()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new NewTask(text));
#ifdef DEBUG
    last_task_id = tasks.back()->id;
#endif
    return tasks.back()->id;
}

uint Document::Save(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SaveTask(text, filename));
#ifdef DEBUG
    last_task_id = tasks.back()->id;
#endif
    return tasks.back()->id;
}

uint Document::Load(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new LoadTask(text, filename));
#ifdef DEBUG
    last_load_task_id = tasks.back()->id;
#endif
    return tasks.back()->id;
}

uint Document::Copy(std::stringstream& out_array, std::u32string& out_text)
{
    out_array.str("");
    out_text = U"";
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new CopyTask(text, out_array, out_text, false));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

uint Document::Paste(std::stringstream& in_array)
{
    DocumentUserData user_data{this};
    UserDataAdapter<DocumentUserData, boost::archive::binary_iarchive> iarchive(user_data, in_array);
    std::vector<ElementPtr> elements;
    RegisterTypes(iarchive);

    StringFormatsPtr _string_formats;

    try
    {
        iarchive >> _string_formats;
        string_formats->AddFormats(*_string_formats);
        iarchive >> elements;
    }
    catch (boost::archive::archive_exception& ex)
    {
        window->OnPasteResult(PasteResult::PasteError);
        return 0;
    }

    if (!elements.empty())
    {
        bool only_formulas = true;
        for (auto& el : elements)
        {
            if (!el->IsFormula())
            {
                only_formulas = false;
                break;
            }
        }
        if (only_formulas)
            InsertFormulas(elements, true, false, false, true);
        else
            InsertElements(elements, true, false, ElementId{}, true);
        window->OnPasteResult(PasteResult::Success);
    }
    else
        window->OnPasteResult(PasteResult::EmptyBuffer);
    return last_task_id;
}

uint Document::Paste(const std::u32string& str)
{
    if (str.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertString(str, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::Cut(std::stringstream& out_array, std::u32string& out_text)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new CopyTask(text, out_array, out_text, true));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

std::string Document::ToHtml()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return text->ToHtml();
}

std::u32string Document::ToText()
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

StringFormatPtr Document::GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline)
{
    return string_formats->GetFormat(family, size, bold, italic, underline);
}

StringFormatPtr Document::GetStringFormat(const boost::uuids::uuid& id)
{
    return string_formats->GetFormat(id);
}

EditorState Document::GetEditorState()
{
    return {caret->GetCaretState(), selection.GetState()};
}

LogicalEditorState Document::GetLogicalEditorState()
{
    return {caret->GetLogicalCaretState(), selection.GetLogicalState()};
}

void Document::SetEditorState(EditorState& state)
{
    caret->SetState(state.caret_state);
    selection.Set(state.selection_state);
}

void Document::SetEditorState(LogicalEditorState& state)
{
    caret->SetState(state.caret_state);
    selection.Set(state.selection_state);
}

void Document::Solve(ElementId _id, uint code_id, Config::AutoResult& config, std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::RealResult& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::IntegerResult& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::RationalResult& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::ComplexResult& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::SetUserIdentifier(ElementId _id, uint code_id, const std::u32string& identifier, const std::u32string& expression, const uint delay)
{
    solver.SetUserIdentifier(_id, code_id, expression + U";", delay);
    ReSolveDependencies(_id, identifier);
}

void Document::RemoveIdentifier(ElementId _id, uint code_id, const std::u32string& identifier, const uint delay)
{
    solver.RemoveIdentifier(_id, code_id, identifier, delay);
    ReSolveDependencies(_id, identifier);
}

uint Document::SetResult(ElementId _id, ResultType result_type, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultTask(text, _id, result_type, with_undo));
#ifdef DEBUG
    last_task_id = tasks.back()->id;
#endif
    return tasks.back()->id;
}

Notation Document::GetNotation(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    auto el = GetElement(_id);
    IntegerResult* r = (IntegerResult*)el.get();
    if (!r)
        return Notation::NONE;
    return r->config.result_notation;
}

uint Document::SetNotation(ElementId _id, Notation notation, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParams(text, _id, notation, with_undo));
#ifdef DEBUG
    last_task_id = tasks.back()->id;
#endif
    return tasks.back()->id;
}

FractionForm Document::GetFractionForm(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    auto el = GetElement(_id);
    RationalResult* r = (RationalResult*)el.get();
    if (!r)
        return FractionForm::NONE;
    return r->config.fraction_form;
}

uint Document::SetFractionForm(ElementId _id, FractionForm fraction_form, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParams(text, _id, fraction_form, with_undo));
#ifdef DEBUG
    last_task_id = tasks.back()->id;
#endif
    return tasks.back()->id;
}

void Document::ReSolve(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResolveTask(text, _id));
}

void Document::ReSolveDependencies(ElementId after_id, const std::u32string& identifier)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResolveDependeciesTask(text, after_id, ToBasicString(identifier)));
}

void Document::ReSolveErrors()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResolveErrorsTask(text));
}

void Document::PutResult(ElementId _id, Result result)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResultTask(text, _id, result));
#ifdef DEBUG
    last_solver_task_id = tasks.back()->id;
#endif
}

void Document::AddResolveElement(ElementId _id)
{
    if (std::find(resolve_elements.begin(), resolve_elements.end(), _id) == resolve_elements.end())
        resolve_elements.push_back(_id);
}

void Document::AddChangedElement(ElementId _id)
{
    if (std::find(changed_elements.begin(), changed_elements.end(), _id) == changed_elements.end())
        changed_elements.push_back(_id);
}

bool Document::IsVisible(ElementId _id)
{
    auto el = GetElement(_id);
    if (!el)
        return false;
    if (el->type == ElementType::TEXT)
        return true;
    
    Rect w = window->GetRect();
    w.left += window->document_point.x;
    w.top += window->document_point.y;
    Rect r = el->GetAbsoluteRect();
    return r.Intersects(w);
}

ElementId Document::GetFirstVisibleParagraph()
{
    ElementId res;
    if (!cur_visible_row.empty())
    {
        auto p = GetParent(cur_visible_row);
        int pos = GetChildPos(p->id);
        if (IsVisible(p->id))
        {
            //the most simple variant - go above and find first invisible paragraph
            res = p->id;
            while (--pos >= 0)
            {
                auto el = p->parent->elements->Get(pos);
                if (!IsVisible(el->id))
                    break;
                res = el->id;
            }
            return res;
        }

        //try to find a visible paragraph above
        for (int i = 0; i < 10 && pos >= 0; ++i, --pos)
        {
            auto el = p->parent->elements->Get(pos);
            if (IsVisible(el->id))
            {
                res = el->id;
                while (--pos >= 0)
                {
                    auto el = p->parent->elements->Get(pos);
                    if (!IsVisible(el->id))
                        break;
                    res = el->id;
                }
                return res;
            }
        }

        //try to find a visible paragraph below
        for (int i = 0; i < 10 && pos >= 0; ++i, ++pos)
        {
            auto el = p->parent->elements->Get(pos);
            if (IsVisible(el->id))
                return el->id;
        }
    }

    cur_visible_row.clear();

    //the most comprehensive variant: find in the whole document
    return text->elements->FindUpper(window->document_point.y);
}

ElementId Document::GetFirstVisibleRow(ElementId paragraph_id)
{
    ElementId res;
    auto el = GetElement(paragraph_id);
    cur_visible_row = el->elements->FindUpper(window->document_point.y);
    return cur_visible_row;
}

void Document::AddErrorMark(ElementId _id, int start, int size)
{
    auto it = std::find_if(error_marks.begin(), error_marks.end(), 
        [_id, start, size](const ErrorMark& m)
        {
            return m.id == _id && m.start == start && m.size == size;
        });
    if (it != error_marks.end())
        return;
    error_marks.emplace_back(ErrorMark{_id, start, size});
}

void Document::RemoveErrorMarks(ElementId parent_id)
{
    for (size_t i = 0; i < error_marks.size();)
    {
        if (parent_id == error_marks[i].id || IsChild(parent_id, error_marks[i].id))
            error_marks.erase(error_marks.begin() + i);
        else
            ++i;
    }
}

bool Document::HasErrorMark(ElementId _id, int& start, int& size)
{
    auto it = std::find_if(error_marks.begin(), error_marks.end(), 
        [_id](const ErrorMark& m)
        {
            return m.id == _id;
        });
    if (it == error_marks.end())
        return false;
    start = it->start;
    size = it->size;
    return true;
}

void Document::WaitTask(uint task_id, uint64_t timeout)
{
    if (task_id == 0)
        return;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    auto cur_time = now;
    while (cur_time - now <= timeout * 1ms)
    {
        std::this_thread::sleep_for(100ms);

        std::lock_guard<std::recursive_mutex> lock(edit_mutex);
        if (std::find(last_tasks.begin(), last_tasks.end(), task_id) != last_tasks.end())
            return;
        last_tasks.clear();

        if (timeout > 0)
            cur_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    }
}

void Document::RestrictUndo()
{
    if (undo_tasks.empty() || undo_tasks.back()->id == cur_task_id) //restrict only if a group has ended
        return;
    
    uint groups_count = 0;
    uint group_id = 0;
    for (auto it = undo_tasks.begin(); it != undo_tasks.end(); ++it) //count groups
    {
        auto& t = *it;
        if (t->id != group_id)
        {
            if (++groups_count > config.undo_size - 1)
            {
                //restrict
                group_id = (*undo_tasks.begin())->id;
                for (auto _it = undo_tasks.begin(); _it != undo_tasks.end();)
                {
                    auto& t = *_it;
                    if (t->id != group_id)
                        break;
                    _it = undo_tasks.erase(_it);
                }
                return;
            }
            group_id = t->id;
        }
    }
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

    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    last_solver_task_id = 0;
    last_solver_executed = false;
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
    
    //move view port in the view if the caret is outside of it
    if (r.left < p.x + view_port.left)
    {
        caret->Hide(); //caret will be shown on Redraw
        window->MoveDocument(r.left - view_port.left - 1, p.y);
        Redraw(text->id, false);
    }
    else if (r.width < view_port.width && r.GetRight() > view_port.GetRight() + p.x)
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
    else if (r.height < view_port.height && r.GetBottom() > view_port.GetBottom() + p.y)
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
