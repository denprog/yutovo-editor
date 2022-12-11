#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <stack>
#include <memory>
#include "window.h"
#include "caret.h"
#include "selection.h"
#include "text.h"
#include "task.h"
#include "util.h"
#include "editor_state.h"
#include "logger.h"

namespace yutovo
{

class Document
{
public:
    Document(Window* _window);
    ~Document();

    void InsertPage(bool with_undo);
    void InsertParagraph(bool with_undo, bool undo = false);
    void InsertText(const std::string& str, bool with_undo);
    void InsertText(const std::string& str, const StringFormatPtr string_format, bool with_undo);
    void InsertText(const std::string& str, const StringFormatPtr string_format, ElementId element_id);

    void InsertElement(Element* element, bool with_undo, bool undo = false, ElementId element_id = ElementId{});
    void InsertElement(Element* element, ElementId element_id = ElementId{});
    void InsertElements(std::vector<ElementPtr>& elements, bool with_undo, bool undo = false, ElementId element_id = ElementId{});

    void DeleteElements(bool left, bool with_undo, bool undo);

    void ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, bool with_undo, bool undo);
    void ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
        bool with_undo);
    void ChangeStringFormat(const StringFormatPtr format, bool with_undo, bool undo);

    void ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo, bool undo);

    void PushEditorState(bool undo);
    void PushEditorState(const CaretState& caret_state, bool undo);
    void PushEditorState(const SelectionState& selection_state, bool undo);
    void PushEditorState(const CaretState& caret_state, const SelectionState& selection_state, bool undo);

    void ResetTasks();

    ElementPtr GetElement(const ElementId& _id);
    ElementPtr GetParent(const ElementId& _id);

    ElementPtr FindParent(const ElementId& id, const ElementType type);

    Rect GetCaretRect(const CaretState& caret_state);

    bool GetCurrentStringFormat(StringFormatPtr& format);
    void SetCurrentStringFormat(StringFormatPtr& format);

    ElementType GetElementType(const ElementId id);

    bool GetStringFormat(const ElementId id, StringFormat& format);
    bool GetParagraphFormat(const ElementId id, ParagraphFormat& format);

    void MoveCaret(MoveCaretTask::MoveCaretDir dir, bool select);
    void MoveCaretLeft(bool select);
    void MoveCaretRight(bool select);
    void MoveCaretUp(bool select);
    void MoveCaretDown(bool select);
    void MoveCaretHome(bool select);
    void MoveCaretEnd(bool select);
    void MoveCaretWordLeft(bool select);
    void MoveCaretWordRight(bool select);
    void MoveCaretToDocumentBegin(bool select);
    void MoveCaretToDocumentEnd(bool select);

    void SetCaretVisible(bool visible);

    void Undo();
    void Redo();

    bool CanUndo();
    bool CanRedo();

    void Resize(uint width, uint height);

    void Redraw(const ElementId& id);
    void Redraw();
    void Remake(const ElementId& id, bool with_elements, bool undo = false);

    bool WillRedraw(const ElementId& id);

    void New();
    uint Save(const std::string& filename);
    uint Load(const std::string& filename);

    void Copy(std::stringstream& out_array, std::string& out_text);
    void Paste(std::stringstream& in_array);
    void Paste(const std::string& text);
    void Cut(std::stringstream& out_array, std::string& out_text);

    std::string ToHtml();
    std::string ToText();

    TextFormatPtr GetDefaultTextFormat();
    PageFormatPtr GetDefaultPageFormat();
    StringFormatPtr GetStringFormat(const std::string family, uint size, bool bold, bool italic, bool underline);
    StringFormatPtr GetStringFormat(const uint id);

    void UpdateFormats();

    void SetFontFamily(const std::string& family);
    void SetFontSize(const uint size);
    void SetBold(const bool enabled);
    void SetItalic(const bool enabled);
    void SetUnderline(const bool enabled);

    void SetCurrentParagraphFormat(const std::string& name);

    EditorState GetEditorState();

private:
    void MainLoop();

#ifdef DEBUG
public:
    void WaitMainLoop();
    void WaitUndo();
    void WaitRedo();
    void WaitCaretMoving();
    void WaitLoad();

private:
    uint last_task_id = 0;
    uint last_load_task_id = 0;
    bool last_task_executed = false;
    bool last_load_executed = false;
    bool last_undo_executed = false;
    bool last_redo_executed = false;

    bool last_caret_moved = false;
#endif

private:
    friend class MoveCaretTask;
    friend class SetEditorStateTask;
    friend class NewTask;
    friend class LoadTask;

    void UpdateCaretView();
    void UpdateLastSelection();

public:
    Window* window;

    StringFormatsPtr string_formats;
    ParagraphFormatsPtr paragraph_formats;

private:
    friend class Page;

    StringFormatPtr current_string_format;
    ParagraphFormatPtr current_paragraph_format;

    ElementPtr text;

public:
    Caret caret;
    Selection selection;
    Selection last_selection;

private:
    std::recursive_mutex tasks_mutex;
    std::vector<TaskPtr> tasks;
    std::stack<TaskPtr> undo_tasks;
    std::vector<TaskPtr> redo_tasks;
    std::vector<bool> undos;
    std::vector<bool> redos;

    bool exit = false;

    std::condition_variable_any next_circle;
    std::thread main_loop;

    CaretSettings caret_settings;

    uint cur_task_id = 0;

    Logger* logger;
};

}

#endif
