#include "task.h"
#include "text.h"
#include "str.h"
#include "paragraph.h"
#include "row.h"
#include "formulas/code_block.h"
#include "formulas/code_paragraph.h"
#include "formulas/code_string.h"
#include "formulas/result.h"
#include "formulas/equation.h"
#include "formulas/assignment.h"
#include "logger.h"
#include "util.h"
#include "result_codes.h"
#include <assert.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <vector>
#include <boost/locale.hpp>
#include <yutovo_service/types.h>

namespace yutovo
{

//Task

uint Task::next_id = 1;

Task::Task(ElementPtr _text) : 
    text(_text),
    document(text->document),
    window(document->window),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true)),
    id(next_id++)
{
}

Task::Task(ElementPtr _text, const uint _id) :
    text(_text),
    document(text->document),
    window(document->window),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true)),
    id(_id)
{
}

//InsertElementsTask

InsertElementsTask::InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo, bool _pasting) :
    Task(_text),
    elements(_elements),
    pasting(_pasting)
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
    //logger->Debug("Execute InsertElementsTask");

    if (with_undo)
        document->PushEditorState(true);

    if (before_state.IsEmpty())
        before_state = document->GetEditorState();
    else
        document->SetEditorState(before_state); //it is redo
    
    CaretState caret_state = before_state.caret_state;
    SelectionState& selection_state = before_state.selection_state;

    ElementPtr el;
    if (element_id.empty())
        el = document->GetParent(caret_state.id);
    else
    {
        el = document->GetParent(element_id);
        caret_state.id = element_id;
    }
    assert(el != nullptr);

    CaretState c;
    if (with_undo && !selection_state.IsEmpty())
    {
        //remove selection before insert
        auto DeleteElements = [&](ElementPtr _el)
        {
            assert(_el != nullptr);
            if (_el->DeleteElements(true, with_undo))
            {
                if (_el->parent)
                    document->Remake(_el->parent->id, true, with_undo, false);
                el = document->GetElement(document->caret->GetElement()->id);
                return true;
            }
            return false;
        };

        for (int i = selection_state.state.size() - 1; i >= 0; --i)
        {
            ElementSelectionState& s = selection_state.state[i];
            if (!DeleteElements(document->GetElement(s.id)))
            {
                if (with_undo)
                    document->RollbackUndo();
                return false;
            }
        }
    }

    std::vector<ElementPtr> _elements;
    for (auto t : elements)
    {
        auto code = document->FindParent(el->id, ElementType::CODE_BLOCK);
        if (t->type == ElementType::STRING && code)
        {
            //change type of string
            String* str = (String*)t.get();
            auto c = new CodeString(*str);
            c->format = el->GetStringFormat();
            _elements.emplace_back(c);
            continue;
        }
        if (t->type == ElementType::PARAGRAPH && code)
        {
            //change type of paragraph
            _elements.emplace_back(new CodeParagraph(document));
            continue;
        }
        if (t->type == ElementType::STRING)
        {
            //divide string by paragraphs
            std::u32string u_str = t->ToText();
            if (u_str.size() > 0)
            {
                size_t p1 = 0, p2 = 0, k = 0;
                while (p2 < u_str.size())
                {
                    p2 = u_str.find(U"\r\n", p1);
                    if (p2 == std::string::npos)
                    {
                        p2 = u_str.find(U"\n", p1);
                        if (p2 == std::string::npos)
                            p2 = u_str.size();
                        else
                            k = 1;
                    }
                    else
                        k = 2;
                    std::u32string u_part = u_str.substr(p1, p2 - p1);
                    std::string s = boost::locale::conv::utf_to_utf<char>(u_part);
                    _elements.emplace_back(new String(document, s, t->GetStringFormat()));
                    if (p2 + k < u_str.size())
                        _elements.emplace_back(new Paragraph(document));
                    p2 += k;
                    p1 = p2;
                }
                continue;
            }
        }
        _elements.push_back(t);
    }

    document->pasting = pasting;
    for (auto& _el : _elements)
    {
        _el->parent = nullptr;
        std::vector<ElementPtr> t{_el};
        if (!el->InsertElements(t, with_undo))
        {
            if (with_undo)
                document->RollbackUndo();
            document->pasting = false;
            return false;
        }
        
        if (document->caret->GetElement())
            el = document->GetElement(document->caret->GetElement()->id);

        if (with_undo)
            document->PushEditorState(true);
    }
    if (el->parent)
        document->Remake(el->parent->id, true, with_undo, false, true); //move into view
    document->pasting = false;
    return true;
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

DeleteElementsTask::DeleteElementsTask(ElementPtr _text, ElementId _element_id, bool _with_undo) :
    Task(_text),
    element_id(_element_id)
{
}

DeleteElementsTask::DeleteElementsTask(ElementPtr _text, ElementId _element_id, bool _with_undo, uint _id) :
    Task(_text, _id),
    element_id(_element_id)
{
    with_undo = _with_undo;
}

bool DeleteElementsTask::Execute()
{
    //logger->Debug("Execute DeleteElementsTask");

    if (with_undo)
        document->PushEditorState(true);

    SelectionState selection_state;
    if (element_id.empty())
    {
        if (before_state.IsEmpty())
            before_state = document->GetEditorState();
        else
            document->SetEditorState(before_state); //it is redo
        selection_state = before_state.selection_state;
    }
    else
    {
        before_state = document->GetEditorState();
        //clear elements inside this element
        selection_state = SelectionState{element_id, 0, document->GetElement(element_id)->elements->Count()};
        EditorState s{before_state.caret_state, selection_state};
        document->SetEditorState(s);
    }

    CaretState caret_state = before_state.caret_state;

    auto DeleteElements = [&](ElementPtr el)
    {
        assert(el != nullptr);
        return el->DeleteElements(left, with_undo);
    };

    if (selection_state.IsEmpty())
    {
        if (DeleteElements(document->GetParent(caret_state.id)))
        {
            if (with_undo)
                document->PushEditorState(true);
            document->Remake(document->caret->GetElement()->id, true, with_undo, false, true); //move into view
            return true;
        }
    }
    else
    {
        for (int i = selection_state.state.size() - 1; i >= 0; --i)
        {
            ElementSelectionState& s = selection_state.state[i];
            if (!DeleteElements(document->GetElement(s.id)))
            {
                if (with_undo)
                    document->RollbackUndo();
                return false;
            }
        }
        if (with_undo)
            document->PushEditorState(true);
        if (document->caret->GetElement())
            document->Remake(document->caret->GetElement()->id, true, with_undo, false, true); //move into view
        return true;
    }

    if (with_undo)
        document->RollbackUndo();
    return false;
}

//InsertFormulasTask

InsertFormulasTask::InsertFormulasTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo, bool _pasting) :
    Task(_text),
    elements(_elements),
    pasting(_pasting)
{
    with_undo = _with_undo;
}

InsertFormulasTask::InsertFormulasTask(ElementPtr _text, uint _id, std::vector<ElementPtr>& _elements, bool _with_undo) :
    Task(_text, _id),
    elements(_elements)
{
    with_undo = _with_undo;
}

bool InsertFormulasTask::Execute()
{
    //logger->Debug("Execute InsertFormulasTask");

    if (before_state.IsEmpty())
        before_state = document->GetEditorState();
    else
        text->document->SetEditorState(before_state); //it is redo
    
    CaretState& caret_state = before_state.caret_state;
    SelectionState& selection_state = before_state.selection_state;

    ElementPtr el = document->GetParent(caret_state.id);
    assert(el);

    bool insert_code_block = false;
    if (elements[0]->type != ElementType::CODE_BLOCK)
    {
        if (document->FindParent(caret_state.id, ElementType::CODE_BLOCK) == nullptr)
        {
            //there is no code element - insert one in the current row
            auto row = document->FindParentRow(caret_state.id);
            if (!row)
            {
                if (with_undo)
                    document->RollbackUndo();
                return false;
            }
            
            if (with_undo)
                document->PushEditorState(true);
            
            ElementPtr code(new CodeBlock(row.get(), document->cur_code_id));
            std::vector v{code};
            if (!row->InsertElements(v, with_undo))
            {
                if (with_undo)
                    document->RollbackUndo();
                return false;
            }
            insert_code_block = true;
            el = code->elements->Get(0)->elements->Get(0);
        }
    }

    if (with_undo)
        document->PushEditorState(true);

    std::vector<ElementPtr> _elements;
    for (int i = 0; i < elements.size(); ++i)
    {
        ElementPtr c(elements[i]->Clone());
        c->UpdateLevel(el->level);
        _elements.push_back(c);
    }
    
    document->pasting = pasting;
    if (el->InsertElements(_elements, insert_code_block ? false : with_undo))
    {
        if (with_undo)
            document->PushEditorState(true);
        document->Remake(el->parent->parent->id, true, with_undo, false, true); //move into view
        document->pasting = false;
        return true;
    }
    document->pasting = false;

    if (with_undo)
        document->RollbackUndo();
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
    //logger->Debug("Execute ChangeStringFormatTask");
    if (with_undo)
        text->document->PushEditorState(true);

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
        if (text->document->IsString(el))
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
        {
            if (with_undo)
                document->RollbackUndo();
            return false;
        }

        document->UpdateFormats();
    }

    if (with_undo)
        document->PushEditorState(true);

    if (!selection_state.IsEmpty())
    {
        auto p_id = selection_state.state.size() == 1 ? GetParent(selection_state.state[0].id) : selection_state.GetCommonElement();
        document->Remake(p_id, true, with_undo, false);
    }
    document->Redraw(caret_state.id, true); //move into view

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
    //logger->Debug("Execute ChangeParagraphFormatTask");
    if (before_state.IsEmpty())
        before_state = document->GetEditorState();

    CaretState& caret_state = before_state.caret_state;
    auto el = document->FindParentParagraph(caret_state.id);
    if (!el)
        return false;

    if (with_undo)
        document->PushEditorState(true);

    if (!el->ChangeParagraphFormat(format, with_undo))
    {
        if (with_undo)
            document->RollbackUndo();
        return false;
    }

    if (with_undo)
        document->PushEditorState(true);

    document->UpdateFormats();
    document->Redraw(el->parent->id, true); //move into view

    return true;
}

//RemakeTask

RemakeTask::RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements, bool _with_undo) : 
    Task(_text), 
    element_id(_element_id),
    with_elements(_with_elements),
    with_undo(_with_undo)
{
}

RemakeTask::RemakeTask(ElementPtr _text, const ElementId& _element_id, bool _with_elements, bool _with_undo, bool _move_into_view, uint id, uint _priority) :
    Task(_text, id), 
    element_id(_element_id),
    with_elements(_with_elements),
    with_undo(_with_undo),
    move_into_view(_move_into_view)
{
    priority = _priority;
}

bool RemakeTask::Execute()
{
    //logger->Debug("Execute RemakeTask element_id={}", IdToString(element_id));
    auto el = document->GetElement(element_id);
    if (!el)
        return false;
    el->Remake(with_elements, true, with_undo);
    document->Redraw(element_id, move_into_view);
    return true;
}

//RedrawTask

RedrawTask::RedrawTask(ElementPtr _text, const ElementId& _id, bool _move_into_view) :
    Task(_text),
    element_id(_id),
    move_into_view(_move_into_view)
{
}

bool RedrawTask::Execute()
{
    ElementPtr element = document->GetElement(element_id);
    if (!element || document->WillRedraw(element_id, move_into_view)) //don't redraw if it will be redrawn later
        return false;
    
    //logger->Debug("Execute RedrawTask element_id={}", IdToString(element_id));

    document->caret->Hide();

    Rect clear_rect = element->draw_rect.IsEmpty() ? element->GetAbsoluteRect() : element->draw_rect;
    window->ClearRect(clear_rect); //clear last rect before drawing
    element->Draw(); //draw element and update its rect
    element->UpdateDrawRect();

    document->caret->Show();
    window->SetDocumentSize({text->rect.width, text->rect.height});
    window->Update(element->GetAbsoluteRect());
    if (move_into_view)
        document->UpdateCaretView();
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
    if (document->WillResize()) //don't resize if it will be resized later
        return false;
    //logger->Debug("Execute ResizeTask width={}, height={}", width, height);
    window->Resize(width, height);
    return true;
}

//CallFuncTask

CallFuncTask::CallFuncTask(ElementPtr _text, const ElementId& _id, CallFuncPtr _func, const uint task_id) :
    Task(_text, task_id),
    id(_id),
    func(_func)
{
}

bool CallFuncTask::Execute()
{
    func(id);
    return true;
}

//MoveCaretTask

MoveCaretTask::MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible) : 
    Task(_text),
    document(_text->document),
    caret(_caret),
    dir(_dir),
    visible(_visible)
{
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible, bool _select) :
    MoveCaretTask(_text, _caret, _dir, _visible)
{
    select = _select;
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible, bool _select, uint _task_id) : 
    Task(_text, _task_id),
    document(_text->document),
    caret(_caret),
    dir(_dir),
    visible(_visible)
{
    select = _select;
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, CaretPtr _caret, Point _point) :
    Task(_text),
    document(_text->document),
    caret(_caret),
    dir(MoveCaretDir::POINT),
    point(_point),
    select(false)
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
    case MoveCaretDir::POINT:
        {
            ElementId id;
            if (text->GetElementAtCoords(point.x, point.y, id))
                caret->SetState(id, true);
        }
        break;
    case MoveCaretDir::LEFT:
        if (!document->selection.IsEmpty() && !select)
        {
            ElementSelection& s = document->selection.selection[0];
            if (document->IsParagraph(s.element) || s.element->type == ElementType::CODE_ROW || s.element->type == ElementType::TEXT)
            {
                auto el = s.element->elements->Get(0);
                if (el)
                {
                    CaretState c;
                    if (el->GetFirstCaretState(c, nullptr))
                    {
                        caret->SetState(c);
                        break;
                    }
                }
            }
            caret->SetState(s.element->id, s.start);
            break;
        }
        caret->MoveLeft(selection);
        break;
    case MoveCaretDir::RIGHT:
        if (!document->selection.IsEmpty() && !select)
        {
            ElementSelection& s = document->selection.selection[document->selection.selection.size() - 1];
            if (document->IsParagraph(s.element) || s.element->type == ElementType::CODE_ROW || s.element->type == ElementType::TEXT)
            {
                auto el = s.element->elements->Get(s.start + s.size - 1);
                if (el)
                {
                    CaretState c;
                    if (el->GetLastCaretState(c, nullptr))
                    {
                        caret->SetState(c);
                        break;
                    }
                }
            }
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
    case MoveCaretDir::PAGE_UP:
        caret->MovePageUp(selection);
        break;
    case MoveCaretDir::PAGE_DOWN:
        caret->MovePageDown(selection);
        break;
    case MoveCaretDir::DOCUMENT_BEGIN:
        caret->MoveToDocumentBegin(selection);
        break;
    case MoveCaretDir::DOCUMENT_END:
        caret->MoveToDocumentEnd(selection);
        break;
    case MoveCaretDir::SELECT_ALL:
        caret->MoveToDocumentEnd(nullptr);
        document->selection.Clear();
        document->selection.Add(text, 0, text->elements->Count());
        break;
    }

    if (move_into_view)
        document->UpdateCaretView();
    document->UpdateLastSelection();
    document->caret->Show();

    if (!select)
    {
        document->selection.Clear();
        document->UpdateLastSelection();
        text->window->OnCaretMoved(document->GetEditorState());
    }

#ifdef DEBUG
    document->last_caret_moved = true;
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
    document->caret->SetState(caret_state);
    document->selection.Set(selection_state);

    document->UpdateCaretView();
    document->UpdateLastSelection();    

#ifdef DEBUG
    document->last_caret_moved = true;
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
    document->caret->MoveToDocumentBegin(nullptr);
    document->ResetTasks();
    document->text.reset(new Text(text->document));
    document->Remake(text->id, true, false, false);
    document->MoveCaretToDocumentBegin(false);
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
    try
    {
        boost::archive::binary_oarchive oarchive(file);
        RegisterTypes(oarchive);

        oarchive << document->string_formats; //store string formats
        oarchive << document->paragraph_formats; //store paragraph formats
        oarchive << text; //store text
    }
    catch (boost::archive::archive_exception& ex)
    {
        window->OnSaveResult(id, ToIOResult(ex.code));
        logger->Error("Error saving document '{}': {}", filename, ex.code);
        return false;
    }
    catch (const std::ifstream::failure& ex)
    {
        window->OnSaveResult(id, IOResult::InputStreamError);
        logger->Error("Error saving file '{}': {}", filename, ex.what());
        return false;
    }
    catch (const std::exception& ex)
    {
        window->OnSaveResult(id, IOResult::InputStreamError);
        logger->Error("Error saving file '{}': {}", filename, ex.what());
        return false;
    }
    catch (...)
    {
        window->OnSaveResult(id, IOResult::InputStreamError);
        logger->Error("Error saving file '{}'", filename);
        return false;
    }

    window->OnSaveResult(id, IOResult::Success);
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
    ElementPtr t;
    std::string str;

    if (filename.substr(filename.find_last_of(".") + 1) == "yut")
    {
        DocumentUserData user_data{document};

        try
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                window->OnLoadResult(id, IOResult::InputStreamError);
                logger->Error("Error loading file '{}': File not open", filename);
                return false;
            }
            UserDataAdapter<DocumentUserData, boost::archive::binary_iarchive> iarchive(user_data, file);
            RegisterTypes(iarchive);

            iarchive >> document->string_formats; //restore string formats
            iarchive >> document->paragraph_formats; //restore paragraph formats
            iarchive >> t; //restore text
        }
        catch (boost::archive::archive_exception& ex)
        {
            window->OnLoadResult(id, ToIOResult(ex.code));
            logger->Error("Error loading file '{}': {}, {}", filename, ex.code, ex.what());
            return false;
        }
        catch (const std::ifstream::failure& ex)
        {
            window->OnLoadResult(id, IOResult::InputStreamError);
            logger->Error("Error loading file '{}': {}", filename, ex.what());
            return false;
        }
        catch (const std::exception& ex)
        {
            window->OnLoadResult(id, IOResult::InputStreamError);
            logger->Error("Error loading file '{}': {}", filename, ex.what());
            return false;
        }
        catch (...)
        {
            window->OnLoadResult(id, IOResult::InputStreamError);
            logger->Error("Error loading file '{}'", filename);
            return false;
        }
    }
    else //".txt" and others load as text
    {
        try
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                window->OnLoadResult(id, IOResult::InputStreamError);
                logger->Error("Error loading file '{}': File not open", filename);
                return false;
            }
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            str = std::string(size, ' ');
            file.seekg(0);
            file.read(&str[0], size);
        }
        catch (const std::ifstream::failure& ex)
        {
            window->OnLoadResult(id, IOResult::InputStreamError);
            logger->Error("Error loading file '{}': {}", filename, ex.what());
            return false;
        }

        t.reset(new Text(document));
    }

    if (!t)
        return false;
    
    document->ResetTasks();
    document->text = t;
    document->MoveCaretToDocumentBegin(false);
    if (!str.empty())
        document->InsertString(str, false);
    document->Remake(text->id, true, false, false);
    document->text->ReSolve();
    document->MoveCaretToDocumentBegin(false);
    window->OnLoadResult(id, IOResult::Success);
    return true;
}

//CopyTask

CopyTask::CopyTask(ElementPtr _text, std::stringstream& _out_array, std::u32string& _out_text, bool _cut) :
    Task(_text),
    out_array(_out_array),
    out_text(_out_text),
    cut(_cut)
{
}

bool CopyTask::Execute()
{
    auto before_state = document->GetEditorState();
    SelectionState& selection_state = before_state.selection_state;
    if (selection_state.IsEmpty())
    {
        window->OnCopyResult(CopyResult::EmptySelection);
        return false;
    }

    std::vector<ElementPtr> copy;
    for (int i = 0; i < selection_state.state.size(); ++i)
        document->GetElement(selection_state.state[i].id)->Copy(copy);

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
        window->OnCopyResult(CopyResult::CopyError);
        return false;
    }

    window->OnCopyResult(CopyResult::Success);

    if (cut)
        document->DeleteElements(true, true, false);
    return true;
}

//ResultTask

ResultTask::ResultTask(ElementPtr _text, ElementId _id, Result _result) :
    Task(_text),
    id(_id),
    result(_result)
{
}

bool ResultTask::Execute()
{
    ElementPtr el = document->GetElement(id);
    if (!el)
        return false;
    switch (el->type)
    {
    case ElementType::AUTO_RESULT:
    {
        AutoResult* r = dynamic_cast<AutoResult*>(el.get());
        if (!r)
            return false;
        r->PutResult(result);
        break;
    }
    case ElementType::ASSIGNMENT:
    {
        Assignment* r = dynamic_cast<Assignment*>(el.get());
        if (!r)
            return false;
        r->PutResult(result);
        break;
    }
    default:
        return false;
    }
    return true;
}

//ResolveTask

ResolveTask::ResolveTask(ElementPtr _text, ElementId _id) :
    Task(_text),
    id(_id)
{
}

bool ResolveTask::Execute()
{
    auto el = document->FindParent(id, ElementType::CODE_BLOCK);
    if (!el)
        return false;
    
    CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
    uint code_id = c->code_id;
    std::vector<ElementId> code_blocks;
    text->GetElements(ElementType::CODE_BLOCK, code_blocks); //find all code blocks
    for (ElementId _id : code_blocks)
    {
        auto el = document->GetElement(_id);
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        if (c && c->code_id == code_id)
            c->ReSolve(); //resolve all the connected code blocks above and the current one
    }
    return true;
}

//ResolveDependeciesTask

ResolveDependeciesTask::ResolveDependeciesTask(ElementPtr _text, ElementId _after_id, const std::string& _identifier) :
    Task(_text),
    after_id(_after_id),
    identifier(_identifier)
{
}

bool ResolveDependeciesTask::Execute()
{
    auto el = document->FindParent(after_id, ElementType::CODE_BLOCK);
    if (!el)
        return false;
    
    CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
    uint code_id = c->code_id;

    std::vector<std::string> id_arr;
    boost::split(id_arr, identifier, boost::is_any_of("()"));

    std::vector<ElementId> equations;
    c->GetElementsBelow(after_id, ElementType::EQUATION, equations); //get equations below in the current code block
    for (ElementId _id : equations)
    {
        auto _el = document->GetElement(_id);
        Equation* eq = dynamic_cast<Equation*>(_el.get());
        for (auto& s : id_arr)
        {
            if (eq->Depends(s))
                eq->ReSolve();
        }
    }

    std::vector<ElementId> code_blocks;
    text->GetElementsBelow(c->id, ElementType::CODE_BLOCK, code_blocks); //find all code blocks below
    for (ElementId _id : code_blocks)
    {
        auto el = document->GetElement(_id);
        CodeBlock* _c = dynamic_cast<CodeBlock*>(el.get());
        if (c && _c->code_id == code_id)
        {
            equations.clear();
            c->GetElements(ElementType::EQUATION, equations);
            for (ElementId _id : equations)
            {
                auto _el = document->GetElement(_id);
                Equation* eq = dynamic_cast<Equation*>(_el.get());
                for (auto& s : id_arr)
                {
                    if (eq->Depends(s))
                        eq->ReSolve();
                }
            }
        }
    }
    return true;
}

//ResolveErrorsTask

ResolveErrorsTask::ResolveErrorsTask(ElementPtr _text) :
    Task(_text)
{
}

bool ResolveErrorsTask::Execute()
{
    std::vector<ElementId> code_blocks;
    text->GetElements(ElementType::CODE_BLOCK, code_blocks); //find all code blocks
    for (ElementId _id : code_blocks)
    {
        auto el = document->GetElement(_id);
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        if (c)
            c->ReSolve(true); //resolve all the expressions with errors
    }
    return true;
}

}
