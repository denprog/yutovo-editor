#include "task.h"
#include "text.h"
#include "str.h"
#include "paragraph.h"
#include "page.h"
#include "row.h"
#include "util.h"
#include "result_codes.h"
#include <assert.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <sstream>

namespace yutovo
{

//Task

uint Task::next_id = 1;

Task::Task(ElementPtr _text) : 
    text(_text),
    logger(Logger::GetInstance()),
    id(next_id++)
{
}

Task::Task(ElementPtr _text, const uint _id) :
    text(_text),
    logger(Logger::GetInstance()),
    id(_id)
{
}

//InsertElementsTask

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo) :
    Task(_text),
    elements(_elements)
{
    with_undo = _with_undo;
}

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, uint _id) :
    Task(_text, _id),
    elements(_elements)
{
    with_undo = false;
}

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, uint _id, ElementId _element_id) :
    InsertElementsTask(_text, _elements, _id)
{
    element_id = _element_id;
}

bool InsertElementsTask::Execute()
{
    logger->Debug("Execute InsertElementsTask");

    if (with_undo)
        text->document->PushEditorState(true);

    if (before_state.IsEmpty())
        before_state = text->document->GetEditorState();
    
    CaretState caret_state = before_state.caret_state;
    SelectionState& selection_state = before_state.selection_state;

    ElementPtr el;
    if (element_id.empty())
        el = text->document->GetParent(caret_state.id);
    else
    {
        el = text->document->GetParent(element_id);
        caret_state.id = element_id;
    }
    assert(el != nullptr);

    CaretState c;
    if (with_undo && !selection_state.IsEmpty())
    {
        //remove selection before insert
        auto DeleteElements = [&](ElementPtr el)
        {
            assert(el != nullptr);
            if (el->DeleteElements(true, with_undo))
            {
                text->document->Remake(el->parent->id, true);
                return true;
            }
            return false;
        };

        for (int i = selection_state.state.size() - 1; i >= 0; --i)
        {
            ElementSelectionState& s = selection_state.state[i];
            if (!DeleteElements(text->document->GetElement(s.id)))
                return false; //todo transaction fix?
        }
    }

    if (el->InsertElements(elements, with_undo))
    {
        if (with_undo)
            text->document->PushEditorState(true);
        text->document->Remake(el->parent->id, true);
        return true;
    }
    return false;
}

//DeleteElementsTask

DeleteElementsTask::DeleteElementsTask(ElementPtr _text, bool _left, bool _with_undo) :
    Task(_text),
    left(_left)
{
    with_undo = _with_undo;
}

DeleteElementsTask::DeleteElementsTask(ElementPtr _text, bool _left, uint _id) :
    Task(_text, _id),
    left(_left)
{
    with_undo = false;
}

bool DeleteElementsTask::Execute()
{
    logger->Debug("Execute DeleteElementsTask");

    if (with_undo)
        text->document->PushEditorState(true);

    if (before_state.IsEmpty())
        before_state = text->document->GetEditorState();

    CaretState caret_state = before_state.caret_state;
    SelectionState& selection_state = before_state.selection_state;

    auto DeleteElements = [&](ElementPtr el)
    {
        assert(el != nullptr);
        if (el->DeleteElements(left, with_undo))
            return true;
        return false;
    };

    if (selection_state.IsEmpty())
    {
        if (DeleteElements(text->document->GetParent(caret_state.id)))
        {
            if (with_undo)
                text->document->PushEditorState(true);
            return true;
        }
    }
    else
    {
        for (int i = selection_state.state.size() - 1; i >= 0; --i)
        {
            ElementSelectionState& s = selection_state.state[i];
            if (!DeleteElements(text->document->GetElement(s.id)))
                return false; //todo transaction fix?
        }
        if (with_undo)
            text->document->PushEditorState(true);
        return true;
    }
    return false;
}

//ChangeStringFormatTask

ChangeStringFormatTask::ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, bool _with_undo) :
    Task(_text),
    format(_format)
{
    with_undo = _with_undo;
}

ChangeStringFormatTask::ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, bool _set_family, bool _set_size, 
    bool _set_bold, bool _set_italic, bool _set_underline, bool _with_undo) :
    ChangeStringFormatTask(_text, _format, _with_undo)
{
    set_family = _set_family;
    set_size = _set_size;
    set_bold = _set_bold;
    set_italic = _set_italic;
    set_underline = _set_underline;
}

ChangeStringFormatTask::ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, uint _id) :
    Task(_text, _id), 
    format(_format)
{
}

bool ChangeStringFormatTask::Execute()
{
    logger->Debug("Execute ChangeStringFormatTask");
    if (with_undo)
    {
        text->document->PushEditorState(true);
        text->document->Remake(text->id, true, true);
    }

    if (before_state.IsEmpty())
        before_state = text->document->GetEditorState();

    CaretState& caret_state = before_state.caret_state;
    SelectionState& selection_state = before_state.selection_state;

    std::vector<ElementPtr> elements;
    for (int i = 0; i < selection_state.state.size(); ++i)
    {
        ElementSelectionState& s = selection_state.state[i];
        elements.push_back(text->document->GetElement(s.id));
    }

    for (int i = 0; i < selection_state.state.size(); ++i)
    {
        ElementSelectionState& s = selection_state.state[i];
        auto el = elements[i];

        StringFormatPtr _format;
        if (text->document->GetElementType(el->id) == ElementType::STRING)
        {
            //set only actual params
            StringFormat f = *((String*)el.get())->format;
            if (set_family)
                f.family = format->family;
            if (set_size)
                f.size = format->size;
            if (set_bold)
                f.bold = format->bold;
            if (set_italic)
                f.italic = format->italic;
            if (set_underline)
                f.underline = format->underline;
            _format = text->document->GetStringFormat(f.family, f.size, f.bold, f.italic, f.underline);
        }
        else
        {
            _format = format;
        }

        if (!el->ChangeStringFormat(_format, with_undo))
            return false;
        text->document->Remake(text->document->GetParent(s.id)->id, true);
        text->document->UpdateFormats();
    }

    if (with_undo)
        text->document->PushEditorState(true);

    return true;
}

ChangeParagraphFormatTask::ChangeParagraphFormatTask(ElementPtr _text, const ParagraphFormatPtr& _format, bool _with_undo) :
    Task(_text),
    format(_format)
{
    with_undo = _with_undo;
}

ChangeParagraphFormatTask::ChangeParagraphFormatTask(ElementPtr _text, const ParagraphFormatPtr& _format, uint _id) :
    Task(_text, _id), 
    format(_format)
{
}

bool ChangeParagraphFormatTask::Execute()
{
    logger->Debug("Execute ChangeParagraphFormatTask");
    if (before_state.IsEmpty())
        before_state = text->document->GetEditorState();

    CaretState& caret_state = before_state.caret_state;
    auto el = text->document->FindParent(caret_state.id, ElementType::PARAGRAPH);
    if (!el)
        return false;

    if (with_undo)
        text->document->PushEditorState(true);

    if (!el->ChangeParagraphFormat(format, with_undo))
        return false;

    if (with_undo)
        text->document->PushEditorState(true);

    text->document->UpdateFormats();

    return true;
}

//RemakeTask

RemakeTask::RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements) : 
    Task(_text), 
    element_id(_element_id),
    with_elements(_with_elements)
{
}

RemakeTask::RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements, uint id) :
    Task(_text, id), 
    element_id(_element_id),
    with_elements(_with_elements)
{
}

bool RemakeTask::Execute()
{
    logger->Debug("Execute RemakeTask element_id={}", IdToString(element_id));
    auto p = text->document->GetElement(element_id);
    if (!p)
        return false;
    text->document->GetElement(element_id)->Remake(with_elements);
    text->document->Redraw(element_id);
    return true;
}

//RedrawTask

RedrawTask::RedrawTask(ElementPtr _text, const ElementId& _id) :
    Task(_text),
    element_id(_id)
{
}

bool RedrawTask::Execute()
{
    logger->Debug("Execute RedrawTask element_id={}", IdToString(element_id));
    ElementPtr element = text->document->GetElement(element_id);
    if (!element)
        return false;
    text->window->DrawFillRect(element->GetAbsoluteRect(), Color::White());
    element->Draw();
    text->window->Update(element->GetAbsoluteRect());
    return true;
}

//ResizeTask

ResizeTask::ResizeTask(ElementPtr _text, const uint _width, const uint _height) : 
    Task(_text),
    width(_width),
    height(_height)
{
}

bool ResizeTask::Execute()
{
    logger->Debug("Execute ResizeTask width={}, height={}", width, height);
    text->window->Resize(width, height);
    return true;
}

//MoveCaretTask

MoveCaretTask::MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible) : 
    Task(_text),
    document(_text->document),
    caret(_caret),
    dir(_dir),
    visible(_visible)
{
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, Caret* _caret, MoveCaretDir _dir, bool _visible, bool _select) :
    MoveCaretTask(_text, _caret, _dir, _visible)
{
    select = _select;
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, Caret* _caret, Point _point) :
    Task(_text),
    document(_text->document),
    caret(_caret),
    point(_point)
{
}

bool MoveCaretTask::Execute()
{
    Selection* selection = select ? &document->selection : nullptr;
    switch (dir)
    {
    case MoveCaretDir::NONE:
        caret->SetVisible(visible);
        return true;
    case MoveCaretDir::LEFT:
        if (!document->selection.IsEmpty() && !select)
        {
            ElementSelection& s = document->selection.selection[0];
            caret->SetState(s.element->id, s.start);
            break;
        }
        caret->MoveLeft(selection);
        break;
    case MoveCaretDir::RIGHT:
        if (!document->selection.IsEmpty() && !select)
        {
            ElementSelection& s = document->selection.selection[document->selection.selection.size() - 1];
            caret->SetState(s.element->id, s.start + s.size);
            break;
        }
        caret->MoveRight(selection);
        break;
    case MoveCaretDir::UP:
        caret->MoveUp(selection);
        break;
    case MoveCaretDir::DOWN:
        caret->MoveDown(selection);
        break;
    case MoveCaretDir::HOME:
        caret->MoveHome(selection);
        break;
    case MoveCaretDir::END:
        caret->MoveEnd(selection);
        break;
    case MoveCaretDir::WORD_LEFT:
        caret->MoveWordLeft(selection);
        break;
    case MoveCaretDir::WORD_RIGHT:
        caret->MoveWordRight(selection);
        break;
    case MoveCaretDir::DOCUMENT_BEGIN:
        caret->MoveToDocumentBegin(selection);
        break;
    case MoveCaretDir::DOCUMENT_END:
        caret->MoveToDocumentEnd(selection);
        break;
    }

    text->document->UpdateCaretView();
    text->document->UpdateLastSelection();

    if (!select)
    {
        text->document->selection.Clear();
        text->document->UpdateLastSelection();
        text->window->OnCaretMoved(text->document->GetEditorState());
    }

#ifdef DEBUG
    text->document->last_caret_moved = true;
#endif

    return true;
}

//SetEditorStateTask

SetEditorStateTask::SetEditorStateTask(ElementPtr _text, const CaretState& _caret_state, const SelectionState& _selection_state, const uint task_id) :
    Task(_text, task_id),
    caret_state(_caret_state),
    selection_state(_selection_state)
{
}

bool SetEditorStateTask::Execute()
{
    text->document->caret.SetState(caret_state);
    text->document->selection.Set(selection_state);

    text->document->UpdateCaretView();
    text->document->UpdateLastSelection();    

#ifdef DEBUG
    text->document->last_caret_moved = true;
#endif
    return true;
}

//NewTask

NewTask::NewTask(ElementPtr _text) :
    Task(_text)
{
}

bool NewTask::Execute()
{
    text->document->MoveCaretToDocumentBegin(false);
    text->document->text = ElementPtr(new Text(text->document));
    text->document->Remake(text->id, true, false);
    return true;
}

//SaveTask

SaveTask::SaveTask(ElementPtr _text, const std::string _filename) :
    Task(_text),
    filename(_filename)
{
}

bool SaveTask::Execute()
{
    std::ofstream file(filename);
    boost::archive::binary_oarchive oarchive(file);
    RegisterTypes(oarchive);

    try
    {
        oarchive << text->document->string_formats; //store string formats
        oarchive << text->document->paragraph_formats; //store paragraph formats
        oarchive << text; //store text
    }
    catch (boost::archive::archive_exception& ex)
    {
        text->window->OnSaveResult(id, ToIOResult(ex.code));
    }

    text->window->OnSaveResult(id, IOResult::Success);
    return true;
}

//LoadTask

LoadTask::LoadTask(ElementPtr _text, const std::string _filename) :
    Task(_text),
    filename(_filename)
{
}

bool LoadTask::Execute()
{
    std::ifstream file(filename);
    DocumentUserData user_data{text->document};
    UserDataAdapter<DocumentUserData, boost::archive::binary_iarchive> iarchive(user_data, file);
    RegisterTypes(iarchive);
    ElementPtr t;

    try
    {
        iarchive >> text->document->string_formats; //restore string formats
        iarchive >> text->document->paragraph_formats; //restore paragraph formats
        iarchive >> t; //restore text
    }
    catch (boost::archive::archive_exception& ex)
    {
        text->window->OnLoadResult(id, ToIOResult(ex.code));
    }

    text->document->text = t;
    text->document->MoveCaretToDocumentBegin(false);
    text->document->Remake(text->id, true, false);
    text->window->OnLoadResult(id, IOResult::Success);
    return true;
}

//CopyTask

CopyTask::CopyTask(ElementPtr _text, std::stringstream& _out_array, std::string& _out_text, bool _cut) :
    Task(_text),
    out_array(_out_array),
    out_text(_out_text),
    cut(_cut)
{
}

bool CopyTask::Execute()
{
    auto before_state = text->document->GetEditorState();
    SelectionState& selection_state = before_state.selection_state;
    if (selection_state.IsEmpty())
    {
        text->document->window->OnCopyResult(CopyResult::EmptySelection);
        return false;
    }

    std::vector<ElementPtr> copy;
    for (int i = 0; i < selection_state.state.size(); ++i)
        text->document->GetElement(selection_state.state[i].id)->Copy(copy);

    for (auto& el : copy)
    {
        out_text += el->ToText();
        el->parent = nullptr; //these elements have no parent
    }

    boost::archive::binary_oarchive oarchive(out_array);
    RegisterTypes(oarchive);

    try
    {
        oarchive << copy;
    }
    catch (boost::archive::archive_exception& ex)
    {
        text->document->window->OnCopyResult(CopyResult::CopyError);
        return false;
    }

    text->document->window->OnCopyResult(CopyResult::Success);

    if (cut)
        text->document->DeleteElements(true, true, false);
    return true;
}

}
