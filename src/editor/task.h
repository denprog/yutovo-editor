#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <memory>
#include "caret_state.h"
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
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, bool _with_undo);
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, uint _id);
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, uint _id, 
        ElementId _element_id);

    virtual bool Execute();

    ElementId element_id; //insert into this element or use id from before_state
    std::vector<ElementPtr> elements;
    CaretState before_state;
    CaretState after_state; //may be empty
};

struct DeleteElementsTask : Task
{
    DeleteElementsTask(ElementPtr _text, const CaretState& _before_state, CaretState& _after_state, bool _left, bool _with_undo);
    DeleteElementsTask(ElementPtr _text, const CaretState& _before_state, CaretState& _after_state, bool _left, uint _id);

    virtual bool Execute();

    ElementId element_id; //delete from this element or use id from before_state
    CaretState before_state;
    CaretState after_state; //may be empty
    bool left; //delete on the left or on the right
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
    MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible, bool _selection);
    MoveCaretTask(ElementPtr _text, Caret* _caret, Point _point);

    virtual bool Execute();

    Caret* caret;
    MoveCaretDir dir = MoveCaretDir::NONE;
    Point point{-1, -1};
    bool visible = false;
    bool selection = false;
};

}

#endif
