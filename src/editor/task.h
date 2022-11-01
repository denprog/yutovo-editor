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

    virtual bool Execute() = 0;

    ElementPtr text;
    bool undo = false; //this is an undo task
    bool with_undo = false; //this task has (will have) undo

    Logger* logger;
};

typedef std::shared_ptr<Task> TaskPtr;

struct InsertElementsTask : Task
{
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, const CaretState& _before_state, CaretState& _after_state, bool _with_undo);

    virtual bool Execute();

    std::vector<ElementPtr> elements;
    CaretState before_state;
    CaretState after_state; //may be empty
};

struct DeleteElementsTask : Task
{
    DeleteElementsTask(ElementPtr _text, const CaretState& _before_state, CaretState& _after_state, bool _with_undo);

    virtual bool Execute();

    CaretState before_state;
    CaretState after_state; //may be empty
};

struct SplitElementTask : Task
{
    SplitElementTask(ElementPtr _text, ElementId _id, ElementId _remake_id, const uint _max_left_width);
    SplitElementTask(ElementPtr _text, ElementId _id, ElementId _remake_id, const int _pos);

    virtual bool Execute();

    ElementId id; //element for splitting
    ElementId remake_id; //remake this element after split
    uint max_left_width = 0; //split by width
    int pos = -1; //split by pos
};

struct MergeElementsTask : Task
{
    MergeElementsTask(ElementPtr _text, ElementId _id1, ElementId _id2, ElementId _remake_id);

    virtual bool Execute();

    ElementId id1;
    ElementId id2;
    ElementId remake_id;
};

struct RemakeTask : Task
{
    RemakeTask(ElementPtr _text, const ElementId& _id, bool _with_elements);

    virtual bool Execute();

    ElementId id;
    bool with_elements;
};

struct RedrawTask : Task
{
    RedrawTask(ElementPtr _text, const ElementId& _id);

    virtual bool Execute();

    ElementId id;
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
        END
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
