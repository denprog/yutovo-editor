#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <memory>
#include <sstream>
#include "caret_state.h"
#include "selection.h"
#include "editor_state.h"
#include "caret.h"
#include "solver.h"

namespace yutovo
{

class Logger;

struct Task
{
    Task(ElementPtr _text);
    Task(ElementPtr _text, const uint _id);

    virtual bool Execute() = 0;

    ElementPtr text;
    Document* document;
    Window* window;
    bool undo = false; //this is an undo task
    bool with_undo = false; //this task has (will have) undo

    Logger* logger;

    static uint next_id;
    uint id; //for syncing with undo/redo
};

typedef std::shared_ptr<Task> TaskPtr;

struct InsertElementsTask : Task
{
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo, bool _pasting = false);
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, uint _id);
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, uint _id, ElementId _element_id);

    virtual bool Execute();

    ElementId element_id; //insert into this element or use id from caret state
    std::vector<ElementPtr> elements;
    EditorState before_state;
    bool pasting = false; //this is pasting from clipboard
};

struct DeleteElementsTask : Task
{
    DeleteElementsTask(ElementPtr _text, bool _left, bool _with_undo);
    DeleteElementsTask(ElementPtr _text, bool _left, uint _id);
    DeleteElementsTask(ElementPtr _text, ElementId _element_id, bool _with_undo);
    DeleteElementsTask(ElementPtr _text, ElementId _element_id, bool _with_undo, uint _id);

    virtual bool Execute();

    ElementId element_id; //delete elements from this element or use id from caret state
    bool left; //delete on the left or on the right
    EditorState before_state;
};

struct InsertFormulasTask : Task
{
    InsertFormulasTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo, bool _pasting = false);
    InsertFormulasTask(ElementPtr _text, uint _id, std::vector<ElementPtr>& _elements, bool _with_undo);

    virtual bool Execute();

    std::vector<ElementPtr> elements;
    EditorState before_state;
    bool pasting = false; //this is pasting from clipboard
};

struct ChangeStringFormatTask : Task
{
    ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, bool _with_undo);
    ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, bool _set_family, bool _set_size, bool _set_bold, bool _set_italic, 
        bool _set_underline, bool _with_undo);
    ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, uint _id);

    virtual bool Execute();

    StringFormatPtr format;

    bool set_family = true;
    bool set_size = true;
    bool set_bold = true;
    bool set_italic = true;
    bool set_underline = true;

    EditorState before_state;
};

struct ChangeParagraphFormatTask : Task
{
    ChangeParagraphFormatTask(ElementPtr _text, const ParagraphFormatPtr& _format, bool _with_undo);
    ChangeParagraphFormatTask(ElementPtr _text, const ParagraphFormatPtr& _format, uint _id);

    virtual bool Execute();

    ParagraphFormatPtr format;

    EditorState before_state;
};

struct RemakeTask : Task
{
    RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements, bool _with_undo);
    RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements, bool _with_undo, bool _move_into_view, uint id);

    virtual bool Execute();

    ElementId element_id;
    bool with_elements;
    bool with_undo;
    bool move_into_view = false;
};

struct RedrawTask : Task
{
    RedrawTask(ElementPtr _text, const ElementId& _id, bool _move_into_view);

    virtual bool Execute();

    ElementId element_id;
    bool move_into_view;
};

struct ResizeTask : Task
{
    ResizeTask(ElementPtr _text, const uint _width, const uint _height);

    virtual bool Execute();

    uint width;
    uint height;
};

typedef std::function<void (const ElementId)> CallFuncPtr;

struct CallFuncTask : Task
{
    CallFuncTask(ElementPtr _text, const ElementId& _id, CallFuncPtr _func, const uint task_id);

    virtual bool Execute();

    const ElementId id;
    CallFuncPtr func;
};

struct MoveCaretTask : Task
{
    enum class MoveCaretDir
    {
        NONE = 0,
        POINT,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        HOME,
        END,
        WORD_LEFT,
        WORD_RIGHT,
        DOCUMENT_BEGIN,
        DOCUMENT_END
    };

    MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible, bool _select);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible, bool _select, uint _task_id);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, Point _point);

    virtual bool Execute();

    Document* document;
    CaretPtr caret;
    MoveCaretDir dir = MoveCaretDir::NONE;
    Point point{-1, -1};
    bool visible = false;
    bool select = false;
};

struct SetEditorStateTask : Task
{
    SetEditorStateTask(ElementPtr _text, const CaretState& _caret_state, const SelectionState& _selection_state, const uint task_id);

    virtual bool Execute();

    CaretState caret_state;
    SelectionState selection_state;
};

struct NewTask : Task
{
    NewTask(ElementPtr _text);

    virtual bool Execute();
};

class Text;
class Page;
class Paragraph;
class Row;
class String;
class StringElements;

struct SaveTask : Task
{
    SaveTask(ElementPtr _text, const std::string _filename);

    virtual bool Execute();

    std::string filename;
};

struct LoadTask : Task
{
    LoadTask(ElementPtr _text, const std::string _filename);

    virtual bool Execute();

    std::string filename;
};

struct CopyTask : Task
{
    CopyTask(ElementPtr _text, std::stringstream& _out_array, std::string& _out_text, bool _cut);

    virtual bool Execute();

    std::stringstream& out_array;
    std::string& out_text;
    bool cut;
};

struct ResultTask : Task
{
    ResultTask(ElementPtr _text, ElementId _id, Result _result);

    virtual bool Execute();

    ElementId id;
    Result result;
};

struct ResolveTask : Task
{
    ResolveTask(ElementPtr _text, ElementId _id);

    virtual bool Execute();

    ElementId id;
};

}

#endif
