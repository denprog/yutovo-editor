#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <thread>
#include <mutex>
#include <vector>
#include <stack>
#include <memory>
#include "window.h"
#include "caret.h"
#include "selection.h"
#include "text.h"
#include "task.h"
#include "solver.h"
#include "util.h"
#include "editor_state.h"
#include "config.h"
#include "undo.h"
#include "logger.h"
#include <yutovo_service/types.h>

namespace yutovo
{

class Document
{
public:
    Document(Window* _window);
    ~Document();

    void Start(Config& _config);

    void GetConfig(Config& _config);
    void SetConfig(const Config& _config);

    uint InsertParagraph(bool with_undo, bool undo = false);
    uint InsertString(const std::string& str, bool with_undo);
    uint InsertString(const std::u32string& str, bool with_undo);
    uint InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo);
    uint InsertString(const std::string& str, const StringFormatPtr string_format, ElementId element_id);

    uint InsertElement(Element* element, bool with_undo, bool undo = false, ElementId element_id = ElementId{});
    uint InsertElement(Element* element, ElementId element_id = ElementId{});
    uint InsertElement(ElementPtr element, ElementId element_id = ElementId{});
    uint InsertElements(std::vector<ElementPtr>& elements, bool with_undo, bool undo = false, ElementId element_id = ElementId{}, 
        bool pasting = false);

    uint DeleteElements(bool left, bool with_undo, bool undo);
    uint ClearElements(ElementId element_id, bool with_undo, bool undo);

    uint InsertCode(bool next_code_id, bool with_undo);
    uint InsertCodeString(const std::string& str, bool with_undo);
    uint InsertPlus(bool with_undo);
    uint InsertMinus(bool with_undo);
    uint InsertMultiply(bool with_undo);
    uint InsertDivision(bool with_undo);
    uint InsertPower(bool with_undo);
    uint InsertNthRoot(bool with_undo);
    uint InsertSquareRoot(bool with_undo);
    uint InsertEquation(yutovo_service::ResultType result_type, bool with_undo);
    uint InsertOpenFence(bool with_undo);
    uint InsertCloseFence(bool with_undo);
    uint InsertAssignment(bool with_undo);
    uint InsertSubscript(bool with_undo);

    uint InsertFences(bool with_undo);
    
    uint InsertFunction(const std::string& name, bool with_undo);
    uint InsertSubscriptFunction(const std::string& name, bool with_undo);

    uint InsertFormula(Element* element, bool with_undo, bool undo, bool with_last_task_id = false);
    uint InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool undo, bool with_last_task_id = false, bool pasting = false);

    uint ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, bool with_undo, bool undo);
    uint ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
        bool with_undo);
    uint ChangeStringFormat(const StringFormatPtr format, bool with_undo, bool undo);

    uint ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, bool undo);

    void PushEditorState(bool undo);
    void PushEditorState(const EditorState& editor_state, bool undo);
    void PushEditorState(const CaretState& caret_state, bool undo);
    void PushEditorState(const SelectionState& selection_state, bool undo);
    void PushEditorState(const CaretState& caret_state, const SelectionState& selection_state, bool undo);

    bool StoreUndo(const ElementId& _id);
    bool StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size = 0);
    bool StoreUndo(const ElementId& parent_id, const int pos, const int size, UndoTask::UndoOperation undo_operation);
    bool RestoreUndo(const int undo_id, std::vector<ElementPtr>& elements);

    void ResetTasks();

    ElementPtr GetElement(const ElementId& _id);
    ElementPtr GetLogicalElement(const LogicalId& _id);
    void GetElements(const LogicalId& _id, std::vector<ElementPtr>& elements);
    ElementPtr GetParent(const ElementId& _id);
    ElementPtr GetLogicalParent(const LogicalId& _id);
    bool GetElementAtCoords(const int x, const int y, ElementId& id);

    LogicalId GetLogicalId(const ElementId& _id);
    LogicalId GetLogicalId(const ElementId& _id, const int pos);
    ElementId GetElementId(const LogicalId& _id, bool& last_pos);
    ElementId GetElementId(const LogicalId& _id, const int pos, bool& last_pos);

    ElementPtr FindParent(const ElementId& id, const ElementType type);
    ElementId FindCurrentParentByType(const ElementType type);
    ElementPtr FindParentParagraph(const ElementId& id);
    ElementPtr FindParentRow(const ElementId& id);

    Rect GetCaretRect(const CaretState& caret_state);

    bool GetCurrentStringFormat(StringFormatPtr& format);
    void SetCurrentStringFormat(StringFormatPtr& format);

    bool GetCurrentParagraphFormat(ParagraphFormatPtr& format);

    bool GetCurrentFormulaFormat(FormulaFormatPtr& format);

    ElementType GetElementType(const ElementId id);

    bool IsString(ElementPtr el);
    bool IsString(ElementId id);
    bool IsRow(ElementPtr el);
    bool IsRow(ElementId id);
    bool IsParagraph(ElementPtr el);
    bool IsParagraph(ElementId id);
    bool IsFormula(ElementPtr el);

    bool GetStringFormat(const ElementId id, StringFormat& format);
    bool GetParagraphFormat(const ElementId id, ParagraphFormat& format);

    uint MoveCaret(MoveCaretTask::MoveCaretDir dir, bool select, bool with_last_task_id = false, bool move_into_view = true);
    uint MoveCaretLeft(bool select, bool with_last_task_id = false);
    uint MoveCaretRight(bool select, bool with_last_task_id = false);
    uint MoveCaretUp(bool select);
    uint MoveCaretDown(bool select);
    uint MoveCaretHome(bool select);
    uint MoveCaretEnd(bool select);
    uint MoveCaretPageUp(bool select);
    uint MoveCaretPageDown(bool select);
    uint MoveCaretWordLeft(bool select);
    uint MoveCaretWordRight(bool select);
    uint MoveCaretToDocumentBegin(bool select);
    uint MoveCaretToDocumentEnd(bool select);
    uint MoveCaretToDocumentEnd(bool select, bool move_into_view);
    uint MoveCaret(const int x, const int y);
    uint SelectAll();

    void SetCaretVisible(bool visible);

    void Undo();
    void Redo();

    bool CanUndo();
    bool CanRedo();

    void RollbackUndo();

    uint Resize(uint width, uint height);

    void Redraw(const ElementId& id, bool move_into_view);
    void Redraw();

    bool WillRedraw(const ElementId& id, bool move_into_view);
    bool WillResize();

    uint New();
    uint Save(const std::string& filename);
    uint Load(const std::string& filename);

    uint Copy(std::stringstream& out_array, std::u32string& out_text);
    uint Paste(std::stringstream& in_array);
    uint Paste(const std::u32string& str);
    uint Cut(std::stringstream& out_array, std::u32string& out_text);

    std::string ToHtml();
    std::u32string ToText();

    TextFormatPtr GetDefaultTextFormat();
    PageFormatPtr GetDefaultPageFormat();
    StringFormatPtr GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline);
    StringFormatPtr GetStringFormat(const boost::uuids::uuid& id);

    void UpdateFormats();

    uint SetFontFamily(const std::string& family);
    uint SetFontSize(const uint size);
    uint SetBold(const bool enabled);
    uint SetItalic(const bool enabled);
    uint SetUnderline(const bool enabled);

    uint SetCurrentParagraphFormat(const std::string& name);

    EditorState GetEditorState();
    LogicalEditorState GetLogicalEditorState();
    void SetEditorState(EditorState& state);
    void SetEditorState(LogicalEditorState& state);

    void Solve(ElementId _id, uint code_id, Config::AutoResult& auto_config, std::u32string& expression, const uint delay);
    void Solve(ElementId _id, uint code_id, Config::RealResult& config, const std::u32string& expression, const uint delay);
    void Solve(ElementId _id, uint code_id, Config::IntegerResult& config, const std::u32string& expression, const uint delay);
    void Solve(ElementId _id, uint code_id, Config::RationalResult& config, const std::u32string& expression, const uint delay);
    void Solve(ElementId _id, uint code_id, Config::ComplexResult& config, const std::u32string& expression, const uint delay);

    void SetUserIdentifier(ElementId _id, uint code_id, const std::u32string& identifier, const std::u32string& expression, const uint delay);
    void RemoveIdentifier(ElementId _id, uint code_id, const std::u32string& identifier, const uint delay);

    uint SetResult(ElementId _id, ResultType result_type);
    void ReSolve(ElementId _id);
    void ReSolveDependencies(ElementId after_id, const std::u32string& identifier);
    void ReSolveErrors();
    void PutResult(ElementId _id, Result result);

    bool IsVisible(ElementId _id);
    ElementId GetFirstVisibleParagraph();
    ElementId GetFirstVisibleRow(ElementId paragraph_id);

    void AddErrorMark(ElementId _id, int start, int size);
    void RemoveErrorMarks(ElementId parent_id);
    bool HasErrorMark(ElementId _id, int& start, int& size);

    void WaitTask(uint task_id, uint64_t timeout = 0);

private:
    void MainLoop();

    void RestrictUndo();

#ifdef DEBUG
public:
    void WaitMainLoop();
    void WaitUndo();
    void WaitRedo();
    void WaitCaretMoving();
    void WaitLoad();
    void WaitSolver();

private:
    int last_task_id = -1;
    int last_load_task_id = -1;
    bool last_task_executed = false;
    bool last_load_executed = false;
    bool last_undo_executed = false;
    bool last_redo_executed = false;

    bool last_caret_moved = false;

    int last_solver_task_id = -1;
    bool last_solver_executed = false;

    std::vector<uint> last_tasks;
#endif

private:
    friend class MoveCaretTask;
    friend class SetEditorStateTask;
    friend class NewTask;
    friend class LoadTask;
    friend class RedrawTask;
    friend class Caret;

    void UpdateCaretView();
    void UpdateLastSelection();

private:
    std::recursive_mutex tasks_mutex;
    std::recursive_mutex solver_tasks_mutex;
    std::recursive_mutex edit_mutex;

public:
    Window* window;

    StringFormatsPtr string_formats;
    ParagraphFormatsPtr paragraph_formats;
    CodeFormatsPtr code_formats;
    FormulaFormatsPtr formula_formats;

private:
    StringFormatPtr current_string_format;
    ParagraphFormatPtr current_paragraph_format;
    CodeFormatPtr current_code_format;
    FormulaFormatPtr current_formula_format;

    ElementPtr text;

public:
    CaretPtr caret;
    Selection selection;
    Selection last_selection;

    bool pasting = false;

    uint cur_code_id = 1;

    std::vector<ErrorMark> error_marks;

    Config config;

    ElementId changed_element;

private:
    std::list<TaskPtr> tasks;
    std::deque<TaskPtr> undo_tasks;
    std::vector<TaskPtr> redo_tasks;
    std::vector<bool> undos; //requieres for undo
    std::vector<bool> redos; //requieres for redo

    Solver solver;

    bool exit = false;

    std::atomic<bool> next_circle{false};
    std::thread main_loop;

    uint cur_task_id = 0;

    ElementId cur_visible_row; //any row which is visible

    UndoBase undo_base;

    Logger* logger;
};

typedef std::shared_ptr<Document> DocumentPtr;

}

#endif
