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
    RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements);
    RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements, uint id);

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

struct SerializeTask : Task
{
    SerializeTask(ElementPtr _text);

 	template<class Archive>
	void RegisterTypes(Archive& archive)
    {
        boost::serialization::void_cast_register<yutovo::Text, yutovo::Element>(static_cast<yutovo::Text*>(NULL), static_cast<yutovo::Element*>(NULL));
        boost::serialization::void_cast_register<yutovo::Page, yutovo::Element>(static_cast<yutovo::Page*>(NULL), static_cast<yutovo::Element*>(NULL));
        boost::serialization::void_cast_register<yutovo::Paragraph, yutovo::Element>(static_cast<yutovo::Paragraph*>(NULL), static_cast<yutovo::Element*>(NULL));
        boost::serialization::void_cast_register<yutovo::Row, yutovo::Element>(static_cast<yutovo::Row*>(NULL), static_cast<yutovo::Element*>(NULL));
        boost::serialization::void_cast_register<yutovo::String, yutovo::Element>(static_cast<yutovo::String*>(NULL), static_cast<yutovo::Element*>(NULL));
        boost::serialization::void_cast_register<yutovo::StringElements, yutovo::Elements>(static_cast<yutovo::StringElements*>(NULL), static_cast<yutovo::Elements*>(NULL));

        archive.template register_type<yutovo::Text>();
        archive.template register_type<yutovo::Page>();
        archive.template register_type<yutovo::Paragraph>();
        archive.template register_type<yutovo::Row>();
        archive.template register_type<yutovo::String>();
        archive.template register_type<yutovo::StringElements>();
    }
};

struct SaveTask : SerializeTask
{
    SaveTask(ElementPtr _text, const std::string _filename);

    virtual bool Execute();

    std::string filename;
};

struct LoadTask : SerializeTask
{
    LoadTask(ElementPtr _text, const std::string _filename);

    virtual bool Execute();

    std::string filename;
};

}

#endif
