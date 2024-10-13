#include "document.h"
#include "str.h"
#include "paragraph.h"
#include "row.h"
#include "image.h"
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
#include "formulas/unit.h"
#include "formulas/subscript.h"
#include "formulas/exclamation.h"
#include "formulas/and.h"
#include "formulas/or.h"
#include "formulas/xor.h"
#include "formulas/percent.h"
#include "formulas/sum.h"
#include "formulas/product.h"
#include "formulas/comma.h"
#include "util.h"
#include <assert.h>
#include <chrono>
#include <sstream>

namespace yutovo
{

using namespace std::chrono_literals;
using namespace std::chrono;

//Document

Document::Document(Window* _window, Config& _config) :
    window(_window),
    selection(this),
    config(_config),
    solver(this),
    undo_base(this),
    last_selection(this),
    logger(Logger::GetInstance(config.logs_path, "yutovo_editor", true, true))
{
    logger->SetLevel((int)_config.log_level);
    LOG_DEBUG("Document start");

    string_formats.reset(new StringFormats());
    paragraph_formats.reset(new ParagraphFormats(string_formats));
    code_formats.reset(new CodeFormats());
    formula_formats.reset(new FormulaFormats(string_formats));

    current_code_format = code_formats->GetFormat("Calculator", 5, 5, 5, 5, 2, 2, 2, 2, 2, Color::Blue());

    current_paragraph_format = paragraph_formats->GetFormat("Text body");
    current_formula_format = formula_formats->GetFormat("Code");
    current_page_format = PageFormats::GetFormat(20, 20, 20, 20, 10);
}

Document::~Document()
{
    exit = true;
    next_circle = true;
    main_loop.join();
    LOG_DEBUG("Document end");
}

void Document::Start()
{
    current_code_format->border_color = config.code_block_border_color;

    window->Init(this);

    caret.reset(new Caret(this));
    text.reset(new Text(this));

    caret->MoveToDocumentBegin(nullptr);

    main_loop = std::thread(&Document::MainLoop, this);

    text->Remake(true);
    Redraw(text->id, false);

    std::tuple<char32_t, std::string, int> s1{U'(', "Arial", 300};
    std::tuple<char32_t, std::string, int> s2{U')', "Arial", 300};
    std::vector<std::tuple<char32_t, std::string, int>> s{s1, s2};
    window->PrepareSymbolsSizes(s);
}

void Document::GetConfig(Config& _config)
{
    std::unique_lock<std::recursive_mutex> lock(tasks_mutex);
    _config = config;
}

uint Document::SetConfig(const Config& _config, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetConfigTask(text, _config, with_undo));
    next_circle = true;
    return tasks.back()->id;
}

uint Document::SetConfig(const std::string& _config, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetConfigTask(text, _config, with_undo));
    next_circle = true;
    return tasks.back()->id;
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
                auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                auto next = now;
                while (!next_circle && next - now < config.caret_blink_delay * 1ms) //wait for tasks
                {
                    std::this_thread::sleep_for(10ms);
                    next = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
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
                caret->Hide(); //caret will be shown on Redraw or caret moving
                selection.can_optimize = false;
                for (size_t i = 0; i < temp_undo_tasks.size(); ++i)
                {
                    changed_elements.clear();
                    resolve_elements.clear();
                    TaskPtr& t = temp_undo_tasks[i];
                    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
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
                caret->Hide();
                for (size_t i = 0; i < temp_redo_tasks.size(); ++i)
                {
                    changed_elements.clear();
                    resolve_elements.clear();
                    TaskPtr& t = temp_redo_tasks[i];
                    cur_task_id = t->id;
                    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
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
            for (auto it = tasks.begin(); it != tasks.end(); ++it)
                temp_tasks.push_back(*it);
            tasks.clear();
        }

        if (!temp_tasks.empty())
        {
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

                {
                    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
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
                    last_editor_selection = selection.GetState();
                }

                {
                    std::lock_guard<std::recursive_mutex> lock(state_mutex);
                    last_editor_state = EditorState{caret->GetCaretState(), last_editor_selection};
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
#endif
                while (last_tasks.size() > last_tasks_count)
                    last_tasks.pop_back();
                last_tasks.push_front(t->id);
            }

            caret->Show();

            temp_tasks.clear();
        }

        UpdateChanged();
    }
}

uint Document::InsertParagraph(bool with_undo)
{
    LOG_TRACE("Insert paragraph");
    CaretState c = caret->GetCaretState();
    auto el = FindParentParagraph(c.id);
    if (!el)
        return 0;
    ParagraphFormatPtr format = ((Paragraph*)el.get())->format;
    return InsertElement(new Paragraph(this, format), with_undo);
}

uint Document::InsertString(const std::string& str, bool with_undo)
{
    LOG_TRACE("Insert string: {}", str);
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new String(this, str, format), with_undo);
    return 0;
}

uint Document::InsertString(const std::u32string& str, bool with_undo)
{
    LOG_TRACE("Insert string: {}", ToBasicString(str));
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new String(this, str, format), with_undo);
    return 0;
}

uint Document::InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo)
{
    LOG_TRACE("Insert string: {}, format: {}", str, string_format->ToString());
    return InsertElement(new String(this, str, string_format), with_undo);
}

uint Document::InsertString(const std::string& str, ElementId element_id, bool with_undo)
{
    LOG_TRACE("Insert string: {}", str);
    StringFormat f;
    if (!GetStringFormat(element_id, f))
        return 0;
    StringFormatPtr string_format = string_formats->GetFormat(f);

    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SetStringTask(text, ToUtfString(str), element_id));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertElement(Element* element, bool with_undo, ElementId element_id)
{
    LOG_TRACE("Insert element: {}", ToBasicString(element->ToText()));
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    return InsertElements(elements, with_undo, element_id);
}

uint Document::InsertElements(std::vector<ElementPtr>& elements, bool with_undo, ElementId element_id, bool pasting)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new InsertElementsTask(text, elements, with_undo, pasting));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::DeleteElements(bool left, bool with_undo)
{
    LOG_TRACE("Delete elements: {}", left);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new DeleteElementsTask(text, left, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ClearElements(ElementId element_id, bool with_undo)
{
    LOG_TRACE("Clear elements: {}", IdToString(element_id));
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new DeleteElementsTask(text, element_id, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertCode(bool next_code_id, bool with_undo)
{
    LOG_TRACE("Insert code: {}", next_code_id);
    if (next_code_id)
        ++cur_code_id;
    return InsertFormula(new CodeBlock(this, cur_code_id), with_undo);
}

uint Document::InsertCodeString(const std::string& str, bool with_undo)
{
    LOG_TRACE("Insert code string: {}", str);
    FormulaFormatPtr format;
    if (GetCurrentFormulaFormat(format))
        return InsertFormula(new CodeString(this, str, format->string_format), with_undo);
    return 0;
}

uint Document::InsertPlus(bool with_undo)
{
    LOG_TRACE("Insert plus");
    return InsertFormula(new Plus(this), with_undo);
}

uint Document::InsertMinus(bool with_undo)
{
    LOG_TRACE("Insert minus");
    return InsertFormula(new Minus(this), with_undo);
}

uint Document::InsertMultiply(bool with_undo)
{
    LOG_TRACE("Insert multiply");
    return InsertFormula(new Multiply(this), with_undo);
}

uint Document::InsertDivision(bool with_undo)
{
    LOG_TRACE("Insert division");
    return InsertFormula(new Division(this), with_undo);
}

uint Document::InsertPower(bool with_undo)
{
    LOG_TRACE("Insert power");
    return InsertFormula(new Power(this), with_undo);
}

uint Document::InsertNthRoot(bool with_undo)
{
    LOG_TRACE("Insert nth root");
    return InsertFormula(new NthRoot(this), with_undo);
}

uint Document::InsertSquareRoot(bool with_undo)
{
    LOG_TRACE("Insert square root");
    return InsertFormula(new SquareRoot(this), with_undo);
}

uint Document::InsertEquation(yutovo_solver::ResultType result_type, bool with_undo)
{
    LOG_TRACE("Insert equation");
    return InsertFormula(new Equation(this, result_type), with_undo);
}

uint Document::InsertOpenFence(bool with_undo)
{
    LOG_TRACE("Insert open fence");
    return InsertFormula(new OpenFence(this), with_undo);
}

uint Document::InsertCloseFence(bool with_undo)
{
    LOG_TRACE("Insert close fence");
    return InsertFormula(new CloseFence(this), with_undo);
}

uint Document::InsertAssignment(bool with_undo)
{
    LOG_TRACE("Insert assignment");
    return InsertFormula(new Assignment(this), with_undo);
}

uint Document::InsertUnit(bool with_undo)
{
    LOG_TRACE("Insert assignment");
    return InsertFormula(new Unit(this), with_undo);
}

uint Document::InsertSubscript(bool with_undo)
{
    LOG_TRACE("Insert subscript");
    return InsertFormula(new Subscript(this), with_undo);
}

uint Document::InsertExclamation(bool with_undo)
{
    LOG_TRACE("Insert exclamation");
    return InsertFormula(new Exclamation(this), with_undo);
}

uint Document::InsertAnd(bool with_undo)
{
    LOG_TRACE("Insert and");
    return InsertFormula(new And(this), with_undo);
}

uint Document::InsertOr(bool with_undo)
{
    LOG_TRACE("Insert or");
    return InsertFormula(new Or(this), with_undo);
}

uint Document::InsertXor(bool with_undo)
{
    LOG_TRACE("Insert xor");
    return InsertFormula(new Xor(this), with_undo);
}

uint Document::InsertPercent(bool with_undo)
{
    LOG_TRACE("Insert percent");
    return InsertFormula(new Percent(this), with_undo);
}

uint Document::InsertSum(bool with_undo)
{
    LOG_TRACE("Insert sum");
    return InsertFormula(new Sum(this), with_undo);
}

uint Document::InsertProduct(bool with_undo)
{
    LOG_TRACE("Insert product");
    return InsertFormula(new Product(this), with_undo);
}

uint Document::InsertImage(const std::string& image_base64, bool with_undo)
{
    LOG_TRACE("Insert image");
    return InsertElement(new Image(this, image_base64), with_undo);
}

uint Document::InsertImage(const std::vector<unsigned char>& image, bool with_undo)
{
    LOG_TRACE("Insert image");
    return InsertElement(new Image(this, image), with_undo);
}

uint Document::InsertComma(bool with_undo)
{
    LOG_TRACE("Insert comma");
    return InsertFormula(new Comma(this), with_undo);
}

uint Document::InsertFences(bool with_undo)
{
    LOG_TRACE("Insert fences");
    InsertFormula(new OpenFence(this), with_undo, false);
    uint r = InsertFormula(new CloseFence(this), with_undo, true);
    MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertFunction(const std::string& name, bool with_undo)
{
    LOG_TRACE("Insert function: {}", name);
    InsertCodeString(name, with_undo);
    InsertFormula(new OpenFence(this), with_undo, true);
    uint r = InsertFormula(new CloseFence(this), with_undo, true);
    MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertSubscriptFunction(const std::string& name, bool with_undo)
{
    LOG_TRACE("Insert subscript function: {}", name);
    InsertCodeString(name, with_undo);
    return InsertFormula(new Subscript(this), with_undo, true);
}

uint Document::InsertFormula(Element* element, bool with_undo, bool with_last_task_id)
{
    LOG_TRACE("Insert formula: {}", ToBasicString(element->ToText()));
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    if (current_formula_format)
        SetCurrentStringFormat(current_formula_format->string_format);
    return InsertFormulas(elements, with_undo, with_last_task_id);
}

uint Document::InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool with_last_task_id, bool pasting)
{
    LOG_TRACE("Insert formulas");
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (with_last_task_id)
            tasks.emplace_back(new InsertFormulasTask(text, last_task_id, elements, with_undo));
        else
            tasks.emplace_back(new InsertFormulasTask(text, elements, with_undo, pasting));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertUnit(const yutovo_calculator::Unit& unit)
{
    LOG_TRACE("Insert unit");
    FormulaFormatPtr f = formula_formats->GetFormat("Code");
    StringFormatPtr string_format = f->string_format;

    ElementPtr numerator(new CodeRow(this));
    numerator->elements->Clear();
    ElementPtr denomerator;
    for (auto& u : unit.unit)
    {
        auto s = ToBasicString(u.first);
        if (u.second > 0)
        {
            if (numerator->elements->Count() > 0)
                numerator->AddElement(ElementPtr(new Multiply(this)));
            if (u.second == 1)
                numerator->AddElement(CodeStringPtr(new CodeString(numerator.get(), s, string_format)));
            else
            {
                PowerPtr p(new Power(this));
                p->AddBase(CodeStringPtr(new CodeString(p.get(), s)));
                p->AddExponent(CodeStringPtr(new CodeString(p.get(), std::to_string(u.second), string_format)));
                numerator->AddElement(p);
            }
        }
        else
        {
            if (!denomerator)
            {
                denomerator.reset(new CodeRow(this));
                denomerator->elements->Clear();
            }
            if (denomerator->elements->Count() > 0)
                denomerator->AddElement(ElementPtr(new Multiply(this)));
            if (u.second == -1)
                denomerator->AddElement(CodeStringPtr(new CodeString(denomerator.get(), s, string_format)));
            else
            {
                PowerPtr p(new Power(this));
                p->AddBase(CodeStringPtr(new CodeString(p.get(), s)));
                p->AddExponent(CodeStringPtr(new CodeString(p.get(), std::to_string(-u.second), string_format)));
                denomerator->AddElement(p);
            }
        }
    }

    auto* row = new CodeRow(this);

    if (denomerator)
    {
        auto* d = new Division(this);
        if (numerator->elements->Count() == 0)
            numerator->AddElement(CodeStringPtr(new CodeString(d, U"1")));
        d->AddNumerator(numerator);
        d->AddDenomerator(denomerator);
        row->AddElement(ElementPtr(d));
    }
    else
    {
        for (int i = 0; i < numerator->elements->Count(); ++i)
            row->AddElement(numerator->elements->Get(i));
    }

    auto* code = new CodeBlock(this, 1);
    code->elements->Clear();
    code->elements->Add(ElementPtr(row));
    return InsertFormula(code, false);
}

uint Document::ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, const bool strikethrough, 
    Color text_color, Color text_bg_color, bool with_undo)
{
    LOG_TRACE("Change string format: family={}, size={}, bold={}, italic={}, underline={}, text_color={}, text_bg_color={}", 
        family, size, bold, italic, underline, text_color.ToString(), text_bg_color.ToString());
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return ChangeStringFormat(string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, text_color, text_bg_color, Color::Blue()), 
        with_undo);
}

uint Document::ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
    bool set_strikethrough, bool set_text_color, bool set_text_bg_color, bool with_undo)
{
    LOG_TRACE("Change string format: format={}, set_family={}, set_size={}, set_bold={}, set_italic={}, set_underline={}, set_strikethrough={}, "\
        "set_text_color={}, set_text_bg_color={}", 
        format->ToString(), set_family, set_size, set_bold, set_italic, set_underline, set_strikethrough, set_text_color, set_text_bg_color);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangeStringFormatTask(text, format, set_family, set_size, set_bold, set_italic, set_underline, set_strikethrough, 
            set_text_color, set_text_bg_color, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeStringFormat(const StringFormatPtr format, bool with_undo)
{
    LOG_TRACE("Change string format: format={}", format->ToString());
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangeStringFormatTask(text, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo)
{
    LOG_TRACE("Change paragraph format: format={}", format->ToString());
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        CaretState c = caret->GetCaretState();
        tasks.emplace_back(new ChangeParagraphFormatTask(text, c.id, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const std::string name, bool with_undo)
{
    LOG_TRACE("Change paragraph format: name={}", name);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        auto format = paragraph_formats->GetFormat(name);
        if (!format)
            return 0;
        CaretState c = caret->GetCaretState();
        tasks.emplace_back(new ChangeParagraphFormatTask(text, c.id, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const ParagraphFormat::Alignment alignment, bool with_undo)
{
    LOG_TRACE("Change paragraph format: alignment={}", (int)alignment);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        CaretState c = caret->GetCaretState();
        auto el = FindParentParagraph(c.id);
        if (!el)
            return 0;
        ParagraphFormatPtr format = ((Paragraph*)el.get())->format;
        format = paragraph_formats->GetFormat(format->name, alignment, format->word_wrap, format->line_spacing, format->indent_before, 
            format->indent_after, format->indent_first_line, format->spacing_before, format->spacing_after, format->default_string_format);
        tasks.emplace_back(new ChangeParagraphFormatTask(text, c.id, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

bool Document::StoreUndo(const ElementId& _id)
{
    if (_id.size() == 1)
        return StoreUndo(_id, 0, text->elements->Count());
    RestrictUndo();

    int undo_id = undo_base.Store(_id);
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, _id, 0, 0, cur_task_id)));
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
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->id, 0, p->elements->Count(), cur_task_id)));
        return true;
    }
    else
    {
        undo_id = undo_base.Store(parent_id, pos, size);
        _id = yutovo::GetChild(parent_id, pos);
    }
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, _id, pos, delete_size, cur_task_id)));
    return true;
}

bool Document::StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size, UndoTask::UndoOperation undo_operation)
{
    RestrictUndo();

    int undo_id;
    auto p = GetParent(parent_id);
    if (p && (p->type == ElementType::EQUATION || p->type == ElementType::ASSIGNMENT))
    {
        int _pos = yutovo::GetChildPos(p->id);
        undo_id = undo_base.Store(p->parent->id, _pos, 1);
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->parent->id, _pos, 1, 0, UndoTask::UndoOperation::CHANGE, cur_task_id)));
        return true;
    }
    else if (IsRow(parent_id))
    {
        undo_id = undo_base.Store(p->id, 0, p->elements->Count());
    }
    else if (IsParagraph(parent_id))
    {
        int _pos = p->elements->GetChildPos(parent_id);
        undo_id = undo_base.Store(p->id, _pos, 1);
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->id, _pos, 1, 0, UndoTask::UndoOperation::CHANGE, cur_task_id)));
        return true;
    }
    else
    {
        undo_id = undo_base.Store(parent_id, pos, size);
    }
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, parent_id, pos, size, delete_size, undo_operation, cur_task_id)));
    return true;
}

bool Document::StoreUndo(const Config& config)
{
    RestrictUndo();
    int undo_id = undo_base.Store(config);
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, cur_task_id)));
    return true;
}

bool Document::RestoreUndo(const int undo_id, std::vector<ElementPtr>& elements)
{
    return undo_base.Restore(undo_id, elements);
}

bool Document::RestoreUndo(const int undo_id, Config& config)
{
    RestrictUndo();
    return undo_base.Restore(undo_id, config);
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

size_t Document::GetUndoSize()
{
    return undo_tasks.size();
}

void Document::ResetTasks()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.clear();
    undo_tasks.clear();
    redo_tasks.clear();
    changed = false;
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
    bool r = false;
    if (edit_mutex.try_lock())
    {
        r = text->GetElementAtCoords(x, y, id);
        edit_mutex.unlock();
    }
    return r;
}

bool Document::GetElementRect(const ElementId id, Rect& rect)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto _el = GetElement(id);
    if (!_el)
        return false;
    rect = _el->GetAbsoluteRect();
    return true;
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

    if (pos > 0 && pos == el->elements->Count())
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
                    if (ch->logical_id != last_id)
                    {
                        last_id = ch->logical_id;
                        ++p;
                    }
                    if (pos - p <= r->elements->Count())
                    {
                        res = r->id;
                        res.push_back(pos - p);
                        if (pos - p == r->elements->Count())
                            last_pos = true;
                        break;
                    }
                    else if (r->elements->Count() == k + 1 && _el->elements->Count() == j + 1 && GetChildPos(ch->logical_id) == pos - 1)
                    {
                        res = r->id;
                        res.push_back(r->elements->Count());
                        last_pos = true;
                        break;
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
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = caret->GetElement();
    if (!el)
        return ElementId{};
    auto p = FindParent(el->id, type);
    return p ? p->id : ElementId{};
}

ElementPtr Document::FindParentParagraph(const ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
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

uint Document::FindCodeBlock(const ElementId& id)
{
    auto el = FindParent(id, ElementType::CODE_BLOCK);
    if (!el)
        return 0;
    CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
    return c->code_id;
}

ElementPtr Document::FindByString(const ElementId& start_id, const std::u32string& str)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (start_id.empty() || str.empty())
        return nullptr;
    ElementPtr el = GetElement(start_id);
    if (!el)
        return nullptr;
    
    ElementPtr res = nullptr;
    if (IsString(el))
    {
        if (el->ToText() == str)
            return el;
    }
    else
    {
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            res = FindByString(el->elements->Get(i)->id, str);
            if (res)
                break;
        }
    }
    return res;
}

ElementPtr Document::FindByType(const ElementId& start_id, const ElementType type)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (start_id.empty())
        return nullptr;
    ElementPtr el = GetElement(start_id);
    if (!el)
        return nullptr;
    
    ElementPtr res = nullptr;
    if (el->type == type)
        return el;
    else if (IsString(el))
    {
        return nullptr;
    }
    else
    {
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            res = FindByType(el->elements->Get(i)->id, type);
            if (res)
                break;
        }
    }
    return res;
}

Rect Document::GetCaretRect(const CaretState& caret_state)
{
    ElementPtr el = GetParent(caret_state.id);
    return el->GetAbsoluteRect(el->GetCaretRect(caret_state.GetPos()));
}

bool Document::GetCurrentStringFormat(StringFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        format = current_string_format;
        return true;
    }
    return false;
}

void Document::SetCurrentStringFormat(StringFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    current_string_format = format;
}

bool Document::GetCurrentParagraphFormat(ParagraphFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_paragraph_format)
    {
        format = current_paragraph_format;
        return true;
    }
    return false;
}

bool Document::GetCurrentFormulaFormat(FormulaFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_formula_format)
    {
        format = current_formula_format;
        return true;
    }
    return false;
}

void Document::UpdateFormats()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
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
        window->OnFormatChanged(MakeEditorState());
    }
}

uint Document::SetFontFamily(const std::string& family)
{
    LOG_TRACE("Set font family: {}", family);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        if (!selection.IsEmpty())
        {
            return ChangeStringFormat(string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, current_string_format->text_color, 
                current_string_format->text_bg_color, current_string_format->text_bg_selection_color), true);
        }
        else
        {
            current_string_format = string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, current_string_format->text_color, 
                current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        }
    }
    return 0;
}

uint Document::SetFontSize(const uint size)
{
    LOG_TRACE("Set font size: {}", size);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, size, current_string_format->bold, current_string_format->italic, 
            current_string_format->underline, current_string_format->strikethrough, current_string_format->text_color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, true, false, false, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetBold(const bool enabled)
{
    LOG_TRACE("Set bold: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, enabled, current_string_format->italic, 
            current_string_format->underline, current_string_format->strikethrough, current_string_format->text_color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, true, false, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetItalic(const bool enabled)
{
    LOG_TRACE("Set italic: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, enabled, 
            current_string_format->underline, current_string_format->strikethrough, current_string_format->text_color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, true, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetUnderline(const bool enabled)
{
    LOG_TRACE("Set underline: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, enabled, current_string_format->strikethrough, current_string_format->text_color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, true, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetStrikethrough(const bool enabled)
{
    LOG_TRACE("Set strikethrough: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, enabled, current_string_format->text_color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, true, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetColor(const Color color)
{
    LOG_TRACE("Set color: {}", color.ToString());
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, false, true, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetBgColor(const Color color)
{
    LOG_TRACE("Set background bold: {}", color.ToString());
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, 
            current_string_format->text_color, color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, false, false, true, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetCurrentParagraphFormat(const std::string& name)
{
    LOG_TRACE("Set current paragraph format: {}", name);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    current_paragraph_format = paragraph_formats->GetFormat(name);
    if (current_paragraph_format)
        return ChangeParagraphFormat(current_paragraph_format, true);
    return 0;
}

ElementType Document::GetElementType(const ElementId id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
        return ElementType::NONE;
    return el->type;
}

bool Document::IsEditable(const ElementId id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
    {
        //check it's the last caret position
        auto p = GetParent(id);
        if (!p || p->elements->Count() == 0)
            return false;
        if (id[id.size() - 1] == p->elements->Count() && p->editable)
            return true;
        return false;
    }
    return el->editable;
}

bool Document::IsEmpty()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return text->IsEmpty();
}

bool Document::IsString(ElementPtr el)
{
    return el && (el->type == ElementType::STRING || el->type == ElementType::CODE_STRING);
}

bool Document::IsString(ElementId id)
{
    return IsString(GetElement(id));
}

bool Document::IsRow(ElementPtr el)
{
    return el && (el->type == ElementType::ROW || el->type == ElementType::CODE_ROW);
}

bool Document::IsRow(ElementId id)
{
    return IsRow(GetElement(id));
}

bool Document::IsParagraph(ElementPtr el)
{
    return el && (el->type == ElementType::PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH);
}

bool Document::IsParagraph(ElementId id)
{
    return IsParagraph(GetElement(id));
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
    else if (IsParagraph(el))
    {
        //check if the paragraph has strings with only one format
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            auto row = el->elements->Get(i);
            for (int j = 0; j < row->elements->Count(); ++j)
            {
                auto ch = row->elements->Get(j);
                if (!IsString(ch))
                    return false;
                auto f = ch->GetStringFormat();
                if (i != 0 && j != 0)
                {
                    if (*f != format)
                        return false;
                }
                format = *f;
            }
        }
        return true;
    }
    else if (IsRow(el))
    {
        //check if the row has strings with only one format
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            auto ch = el->elements->Get(i);
            if (!IsString(ch))
            {
                auto f = el->GetStringFormat();
                if (!f)
                    return false;
                format = *f;
                break;
            }
            auto f = ch->GetStringFormat();
            if (i != 0)
            {
                if (*f != format)
                    return false;
            }
            format = *f;
        }
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
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
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
    return last_task_id;
}

uint Document::SelectAll()
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::SELECT_ALL, true, false, false);
}

uint Document::Select(const int start_x, const int start_y, const int end_x, const int end_y)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, Point{start_x, start_y}, Point{end_x, end_y}));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::SelectOut()
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::SELECT_OUT, true);
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

bool Document::IsChanged()
{
    return changed;
}

uint Document::Resize(uint width, uint height)
{
    break_remake = true;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ResizeTask(text, width, height));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Redraw(const ElementId& id, bool move_into_view)
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
    next_circle = true;
    return tasks.back()->id;
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
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new NewTask(text));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Save(const std::string& filename)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SaveTask(text, filename));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::SaveJson(std::u32string& json)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SaveTask(text, &json));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Load(const std::string& filename)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new LoadTask(text, filename));
#ifdef DEBUG
        last_load_task_id = tasks.back()->id;
#endif
    }
    next_circle = true;
    return last_load_task_id;
}

uint Document::LoadJson(const std::u32string& json_doc, const int document_id)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new LoadTask(text, json_doc, document_id));
#ifdef DEBUG
        last_load_task_id = tasks.back()->id;
#endif
    }
    next_circle = true;
    return last_load_task_id;
}

uint Document::Copy(std::u32string& out_json, std::u32string& out_text)
{
    LOG_TRACE("Copy: out_json={}, out_text={}", ToBasicString(out_json), ToBasicString(out_text));
    {
        out_json = U"";
        out_text = U"";
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new CopyTask(text, out_json, out_text, false));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Paste(std::u32string& in_json)
{
    LOG_TRACE("Paste: in_json={}", ToBasicString(in_json));
    StringFormatsPtr _string_formats;
    rapidjson::Document doc;
    auto str = ToBasicString(in_json);
    if (doc.Parse<0>(str.c_str()).HasParseError())
    {
        window->OnPasteResult(PasteResult::PasteError);
        return 0;
    }

    if (doc.HasMember("string_formats") && doc["string_formats"].IsArray())
    {
        //load string formats
        std::lock_guard<std::recursive_mutex> lock(edit_mutex);
        string_formats->FromJson(doc["string_formats"], doc.GetAllocator());
    }

    if (!doc.HasMember("copy") || !doc["copy"].IsArray())
    {
        window->OnPasteResult(PasteResult::PasteError);
        return 0;
    }

    //load elements
    std::vector<ElementPtr> elements;
    rapidjson::Value arr = doc["copy"].GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return 0;
        rapidjson::Value value = arr[i].GetObject();
        ElementPtr el(CreateFromJson(nullptr, this, value, doc.GetAllocator()));
        if (!el)
            return 0;
        elements.push_back(el);
    }

    if (elements.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return last_task_id;
    }

    bool only_formulas = true;
    bool only_paragraphs = true;
    for (auto& el : elements)
    {
        if (!el->IsFormula())
            only_formulas = false;
        if (!IsParagraph(el))
            only_paragraphs = false;
    }

    if (only_formulas)
        InsertFormulas(elements, true, false, true);
    else if (only_paragraphs)
    {
        //compound them in text to workout them in one iteration
        ElementPtr t(new Text(this, false));
        for (auto& el : elements)
            t->elements->Add(el);
        elements.clear();
        elements.push_back(t);
        InsertElements(elements, true, ElementId{}, true);
    }
    else
        InsertElements(elements, true, ElementId{}, true);
    
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::PasteText(std::u32string&& str)
{
    LOG_TRACE("Paste text: {}", ToBasicString(str));
    if (str.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertString(str, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::PasteImage(const std::vector<unsigned char>& image)
{
    LOG_TRACE("Paste image");
    if (image.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertImage(image, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::PasteImage(const std::string& image_base64)
{
    LOG_TRACE("Paste image");
    if (image_base64.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertImage(image_base64, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::Cut(std::u32string& out_json, std::u32string& out_text)
{
    LOG_TRACE("Cut: out_json={}, out_text={}", ToBasicString(out_json), ToBasicString(out_text));
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new CopyTask(text, out_json, out_text, true));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

std::string Document::ToHtml()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return text->ToHtml();
}

std::u32string Document::ToText()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return text->ToText();
}

TextFormatPtr Document::GetDefaultTextFormat()
{
    return TextFormats::GetFormat(TextFormat::Paging::ONE_PAGE);
}

PageFormatPtr Document::GetDefaultPageFormat()
{
    if (config.with_border)
        return current_page_format;
    return PageFormats::GetFormat(0, 0, 0, 0, 0);
}

uint Document::SetDefaultPageFormat(uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, uint paragraph_spacing)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangePageFormatTask(text, PageFormats::GetFormat(left_indent, top_indent, right_indent, bottom_indent, paragraph_spacing)));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

StringFormatPtr Document::GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, Color::Black(), Color::White(), Color::Blue());
}

StringFormatPtr Document::GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
    Color text_color, Color text_bg_color)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, text_color, text_bg_color, Color::Blue());
}

StringFormatPtr Document::GetStringFormat(const boost::uuids::uuid& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(id);
}

EditorState Document::GetEditorState()
{
    std::lock_guard<std::recursive_mutex> lock(state_mutex);
    return last_editor_state;
}

EditorState Document::MakeEditorState()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return EditorState{caret->GetCaretState(), selection.GetState()};
}

LogicalEditorState Document::GetLogicalEditorState()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return {caret->GetLogicalCaretState(), selection.GetLogicalState()};
}

void Document::SetEditorState(EditorState& state)
{
    selection.Set(state.selection_state);
    caret->SetState(state.caret_state);
}

void Document::SetEditorState(LogicalEditorState& state)
{
    selection.Set(state.selection_state);
    caret->SetState(state.caret_state);
}

void Document::Solve(ElementId _id, uint code_id, Config::AutoResultConfig& config, std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::RealResultConfig& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::IntegerResultConfig& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::RationalResultConfig& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::Solve(ElementId _id, uint code_id, Config::ComplexResultConfig& config, const std::u32string& expression, const uint delay)
{
    solver.Solve(_id, code_id, config, expression + U";", delay);
}

void Document::BreakSolving(const ElementId id, uint code_id)
{
    solver.BreakSolving(id, code_id);
}

void Document::SetIdentifier(ElementId _id, uint code_id, const std::u32string& identifier, const std::u32string& expression, const uint delay)
{
    solver.SetIdentifier(_id, code_id, identifier, expression + U";", delay);
}

void Document::RemoveIdentifier(ElementId _id, uint code_id, const std::u32string& identifier, const uint delay)
{
    solver.RemoveIdentifier(_id, code_id, identifier, delay);
}

void Document::RemoveUserIdentifiers()
{
    solver.RemoveUserIdentifiers();
}

ResultType Document::GetResultType(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    AutoResult* r = (AutoResult*)el.get();
    if (!r)
        return ResultType::NONE;
    return r->GetResultType();
}

uint Document::SetResult(ElementId _id, ResultType result_type, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultTask(text, _id, result_type, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

int Document::GetPrecision(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return 0;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.precision;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.precision;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.precision;
    return -1;
}

uint Document::SetPrecision(ElementId _id, uint precision, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, precision, -1, AngleMeasure::None, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

int Document::GetExp(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return 0;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.exp;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.exp;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.exp;
    return -1;
}

uint Document::SetExp(ElementId _id, uint exp, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, -1, exp, AngleMeasure::None, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

AngleMeasure Document::GetResultAngleMeasure(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return AngleMeasure::None;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.result_angle_measure;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.result_angle_measure;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.result_angle_measure;
    return AngleMeasure::None;
}

uint Document::SetResultAngleMeasure(ElementId _id, AngleMeasure result_angle_measure, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, -1, -1, result_angle_measure, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

Notation Document::GetResultNotation(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return Notation::None;
    IntegerResult* r = dynamic_cast<IntegerResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.integer_result.result_notation;
        return Notation::None;
    }
    return r->config.result_notation;
}

Notation Document::GetDefaultNotation(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return Notation::None;
    IntegerResult* r = dynamic_cast<IntegerResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.integer_result.default_notation;
        return Notation::None;
    }
    return r->config.default_notation;
}

uint Document::SetNotation(ElementId _id, Notation default_notation, Notation result_notation, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, default_notation, result_notation, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

FractionForm Document::GetFractionForm(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return FractionForm::None;
    RationalResult* r = dynamic_cast<RationalResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.rational_result.fraction_form;
        return FractionForm::None;
    }
    return r->config.fraction_form;
}

uint Document::SetFractionForm(ElementId _id, FractionForm fraction_form, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, fraction_form, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

ComplexForm Document::GetComplexForm(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return ComplexForm::None;
    ComplexResult* r = dynamic_cast<ComplexResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.complex_result.form;
        return ComplexForm::None;
    }
    return r->config.form;
}

uint Document::SetComplexForm(ElementId _id, ComplexForm form, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, form, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

bool Document::HasUnit(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return false;
    
    if (el->type == ElementType::AUTO_RESULT)
    {
        AutoResult* r = dynamic_cast<AutoResult*>(el.get());
        std::vector<yutovo_calculator::Unit> cast_units;
        r->GetCastUnits(cast_units);
        return !cast_units.empty();
    }
    else if (el->type == ElementType::REAL_RESULT)
    {
        RealResult* r = dynamic_cast<RealResult*>(el.get());
        return !r->cast_units.empty();
    }
    else if (el->type == ElementType::RATIONAL_RESULT)
    {
        RationalResult* r = dynamic_cast<RationalResult*>(el.get());
        return !r->cast_units.empty();
    }
    return false;
}

void Document::GetCastUnits(ElementId _id, std::vector<yutovo_calculator::Unit>& cast_units)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return;
    if (el->type == ElementType::AUTO_RESULT)
    {
        AutoResult* r = dynamic_cast<AutoResult*>(el.get());
        r->GetCastUnits(cast_units);
    }
    else if (el->type == ElementType::REAL_RESULT)
    {
        RealResult* r = dynamic_cast<RealResult*>(el.get());
        cast_units = r->cast_units;
    }
    else if (el->type == ElementType::RATIONAL_RESULT)
    {
        RationalResult* r = dynamic_cast<RationalResult*>(el.get());
        cast_units = r->cast_units;
    }
}

uint Document::SetUnit(ElementId _id, yutovo_calculator::Unit& unit, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, unit, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
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
    //id could be changed
    auto it = changed_ids.find(_id);
    if (it != changed_ids.end())
        _id = it->second;

    tasks.emplace_back(new ResultTask(text, _id, result));

#ifdef DEBUG
    if ((result.type != ResultType::NONE && result.error.error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR) ||    
        result.error.error_code == yutovo_solver::ErrorCode::PARSER_ERROR)
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

void Document::GetSolverGuid(std::string& guid)
{
    guid = solver.guid;
}

uint Document::SetLocale(const yutovo_calculator::Language language, bool with_undo)
{
    Config c = config;
    c.language = language;
    return SetConfig(c, with_undo);
}

void Document::ListIdentifiers(const uint code_id)
{
    solver.ListIdentifiers(code_id);
}

void Document::ElementIdChanged(ElementId last_id, ElementId new_id)
{
    auto it = std::find_if(changed_ids.begin(), changed_ids.end(), 
        [last_id](const auto& p)
        {
            return p.second == last_id;
        });
    if (it != changed_ids.end())
        changed_ids.erase(it);
    changed_ids[last_id] = new_id;
}

void Document::RemoveChangedId(ElementId _id)
{
    auto it = std::find_if(changed_ids.begin(), changed_ids.end(), 
        [_id](const auto& p)
        {
            return p.second == _id;
        });
    if (it != changed_ids.end())
        changed_ids.erase(it);
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
    {
        //id could be changed
        for (auto& p : changed_ids)
        {
            if (IsChild(p.second, _id))
            {
                _id = GetWithParent(_id, p.first);
                break;
            }
        }
        it = std::find_if(error_marks.begin(), error_marks.end(), 
            [_id](const ErrorMark& m)
            {
                return m.id == _id;
            });
        if (it == error_marks.end())
            return false;
    }
    start = it->start;
    size = it->size;
    return true;
}

bool Document::HasErrorMarks(ElementId _id)
{
    int start, size;
    if (HasErrorMark(_id, start, size))
        return true;
    auto el = GetElement(_id);
    if (!el || IsString(el))
        return false;
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        if (HasErrorMarks(el->elements->Get(i)->id))
            return true;
    }
    return false;
}

void Document::SetIdentifiers(const uint code_id, const std::vector<std::string>& variables, const std::vector<std::string>& functions, 
    std::vector<std::string>& units)
{
    {
        std::lock_guard<std::recursive_mutex> lock(identifiers_mutex);
        identifiers[code_id] = Identifiers{variables, functions, units};
    }
    
    std::vector<ElementId> code_blocks;
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    text->GetElements(ElementType::CODE_BLOCK, code_blocks); //find all code blocks
    for (ElementId _id : code_blocks)
    {
        auto el = GetElement(_id);
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        if (c && c->code_id == code_id)
            Redraw(_id, false);
    }
}

IdentifierType Document::FindIdentifier(const uint code_id, const std::string& str)
{
    std::lock_guard<std::recursive_mutex> lock(identifiers_mutex);
    auto it = identifiers.find(code_id);
    if (it == identifiers.end())
        return IdentifierType::NONE;
    Identifiers& id = it->second;
    if (std::find(id.variables.begin(), id.variables.end(), str) != id.variables.end())
        return IdentifierType::VARIABLE;
    if (std::find(id.functions.begin(), id.functions.end(), str) != id.functions.end())
        return IdentifierType::FUNCTION;
    if (std::find(id.units.begin(), id.units.end(), str) != id.units.end())
        return IdentifierType::UNIT;
    return IdentifierType::NONE;
}

void Document::WaitTask(uint task_id, uint64_t timeout, uint64_t circle_delay)
{
    if (task_id == 0)
        return;
    auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    auto cur_time = now;
    while (cur_time - now <= timeout * 1ms)
    {
        {
            std::lock_guard<std::recursive_mutex> lock(edit_mutex);
            if (std::find(last_tasks.begin(), last_tasks.end(), task_id) != last_tasks.end())
                return;
        }

        std::this_thread::sleep_for(circle_delay * 1ms);

        if (timeout > 0)
            cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
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

void Document::UpdateChanged()
{
    if (undo_tasks.empty())
    {
        changed = false;
    }
    else
    {
        TaskPtr t = undo_tasks.back();
        changed = !(t->id == save_task_id);
    }
    window->OnDocumentChanged(changed);
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
