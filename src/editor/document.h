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
#include "text.h"
#include "task.h"
#include "util.h"
#include "logger.h"

namespace yutovo
{

class Document
{
public:
    Document(Window* _window);
    ~Document();

    void InsertPage(bool with_undo);
    void InsertParagraph(bool with_undo);
    void InsertText(const std::string& str, bool with_undo);
    void InsertText(const std::string& str, const StringFormatPtr string_format, bool with_undo);
    void InsertText(const std::string& str, const StringFormatPtr string_format, const CaretState& before_state, CaretState& after_state, 
        ElementId element_id);

    void InsertElement(Element* element, const CaretState& caret_state, bool with_undo, bool undo = false, ElementId element_id = ElementId{});
    void InsertElement(Element* element, const CaretState& before_state, CaretState& after_state, ElementId element_id = ElementId{});
    void InsertElements(std::vector<ElementPtr>& elements, const CaretState& caret_state, bool with_undo, bool undo = false, 
        ElementId element_id = ElementId{});
    void InsertElements(std::vector<ElementPtr>& elements, const CaretState& before_state, CaretState& after_state, bool with_undo, bool undo = false, 
        ElementId element_id = ElementId{});

    void DeleteElements(bool left, bool with_undo, bool undo);
    void DeleteElements(const CaretState& caret_state, bool left, bool with_undo, bool undo);
    void DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo, bool undo);

    ElementPtr GetElement(const ElementId& _id);
    ElementPtr GetParent(const ElementId& _id);

    Rect GetCaretRect(const CaretState& caret_state);

    bool GetCurrentStringFormat(StringFormatPtr& format);

    void MoveCaret(MoveCaretTask::MoveCaretDir dir, bool selection);
    void MoveCaretLeft(bool selection);
    void MoveCaretRight(bool selection);
    void MoveCaretUp(bool selection);
    void MoveCaretDown(bool selection);
    void MoveCaretHome(bool selection);
    void MoveCaretEnd(bool selection);
    void MoveCaretWordLeft(bool selection);
    void MoveCaretWordRight(bool selection);

    void SetCaretVisible(bool visible);

    void Undo();
    void Redo();

    bool CanUndo();
    bool CanRedo();

    void Resize(uint width, uint height);

    void Redraw(const ElementId& id);
    void Redraw();
    void Remake(const ElementId& id, bool with_elements);

    std::string ToHtml();
    std::string ToText();

    TextFormatPtr GetDefaultTextFormat();
    PageFormatPtr GetDefaultPageFormat();
    ParagraphFormatPtr GetDefaultParagraphFormat();
    StringFormatPtr GetDefaultStringFormat();

private:
    void MainLoop();

#ifdef DEBUG
public:
    void WaitMainLoop();
    void WaitCaretMoving();

private:
    uint last_task_id = 0;
    bool last_task_executed = false;
    bool last_undo_executed = false;
    bool last_redo_executed = false;

    friend class MoveCaretTask;
    bool last_caret_moved = false;
#endif

public:
    Window* window;

private:
    ElementPtr text;

public:
    Caret caret;

    std::mutex tasks_mutex;
    std::vector<TaskPtr> tasks;
    std::stack<TaskPtr> undo_tasks;
    std::vector<TaskPtr> redo_tasks;
    std::vector<bool> undos;
    std::vector<bool> redos;

    bool exit = false;

    std::condition_variable next_circle;
    std::thread main_loop;

    CaretSettings caret_settings;

    uint cur_task_id = 0;

    Logger* logger;
};

}

#endif
