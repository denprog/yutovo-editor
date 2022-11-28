#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <memory>
#include "caret_state.h"
#include "selection.h"
#include "editor_state.h"
#include "caret.h"
#include "logger.h"

namespace yutovo
{

struct Task
{
    Task(ElementPtr _text);
    Task(ElementPtr _text, const uint _id);

    virtual bool Execute() = 0;

    ElementPtr text;
    bool undo = false; //this is an undo task
    bool with_undo = false; //this task has (will have) undo

    Logger* logger;

    static uint next_id;
    uint id; //for syncing with undo/redo
};

typedef std::shared_ptr<Task> TaskPtr;

struct InsertElementsTask : Task
{
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo);
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, uint _id);
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, uint _id, ElementId _element_id);

    virtual bool Execute();

    ElementId element_id; //insert into this element or use id from caret state
    std::vector<ElementPtr> elements;
    EditorState before_state;
};

struct DeleteElementsTask : Task
{
    DeleteElementsTask(ElementPtr _text, bool _left, bool _with_undo);
    DeleteElementsTask(ElementPtr _text, bool _left, uint _id);

    virtual bool Execute();

    ElementId element_id; //delete from this element or use id from caret state
    bool left; //delete on the left or on the right
    EditorState before_state;
};

struct ChangeStringFormatTask : Task
{
    ChangeStringFormatTask(ElementPtr _text, const StringFormat& _format);

    virtual bool Execute();

    StringFormat format;
};

struct ChangeParagraphFormatTask : Task
{
    ChangeParagraphFormatTask(ElementPtr _text, const ParagraphFormat& _format);

    virtual bool Execute();

    ParagraphFormat format;
};

struct RemakeTask : Task
{
    RemakeTask(ElementPtr _text, const ElementId& _id, bool _with_elements);

    virtual bool Execute();

    ElementId element_id;
    bool with_elements;
};

struct RedrawTask : Task
{
    RedrawTask(ElementPtr _text, const ElementId& _id);

    virtual bool Execute();

    ElementId element_id;
};

struct ResizeTask : Task
{
    ResizeTask(ElementPtr _text, const uint _width, const uint _height);

    virtual bool Execute();

    uint width;
    uint height;
};

struct MoveCaretTask : Task
{
    enum class MoveCaretDir
    {
        NONE = 0,
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

    MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible);
    MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible, bool _select);
    MoveCaretTask(ElementPtr _text, Caret* _caret, Point _point);

    virtual bool Execute();

    Document* document;
    Caret* caret;
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

}

#endif
