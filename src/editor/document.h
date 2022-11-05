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
    void InsertText(const std::string& str, const StringFormatPtr string_format, const CaretState& before_state, CaretState& after_state);

    void InsertElement(Element* element, const CaretState& caret_state, bool with_undo, bool undo = false);
    void InsertElement(Element* element, const CaretState& before_state, CaretState& after_state);
    void InsertElements(std::vector<ElementPtr>& elements, const CaretState& caret_state, bool with_undo, bool undo = false);
    void InsertElements(std::vector<ElementPtr>& elements, const CaretState& before_state, CaretState& after_state, bool with_undo, bool undo = false);

    void DeleteElements(bool left, bool with_undo, bool undo);
    void DeleteElements(const CaretState& caret_state, bool left, bool with_undo, bool undo);
    void DeleteElements(const CaretState& before_state, CaretState& after_state, bool left, bool with_undo, bool undo);

    void SplitElement(const ElementId& id, const ElementId& remake_id, const uint max_left_width);
    void MergeElement(const ElementId& id1, const ElementId& id2, ElementId remake_id);

    ElementPtr GetElement(const ElementId& _id);
    ElementPtr GetParent(const ElementId& _id);

    Rect GetCaretRect(const CaretState& caret_state);

    void MoveCaretLeft(bool selection);
    void MoveCaretRight(bool selection);
    void MoveCaretUp(bool selection);
    void MoveCaretDown(bool selection);
    void MoveCaretHome(bool selection);
    void MoveCaretEnd(bool selection);

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

private:
    uint last_task_id = 0;
    bool last_task_executed = false;
    bool last_undo_executed = false;
    bool last_redo_executed = false;
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
