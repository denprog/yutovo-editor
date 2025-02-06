#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <thread>
#include <mutex>
#include <vector>
#include <stack>
#include <memory>
#include <map>
#include "window.h"
#include "caret.h"
#include "selection.h"
#include "text.h"
#include "task.h"
#include "solver.h"
#include "editor_utils.h"
#include "editor_state.h"
#include "config.h"
#include "undo.h"
#include <yutovo_logger/logger.h>
#include <yutovo_solver/types.h>
#include <yutovo_calculator/math_helper.h>

namespace yutovo
{

class Document
{
public:
    Document(Window* _window, Config& _config);
    ~Document();

    void Start();

    void GetConfig(Config& _config);
    uint SetConfig(const Config& _config, bool with_undo);
    uint SetConfig(const std::string& _config, bool with_undo);

    uint InsertParagraph(bool with_undo);
    uint InsertString(const std::string& str, bool with_undo);
    uint InsertString(const std::u32string& str, bool with_undo);
    uint InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo);
    uint InsertString(const std::string& str, ElementId element_id, bool with_undo);

    uint InsertLink(const std::string& str, const std::string& url, bool with_undo);
    uint InsertLink(const std::u32string& str, const std::u32string& url, bool with_undo);

    uint InsertElement(Element* element, bool with_undo, ElementId element_id = ElementId{}, bool pasting = false);
    uint InsertElements(std::vector<ElementPtr>& elements, bool with_undo, ElementId element_id = ElementId{}, 
        bool pasting = false);

    uint DeleteElements(bool left, bool with_undo);
    uint ClearElements(ElementId element_id, bool with_undo);

    uint InsertCode(bool next_code_id, bool with_undo);
    uint InsertCodeString(const std::string& str, bool with_undo);
    uint InsertPlus(bool with_undo);
    uint InsertMinus(bool with_undo);
    uint InsertMultiply(bool with_undo);
    uint InsertDivision(bool with_undo);
    uint InsertPower(bool with_undo);
    uint InsertNthRoot(bool with_undo);
    uint InsertSquareRoot(bool with_undo);
    uint InsertEquation(yutovo_solver::ResultType result_type, bool with_undo);
    uint InsertOpenFence(bool with_undo);
    uint InsertCloseFence(bool with_undo);
    uint InsertAssignment(bool with_undo);
    uint InsertUnit(bool with_undo);
    uint InsertSubscript(bool with_undo);
    uint InsertExclamation(bool with_undo);
    uint InsertAnd(bool with_undo);
    uint InsertOr(bool with_undo);
    uint InsertXor(bool with_undo);
    uint InsertPercent(bool with_undo);
    uint InsertSum(bool with_undo);
    uint InsertProduct(bool with_undo);
    uint InsertImage(const std::string& image_base64, bool with_undo, bool pasting);
    uint InsertImage(const std::vector<unsigned char>& image, bool with_undo, bool pasting);
    uint InsertComma(bool with_undo);

    uint InsertFences(bool with_undo);
    
    uint InsertFunction(const std::string& name, bool with_undo);
    uint InsertSubscriptFunction(const std::string& name, bool with_undo);

    uint InsertFormula(Element* element, bool with_undo, bool with_last_task_id = false);
    uint InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool with_last_task_id = false, bool pasting = false, int select_pos = -1);

    uint InsertUnit(const yutovo_calculator::Unit& unit, bool list_identifiers = true);

    uint ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, const bool strikethrough, 
        Color text_color, Color text_bg_color, bool with_undo);
    uint ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
        bool set_strikethrough, bool set_text_color, bool set_text_bg_color, bool with_undo);
    uint ChangeStringFormat(const StringFormatPtr format, bool with_undo);

    uint ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo);
    uint ChangeParagraphFormat(const std::string name, bool with_undo);
    uint ChangeParagraphFormat(const ParagraphFormat::Alignment alignment, bool with_undo);

    bool StoreUndo(const ElementId& _id);
    bool StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size = 0);
    bool StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size, UndoTask::UndoOperation undo_operation);
    bool StoreUndo(const Config& config);
    bool RestoreUndo(const int undo_id, std::vector<ElementPtr>& elements);
    bool RestoreUndo(const int undo_id, Config& config);
    void RollbackUndo();
    size_t GetUndoSize();

    void ResetTasks();

    ElementPtr GetElement(const ElementId& _id);
    ElementPtr GetLogicalElement(const LogicalId& _id);
    void GetElements(const LogicalId& _id, std::vector<ElementPtr>& elements);
    ElementPtr GetParent(const ElementId& _id);
    ElementPtr GetLogicalParent(const LogicalId& _id);
    bool GetElementAtCoords(const int x, const int y, ElementId& id);
    bool GetElementRect(const ElementId id, Rect& rect);

    LogicalId GetLogicalId(const ElementId& _id);
    LogicalId GetLogicalId(const ElementId& _id, const int pos);
    ElementId GetElementId(const LogicalId& _id, bool& last_pos);
    ElementId GetElementId(const LogicalId& _id, const int pos, bool& last_pos);

    ElementPtr FindElementOrParent(const ElementId& id, const ElementType type);
    ElementPtr FindParent(const ElementId& id, const ElementType type);
    ElementId FindCurrentParentByType(const ElementType type);
    ElementPtr FindParentParagraph(const ElementId& id);
    ElementPtr FindParentRow(const ElementId& id);
    uint FindCodeBlock(const ElementId& id);
    ElementPtr FindByString(const ElementId& start_id, const std::u32string& str);
    ElementPtr FindByType(const ElementId& start_id, const ElementType type);

    bool GetLink(const ElementId& id, std::u32string& str, std::u32string& url);

    Rect GetCaretRect(const CaretState& caret_state);

    bool GetCurrentStringFormat(StringFormatPtr& format);
    void SetCurrentStringFormat(StringFormatPtr& format);

    bool GetCurrentParagraphFormat(ParagraphFormatPtr& format);
    uint SetCurrentParagraphFormat(const std::string& name);

    bool GetCurrentFormulaFormat(FormulaFormatPtr& format);

    ElementType GetElementType(const ElementId id);
    bool IsEditable(const ElementId id);

    bool IsEmpty();

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
    uint MoveCaret(const int x, const int y, bool ctrl = false);
    uint SelectAll();
    uint Select(const int start_x, const int start_y, const int end_x, const int end_y);
    uint SelectOut();

    void SetCaretVisible(bool visible);

    void Undo();
    void Redo();

    bool CanUndo();
    bool CanRedo();

    bool IsChanged();

    uint Resize(uint width, uint height);

    uint Redraw(const ElementId& id, bool move_into_view);
    void Redraw();

    bool WillRedraw(const ElementId& id, bool move_into_view);
    bool WillResize();

    uint New();
    uint Save(const std::string& filename);
    uint SaveJson(std::u32string& json, const int document_id);
    uint Load(const std::string& filename);
    uint LoadJson(const std::u32string& json_doc, const int document_id);

    uint Copy(std::u32string& out_json, std::u32string& out_text);
    uint Paste(std::u32string& in_json);
    uint PasteText(std::u32string&& str);
    uint PasteImage(const std::vector<unsigned char>& image);
    uint PasteImage(const std::string& image_base64);
    uint Cut(std::u32string& out_json, std::u32string& out_text);

    std::string ToHtml();
    std::u32string ToText();

    TextFormatPtr GetDefaultTextFormat();
    PageFormatPtr GetDefaultPageFormat();
    uint SetDefaultPageFormat(uint left_indent, uint top_indent, uint right_indent, uint bottom_indent, uint paragraph_spacing);
    StringFormatPtr GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough);
    StringFormatPtr GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
        Color text_color, Color text_bg_color);
    StringFormatPtr GetStringFormat(const boost::uuids::uuid& id);

    void UpdateFormats();

    uint SetFontFamily(const std::string& family);
    uint SetFontSize(const uint size);
    uint SetBold(const bool enabled);
    uint SetItalic(const bool enabled);
    uint SetUnderline(const bool enabled);
    uint SetStrikethrough(const bool enabled);
    uint SetColor(const Color color);
    uint SetBgColor(const Color color);

    ElementPtr CreateParagraph(const ElementId& id);

    EditorState GetEditorState();
    EditorState MakeEditorState();
    LogicalEditorState GetLogicalEditorState();
    void SetEditorState(EditorState& state);
    void SetEditorState(LogicalEditorState& state);

    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::AutoResultConfig& auto_config, std::u32string& expression, 
        const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::RealResultConfig& config, const std::u32string& expression, 
        const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::IntegerResultConfig& config, const std::u32string& expression, 
        const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::RationalResultConfig& config, const std::u32string& expression, 
        const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::ComplexResultConfig& config, const std::u32string& expression, 
        const uint delay);
    void BreakSolving(const LogicalId& _id, const std::string& guid, uint code_id);

    void SetIdentifier(const LogicalId& _id, const std::string& guid, uint code_id, const std::u32string& identifier, 
        const std::u32string& expression, const uint delay);
    void RemoveIdentifier(const LogicalId& _id, uint code_id, const std::u32string& identifier, const uint delay);
    void RemoveUserIdentifiers();

    ResultType GetResultType(ElementId _id);
    uint SetResultType(ElementId _id, ResultType result_type, bool with_undo);

    int GetPrecision(ElementId _id);
    uint SetPrecision(ElementId _id, uint precision, bool with_undo);

    int GetExp(ElementId _id);
    uint SetExp(ElementId _id, uint exp, bool with_undo);

    AngleMeasure GetDefaultAngleMeasure(ElementId _id);
    AngleMeasure GetResultAngleMeasure(ElementId _id);
    uint SetAngleMeasure(ElementId _id, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo);

    Notation GetResultNotation(ElementId _id);
    Notation GetDefaultNotation(ElementId _id);
    uint SetNotation(ElementId _id, Notation default_notation, Notation result_notation, bool with_undo);

    FractionForm GetFractionForm(ElementId _id);
    uint SetFractionForm(ElementId _id, FractionForm fraction_form, bool with_undo);

    ComplexForm GetComplexForm(ElementId _id);
    uint SetComplexForm(ElementId _id, ComplexForm form, bool with_undo);

    bool HasUnit(ElementId _id);
    void GetCastUnits(ElementId _id, std::vector<yutovo_calculator::Unit>& cast_units);
    uint SetUnit(ElementId _id, yutovo_calculator::Unit& unit, bool with_undo);

    uint ReSolve(const ElementId& _id);
    void ReSolve(const LogicalId& _id);
    uint ReSolveDependencies(const LogicalId& after_id, const std::u32string& identifier);
    uint ReSolveErrors();
    uint PutResult(const std::string& guid, const Result& result);
    void AddResolveElement(const ElementId& _id);
    void AddChangedElement(const ElementId& _id);
    void GetSolverGuid(std::string& guid);
    uint SetLocale(const yutovo_calculator::Language language, bool with_undo);
    void ListIdentifiers(const uint code_id);

    void UpdateSolveId(const std::string& guid, const LogicalId& new_id);

    bool IsVisible(ElementId _id);
    ElementId GetFirstVisibleParagraph();
    ElementId GetFirstVisibleRow(ElementId paragraph_id);

    void AddErrorMark(const ElementId& _id, int start, int size);
    void RemoveErrorMarks(const ElementId& parent_id);
    bool HasErrorMark(const ElementId& _id, int& start, int& size);
    bool HasErrorMarks(const ElementId& _id);

    void SetIdentifiers(const uint code_id, const std::vector<std::string>& variables, const std::vector<std::string>& functions, 
        std::vector<std::string>& units);
    IdentifierType FindIdentifier(const uint code_id, const std::string& str);

    void WaitTask(uint task_id, uint64_t timeout = 0, uint64_t circle_delay = 1);

private:
    void MainLoop();

    void RestrictUndo();

    void UpdateChanged();

#ifdef DEBUG
public:
    void WaitMainLoop();
    void WaitUndo();
    void WaitRedo();
    void WaitLoad();
    void WaitSolver();
#endif

private:
    int last_task_id = -1;
    int last_load_task_id = -1;
    bool last_task_executed = false;
    bool last_load_executed = false;
    bool last_undo_executed = false;
    bool last_redo_executed = false;

    int last_solver_task_id = -1;
    bool last_solver_executed = false;

    std::deque<uint> last_tasks;
    const int last_tasks_count = 100;

    int last_modify_task_id = 0;

private:
    friend class MoveCaretTask;
    friend class SetEditorStateTask;
    friend class GetEditorStateTask;
    friend class NewTask;
    friend class LoadTask;
    friend class SaveTask;
    friend class RedrawTask;
    friend class SetConfigTask;
    friend class DeleteElementsTask;
    friend class Caret;
    friend class Element;

    void UpdateCaretView();
    void UpdateLastSelection();

private:
    std::recursive_mutex tasks_mutex;
    std::recursive_mutex solver_tasks_mutex;
    std::recursive_mutex edit_mutex;
    std::recursive_mutex state_mutex;
    std::recursive_mutex last_tasks_mutex;

public:
    Window* window;

    StringFormatsPtr string_formats;
    ParagraphFormatsPtr paragraph_formats;
    CodeFormatsPtr code_formats;
    FormulaFormatsPtr formula_formats;

    PageFormatPtr current_page_format;

private:
    StringFormatPtr current_string_format;
    ParagraphFormatPtr current_paragraph_format;
    CodeFormatPtr current_code_format;
    FormulaFormatPtr current_formula_format;

    ElementPtr text;

public:
    CaretPtr caret;
    Selection selection;

    bool pasting = false;
    bool saving = false;

    uint cur_code_id = 1;

    Config config;

    std::vector<ElementId> changed_elements;
    std::vector<ElementId> resolve_elements;

    std::map<std::string, LogicalId> solve_ids;

    std::atomic_bool break_remake = false;

    std::atomic<bool> changed{false};
    uint save_task_id = 0;

private:
    std::list<TaskPtr> tasks;
    std::deque<TaskPtr> undo_tasks;
    std::vector<TaskPtr> redo_tasks;
    std::vector<bool> undos; //requieres for undo
    std::vector<bool> redos; //requieres for redo

    struct Identifiers
    {
        std::vector<std::string> variables;
        std::vector<std::string> functions;
        std::vector<std::string> units;
    };

    std::recursive_mutex identifiers_mutex;
    std::map<uint, Identifiers> identifiers; //by code_id

    Solver solver;

    bool exit = false;

    std::atomic<bool> next_circle{false};
    std::thread main_loop;

    uint cur_task_id = 0;

    ElementId cur_visible_row; //any row which is visible

    UndoBase undo_base;

    EditorState last_editor_state;

    Selection last_selection;

    SelectionState last_editor_selection;

    Logger* logger;
};

typedef std::shared_ptr<Document> DocumentPtr;

}

#endif
