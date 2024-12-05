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
#include <yutovo_logger/logger.h>
#include "util.h"
#include "result_codes.h"
#include <assert.h>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <vector>
#include <boost/locale.hpp>
#include <yutovo_solver/types.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>

namespace yutovo
{

//Task

uint Task::next_id = 1;

Task::Task(ElementPtr _text) : 
    text(_text),
    document(text->document),
    window(document->window),
    logger(Logger::GetInstance(document->config.logs_path, "yutovo_editor", true, true)),
    id(next_id++)
{
}

Task::Task(ElementPtr _text, const uint _id) :
    text(_text),
    document(text->document),
    window(document->window),
    logger(Logger::GetInstance(document->config.logs_path, "yutovo_editor", true, true)),
    id(_id)
{
}

void Task::Remake(ElementId _id, bool move_into_view)
{
    for (auto ch : document->changed_elements)
    {
        if (IsChild(ch, _id))
            _id = ch;
    }

    auto el = document->GetElement(_id);
    while (!el && !_id.empty())
    {
        el = document->GetElement(GetParent(_id));
    }

    if (!el)
        return;
    Element* _el = el.get();
    _el->Normalize();
    if (_el->Remake(true))
    {
        _el = _el->parent;
        while (_el->Remake())
        {
            _el = _el->parent;
        }
    }

    if (document->IsVisible(_el->id))
        document->Redraw(_el->id, move_into_view);

    for (auto r : document->resolve_elements)
    {
        auto el = document->GetElement(r);
        if (!el)
            continue;
        el->ReSolve();

        Element* _el = el.get();
        if (_el->Remake(true))
        {
            _el = _el->parent;
            while (_el->Remake())
            {
                _el = _el->parent;
            }
        }

        if (document->IsVisible(_el->id))
            document->Redraw(_el->id, move_into_view);
    }
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
    size_t last_undo_size = document->GetUndoSize();

    CaretState caret_state;
    SelectionState selection_state;

    if (before_state.IsEmpty())
    {
        before_state = document->GetLogicalEditorState();
    }
    else
    {
        document->SetEditorState(before_state); //it is redo
        for (auto _el : elements)
            _el->id.clear();
    }

    caret_state = document->caret->GetCaretState();
    selection_state = document->selection.GetState();

    FormattingScope s(window);

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
            ElementId changed_element;
            if (_el->DeleteElements(true, with_undo, changed_element))
            {
                Remake(changed_element, false);
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
                if (with_undo && last_undo_size < document->GetUndoSize())
                    document->Undo();
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
            _elements.emplace_back(new CodeString(*str));
            continue;
        }
        if (t->type == ElementType::PARAGRAPH && code)
        {
            //change type of paragraph
            _elements.emplace_back(new CodeParagraph((Paragraph*)t.get()));
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

    std::vector<ElementId> changed_elements;
    document->pasting = pasting;
    document->caret->notify = false;
    for (auto& _el : _elements)
    {
        _el->parent = nullptr;
        std::vector<ElementPtr> t{ElementPtr(_el->Clone())};
        ElementId changed_element;
        if (el->editable && !el->InsertElements(t, with_undo, changed_element))
        {
            if (with_undo && last_undo_size < document->GetUndoSize())
                document->Undo();
            document->pasting = false;
            document->caret->notify = true;
            return false;
        }
        
        if (document->caret->GetElement())
            el = document->GetElement(document->caret->GetElement()->id);

        for (size_t i = 0; i < changed_elements.size();)
        {
            if (IsChild(changed_element, changed_elements[i]))
                changed_elements.erase(changed_elements.begin() + i);
            else
                ++i;
        }

        auto it = std::find_if(changed_elements.begin(), changed_elements.end(), 
            [changed_element](ElementId& _el)
            {
                return _el == changed_element || IsChild(_el, changed_element);
            });
        if (it == changed_elements.end())
            changed_elements.push_back(changed_element); //remake all the changed elements after this circle
    }
    for (auto ch : changed_elements)
    {
        if (document->GetElement(ch))
            Remake(ch, true); //move into view
    }
    document->pasting = false;
    document->caret->notify = true;
    window->OnCaretMoved(document->MakeEditorState());
    document->UpdateFormats();
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
    size_t last_undo_size = document->GetUndoSize();

    CaretState caret_state;
    SelectionState selection_state;
    if (element_id.empty())
    {
        if (before_state.IsEmpty())
            before_state = document->GetLogicalEditorState();
        else
            document->SetEditorState(before_state); //it is redo
        caret_state = document->caret->GetCaretState();
        selection_state = document->selection.GetState();
    }
    else
    {
        before_state = document->GetLogicalEditorState();
        caret_state = document->caret->GetCaretState();
        //clear elements inside this element
        selection_state = SelectionState{element_id, 0, document->GetElement(element_id)->elements->Count()};
        EditorState s{caret_state, selection_state};
        document->SetEditorState(s);
    }

    auto DeleteElements = [&](ElementPtr el, bool _left, ElementId& changed_element, bool _with_undo)
    {
        assert(el != nullptr);
        return el->DeleteElements(_left, _with_undo, changed_element);
    };

    ElementId changed_element;
    if (selection_state.IsEmpty())
    {
        auto el = document->GetParent(caret_state.id);
        if (!el->editable)
            return false;
        if (DeleteElements(el, left, changed_element, with_undo))
        {
            document->UpdateLastSelection();
            Remake(changed_element, true); //move into view
            return true;
        }
    }
    else
    {
        bool merge_paragraphs = false;
        if (selection_state.state.size() >= 2)
        {
            ElementId p_id = selection_state.GetCommonElement();
            if (p_id.size() == 1)
            {
                if (selection_state.state[0].id != ElementId{0} && selection_state.state[selection_state.state.size() - 1].id != ElementId{0})
                {
                    merge_paragraphs = true;
                }
            }
        }

        if (with_undo)
        {
            uint start = 0, size = 0;
            ElementId p_id = selection_state.GetCommonElement(start, size);
            auto p = document->GetElement(p_id);
            if (document->IsString(p))
            {
                document->StoreUndo(p->parent->parent->id);
            }
            else
            {
                if (start == 0 && size == p->elements->Count())
                    document->StoreUndo(p_id, start, size, 1);
                else
                {
                    auto ch = GetChild(p_id, start);
                    document->StoreUndo(p_id, start, size, 
                        (merge_paragraphs || 
                        (selection_state.state[0].id == ElementId{0} && selection_state.state[selection_state.state.size() - 1].id != ElementId{0}) || 
                        (selection_state.state[0].id != ElementId{0} && selection_state.state[selection_state.state.size() - 1].id == ElementId{0}) || 
                        (p_id.size() == 1 && selection_state.GetCommonElement() != p_id)) ? 1 : 0, 
                        (document->IsParagraph(ch) ? UndoTask::UndoOperation::INSERT : UndoTask::UndoOperation::CHANGE));
                }
            }
        }

        std::vector<ElementId> changed_elements;
        for (int i = selection_state.state.size() - 1; i >= 0; --i)
        {
            ElementSelectionState& s = selection_state.state[i];
            auto el = document->GetElement(s.id);
            if (!el->editable)
                continue;
            if (!DeleteElements(el, left, changed_element, false))
            {
                if (with_undo && last_undo_size < document->GetUndoSize())
                    document->Undo();
                return false;
            }

            auto it = std::find_if(changed_elements.begin(), changed_elements.end(), 
                [changed_element](ElementId& _el)
                {
                    return _el == changed_element;
                });
            if (it == changed_elements.end())
                changed_elements.push_back(changed_element);  //remake all the changed elements after this circle
            document->UpdateLastSelection();
        }

        std::sort(changed_elements.begin(), changed_elements.end(), std::greater<>());

        for (auto ch : changed_elements)
        {
            if (document->GetElement(ch))
                Remake(ch, true); //move into view
        }

        if (merge_paragraphs)
        {
            auto _el = document->caret->GetElement();
            if (DeleteElements(document->GetElement(_el->id), document->caret->GetPos() == 0 ? true : false, changed_element, false))
            {
                document->UpdateLastSelection();
                Remake(changed_element, true);
            }
        }

        return true;
    }

    if (with_undo && last_undo_size < document->GetUndoSize())
        document->Undo();
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
    size_t last_undo_size = document->GetUndoSize();

    if (before_state.IsEmpty())
        before_state = document->GetLogicalEditorState();
    else
        document->SetEditorState(before_state); //it is redo

    CaretState caret_state = document->caret->GetCaretState();
    SelectionState selection_state = document->selection.GetState();

    ElementPtr el = document->GetParent(caret_state.id);
    assert(el);

    CaretState c;
    if (!selection_state.IsEmpty() && ((document->FindParent(caret_state.id, ElementType::CODE_BLOCK) == nullptr) || !elements[0]->UseSelection()))
    {
        //remove selection before insert
        auto DeleteElements = [&](ElementPtr _el)
        {
            assert(_el != nullptr);
            ElementId changed_element;
            if (_el->DeleteElements(true, with_undo, changed_element))
            {
                Remake(changed_element, false);
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
                if (with_undo && last_undo_size < document->GetUndoSize())
                    document->Undo();
                return false;
            }
            caret_state = document->caret->GetCaretState();
        }
    }

    bool insert_code_block = false;
    ElementId changed_element;
    if (elements[0]->type != ElementType::CODE_BLOCK)
    {
        if (document->FindParent(caret_state.id, ElementType::CODE_BLOCK) == nullptr)
        {
            //there is no code element - insert one in the current row
            auto row = document->FindParentRow(caret_state.id);
            if (!row)
            {
                if (with_undo && last_undo_size < document->GetUndoSize())
                    document->Undo();
                return false;
            }
            
            ElementPtr code(new CodeBlock(row.get(), document->cur_code_id));
            std::vector v{code};
            if (!row->InsertElements(v, with_undo, changed_element))
            {
                if (with_undo && last_undo_size < document->GetUndoSize())
                    document->Undo();
                return false;
            }
            insert_code_block = true;
            el = code->elements->Get(0)->elements->Get(0);
        }
    }

    std::vector<ElementPtr> _elements;
    for (int i = 0; i < elements.size(); ++i)
    {
        ElementPtr c(elements[i]->Clone());
        c->UpdateLevel(el->level);
        _elements.push_back(c);
    }

    if (pasting)
    {
        for (int i = 0; i < _elements.size(); ++i)
            _elements[i]->BeforePaste();
    }
    
    document->pasting = pasting;
    if (el->InsertElements(_elements, insert_code_block ? false : with_undo, changed_element))
    {
        Remake(changed_element, true); //move into view
        document->pasting = false;
        return true;
    }
    document->pasting = false;

    if (with_undo && last_undo_size < document->GetUndoSize())
        document->Undo();
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
    bool _set_bold, bool _set_italic, bool _set_underline, bool _set_strikethrough, bool _set_text_color, bool _set_text_bg_color, bool _with_undo) :
    ChangeStringFormatTask(_text, _format, _with_undo)
{
    set_family = _set_family;
    set_size = _set_size;
    set_bold = _set_bold;
    set_italic = _set_italic;
    set_underline = _set_underline;
    set_strikethrough = _set_strikethrough;
    set_text_color = _set_text_color;
    set_text_bg_color = _set_text_bg_color;
}

ChangeStringFormatTask::ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, uint _id) :
    Task(_text, _id), 
    format(_format)
{
}

bool ChangeStringFormatTask::Execute()
{
    size_t last_undo_size = document->GetUndoSize();

    CaretState caret_state;
    SelectionState selection_state;

    if (before_state.IsEmpty())
        before_state = document->GetLogicalEditorState();
    else
        document->SetEditorState(before_state); //it is redo

    caret_state = document->caret->GetCaretState();
    selection_state = document->selection.GetState();

    std::vector<ElementPtr> elements;
    for (int i = 0; i < selection_state.state.size(); ++i)
    {
        ElementSelectionState& s = selection_state.state[i];
        elements.push_back(document->GetElement(s.id));
    }

    auto get_string_format = 
        [&](String* str)
        {
            //set only actual params
            StringFormat f = *str->format;
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
            if (set_strikethrough)
                f.strikethrough = format->strikethrough;
            if (set_text_color)
                f.text_color = format->text_color;
            if (set_text_bg_color)
                f.text_bg_color = format->text_bg_color;
            return document->GetStringFormat(f.family, f.size, f.bold, f.italic, f.underline, f.strikethrough, f.text_color, f.text_bg_color);
        };
    
    std::function<bool (ElementPtr el, ElementId& changed_element)> change_string_format = 
        [&](ElementPtr el, ElementId& changed_element)
        {
            ElementId _changed_element;
            if (document->IsString(el))
            {
                StringFormatPtr _format = get_string_format((String*)el.get());
                if (!el->ChangeStringFormat(_format, with_undo, _changed_element))
                {
                    if (with_undo && last_undo_size < document->GetUndoSize())
                        document->Undo();
                    return false;
                }
                if (!changed_element.empty())
                    changed_element = GetCommonParent(changed_element, _changed_element);
                else
                    changed_element = _changed_element;
            }
            else
            {
                for (int i = 0; i < el->elements->Count(); ++i)
                {
                    auto _el = el->elements->Get(i);
                    if (!change_string_format(_el, _changed_element))
                        return false;
                    if (!changed_element.empty())
                        changed_element = GetCommonParent(changed_element, _changed_element);
                    else
                        changed_element = _changed_element;
                }
            }
            return true;
        };

    ElementId changed_element;
    for (int i = selection_state.state.size() - 1; i >= 0; --i)
    {
        ElementSelectionState& s = selection_state.state[i];
        auto el = elements[i];

        StringFormatPtr _format;
        ElementId _changed_element;
        if (document->IsString(el))
        {
            if (!change_string_format(el, _changed_element))
                return false;
            if (!changed_element.empty())
                changed_element = GetCommonParent(changed_element, _changed_element);
            else
                changed_element = _changed_element;
        }
        else
        {
            for (int i = s.start; i < s.start + s.size; ++i)
            {
                if (!change_string_format(el->elements->Get(i), _changed_element))
                    return false;
                if (!changed_element.empty())
                    changed_element = GetCommonParent(changed_element, _changed_element);
                else
                    changed_element = _changed_element;
            }
        }

        document->UpdateFormats();
    }

    Remake(changed_element, false);

    if (!selection_state.IsEmpty())
    {
        auto p_id = selection_state.state.size() == 1 ? GetParent(selection_state.state[0].id) : selection_state.GetCommonElement();
        Remake(p_id, false);
    }

    return true;
}

//ChangeParagraphFormatTask

ChangeParagraphFormatTask::ChangeParagraphFormatTask(ElementPtr _text, ElementId _element_id, const ParagraphFormatPtr& _format, bool _with_undo) :
    Task(_text),
    element_id(_element_id),
    format(_format)
{
    with_undo = _with_undo;
}

ChangeParagraphFormatTask::ChangeParagraphFormatTask(ElementPtr _text, ElementId _element_id, const ParagraphFormatPtr& _format, uint _id) :
    Task(_text, _id), 
    element_id(_element_id),
    format(_format)
{
}

bool ChangeParagraphFormatTask::Execute()
{
    size_t last_undo_size = document->GetUndoSize();

    ElementPtr el;
    if (before_state.IsEmpty())
    {
        before_state = document->GetLogicalEditorState();
        el = document->FindParentParagraph(element_id);
    }
    else
    {
        document->SetEditorState(before_state); //it is redo
        CaretState caret_state = document->caret->GetCaretState();
        el = document->FindParentParagraph(caret_state.id);
    }

    if (!el)
        return false;

    ElementId changed_element;
    if (before_state.selection_state.IsEmpty())
    {
        if (!el->ChangeParagraphFormat(format, with_undo, changed_element))
        {
            if (with_undo && last_undo_size < document->GetUndoSize())
                document->Undo();
            return false;
        }
    }
    else
    {
        //find all the selected paragraphs and change their formats
        std::vector<ElementId> ids;
        ElementId ch;
        std::vector<ElementLogicalSelectionState>& s = before_state.selection_state.state;
        for (size_t i = 0; i < s.size(); ++i)
        {
            if (s[i].id == text->id)
            {
                for (size_t j = s[i].start; j < s[i].start + s[i].size; ++j)
                {
                    ElementId _id = yutovo::GetChild(s[i].id, j);
                    if (std::find(ids.begin(), ids.end(), _id) == ids.end())
                    {
                        el = document->FindParentParagraph(_id);
                        if (!el->ChangeParagraphFormat(format, with_undo, ch))
                        {
                            if (with_undo && last_undo_size < document->GetUndoSize())
                                document->Undo();
                            return false;
                        }
                        ids.push_back(el->id);
                    }
                }
            }
            else
            {
                bool last_pos;
                ElementId _id = document->GetElementId(s[i].id, last_pos);
                if (std::find(ids.begin(), ids.end(), _id) == ids.end())
                {
                    el = document->FindParentParagraph(_id);
                    if (!el)
                        continue;
                    if (!el->ChangeParagraphFormat(format, with_undo, ch))
                    {
                        if (with_undo && last_undo_size < document->GetUndoSize())
                            document->Undo();
                        return false;
                    }
                    ids.push_back(el->id);
                }
            }
        }
        changed_element = GetCommonParent(ids);
    }

    Remake(changed_element, true);
    document->UpdateFormats();

    return true;
}

//ChangePageFormatTask

ChangePageFormatTask::ChangePageFormatTask(ElementPtr _text, const PageFormatPtr& _format) :
    Task(_text), 
    format(_format)
{
}

bool ChangePageFormatTask::Execute()
{
    document->current_page_format = format;

    Remake(ElementId{0}, true);
    document->UpdateFormats();

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
    
    Rect clear_rect = element->draw_rect.IsEmpty() ? element->GetAbsoluteRect() : element->draw_rect;
    window->ClearRect(clear_rect); //clear last rect before drawing
    element->Draw(); //draw element and update its rect
    element->UpdateDrawRect();

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
    
    window->OnResizeStarted();
    document->caret->block = true;
    document->break_remake = false;
    window->Resize(width, height);
    document->caret->block = false;
    Remake(text->id, false);
    window->OnResizeFinished();
    return true;
}

//UndoTask

UndoTask::UndoTask(ElementPtr _text, int _undo_id, ElementId _id, const int _pos, const int _delete_size, const uint task_id) :
    Task(_text, task_id),
    undo_id(_undo_id),
    id(document->GetLogicalId(_id)),
    delete_size(_delete_size),
    pos(_pos)
{
    before_state = document->GetLogicalEditorState();
}

UndoTask::UndoTask(ElementPtr _text, int _undo_id, ElementId _id, const int _pos, const int _size, const int _delete_size, 
    UndoOperation _undo_operation, const uint task_id) :
    Task(_text, task_id),
    undo_id(_undo_id),
    id(document->GetLogicalId(_id)),
    delete_size(_delete_size),
    undo_operation(_undo_operation),
    pos(_pos),
    size(_size)
{
    before_state = document->GetLogicalEditorState();
}

UndoTask::UndoTask(ElementPtr _text, int _undo_id, const uint task_id) : 
    Task(_text, task_id),
    undo_id(_undo_id),
    undo_operation(UndoOperation::CONFIG)
{
}

bool UndoTask::Execute()
{
    if (undo_operation == UndoOperation::DELETE)
    {
        auto p = document->GetLogicalElement(id);
        p->elements->RemoveAt(pos, size);
        Remake(p->id, true);
        return true;
    }

    if (undo_operation == UndoOperation::CONFIG)
    {
        Config config;
        if (!document->RestoreUndo(undo_id, config))
            return false;
        document->SetConfig(config, false);
        return true;
    }

    std::vector<ElementPtr> undo_elements;
    if (!document->RestoreUndo(undo_id, undo_elements))
        return false;

    document->caret->block = true;

    std::vector<ElementPtr> _elements;
    document->GetElements(id, _elements);
    for (auto& _el : _elements)
        document->RemoveErrorMarks(_el->id);

    ElementPtr p = document->GetLogicalElement(id);
    if (id.size() > 2 && (p->type != ElementType::CODE_ROW && p->type != ElementType::CODE_BLOCK && 
        !(undo_elements[0]->type == ElementType::CODE_ROW && p->parent->type != ElementType::CODE_PARAGRAPH)) || 
        (p->type == ElementType::CODE_BLOCK && undo_operation == UndoOperation::CHANGE))
    {
        p = document->GetLogicalParent(id);
    }
    
    ElementId remake_id = p->id;

    if (p->type == ElementType::TEXT)
    {
        if (delete_size > 0)
            p->elements->RemoveAt(pos, delete_size);
        for (int i = 0; i < undo_elements.size(); ++i)
            p->elements->Insert(undo_elements[i], pos + i);
    }
    else if (p->type == ElementType::PARAGRAPH)
    {
        if (undo_elements[0]->type == ElementType::PARAGRAPH)
        {
            if (delete_size > 0)
                p->parent->elements->RemoveAt(pos, delete_size);
            else
            {
                p->parent->elements->RemoveAt(GetChildPos(id), 
                    p->parent->elements->Count() > undo_elements.size() ? undo_elements.size() : p->parent->elements->Count());
            }
            for (size_t i = 0; i < undo_elements.size(); ++i)
                p->parent->elements->Insert(undo_elements[i], GetChildPos(id) + i);
            remake_id = p->parent->id;
        }
        else
        {
            p->elements->Clear();
            p->AddEmptyElement();
            auto r = p->elements->Get(0);
            r->elements->Clear();
            for (int i = 0; i < undo_elements.size(); ++i)
                r->elements->Insert(undo_elements[i], i);
        }
    }
    else
    {
        if (p->type == ElementType::ROW)
        {
            std::vector<ElementPtr> elements;
            document->GetElements(id, elements);
            pos = GetChildPos(elements[0]->id);
            for (int j = elements.size() - 1; j >= 0; --j)
                elements[j]->parent->elements->Remove(elements[j]);
            for (int i = 0; i < undo_elements.size(); ++i)
                p->elements->Insert(undo_elements[i], pos + i);
            remake_id = p->parent->id;
        }
        else if (document->IsFormula(p) && undo_elements.size() == 1)
        {
            auto ch = p->elements->Get(pos);
            ch->elements->ReplaceAll(*undo_elements[0]->elements);
        }
        else
        {
            if (undo_operation != UndoOperation::INSERT)
            {
                if (!(undo_elements[0]->type == ElementType::CODE_ROW && p->type != ElementType::CODE_PARAGRAPH))
                    pos = GetChildPos(id);
                if (delete_size > 0)
                {
                    if (delete_size <= pos + p->elements->Count())
                        p->elements->RemoveAt(pos, delete_size);
                }
                else if (pos < p->elements->Count())
                {
                    if (undo_elements[0]->type == ElementType::CODE_ROW)
                    {
                        if (p->type == ElementType::CODE_ROW && pos == 0)
                        {
                            p->elements->RemoveAt(0, p->elements->Count());
                        }
                        else
                        {
                            p->elements->RemoveAt(pos, undo_elements[0]->elements->Count() < p->elements->Count() - pos ? 
                                undo_elements[0]->elements->Count() : p->elements->Count() - pos);
                        }
                    }
                    else
                        p->elements->RemoveAt(pos, undo_elements.size() < p->elements->Count() - pos ? undo_elements.size() : p->elements->Count() - pos);
                }
            }
            for (int i = 0; i < undo_elements.size(); ++i)
            {
                auto _el = undo_elements[i];
                if (_el->type == ElementType::CODE_ROW && p->type == ElementType::CODE_ROW)
                {
                    if (undo_operation == UndoOperation::CHANGE)
                    {
                        p->elements->Clear();
                        for (int i = 0; i < _el->elements->Count(); ++i)
                            p->elements->Add(_el->elements->Get(i));
                    }
                    else
                    {
                        for (size_t j = 0; j < _el->elements->Count(); ++j)
                            p->elements->Insert(_el->elements->Get(j), pos + i + j);
                    }
                }
                else
                {
                    p->elements->Insert(undo_elements[i], pos + i);
                    p->elements->Get(pos + i)->AfterInsert(with_undo);
                }
            }
        }
    }

    Remake(remake_id, true);

    document->caret->block = false;
    document->SetEditorState(before_state);
    document->RemoveErrorMarks(remake_id);
    document->ReSolve(remake_id);
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
    move_into_view = false;
}

MoveCaretTask::MoveCaretTask(ElementPtr _text, CaretPtr _caret, Point _start, Point _end) :
    Task(_text),
    document(_text->document),
    caret(_caret),
    dir(MoveCaretDir::SELECT_TO_POINT),
    point(_start),
    end_point(_end),
    select(true)
{
    move_into_view = true;
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
            CaretState c;
            if (text->GetNearestCaretState(point.x, point.y, c) && c.id != text->id)
                caret->SetState(c, true);
        }
        break;
    case MoveCaretDir::LEFT:
        if (!document->selection.IsEmpty() && !select)
        {
            ElementSelection& s = document->selection.selection[0];
            if (document->IsParagraph(s.element) || document->IsRow(s.element) || s.element->type == ElementType::TEXT)
            {
                auto el = s.element->elements->Get(s.start);
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
            if (document->IsParagraph(s.element) || document->IsRow(s.element) || s.element->type == ElementType::TEXT)
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
    case MoveCaretDir::SELECT_TO_POINT:
        {
            if (!select)
                return false;
            document->selection.Clear();

            CaretState start, end;
            if (!text->GetNearestCaretState(point.x, point.y, start) || !text->GetNearestCaretState(end_point.x, end_point.y, end))
                return false;

            caret->notify = false;
            caret->SetState(start);
            if (start < end)
            {
                while (start < end)
                {
                    caret->MoveRight(selection);
                    start = caret->GetCaretState();
                }
                if (!selection->IsEmpty())
                    caret->SetState(selection->GetLastCaretState());
            }
            else
            {
                while (end < start)
                {
                    caret->MoveLeft(selection);
                    start = caret->GetCaretState();
                }
                if (!selection->IsEmpty())
                    caret->SetState(selection->GetFirstCaretState());
            }
            caret->notify = true;
        }
        break;
    case MoveCaretDir::SELECT_OUT:
        if (!select)
            return false;
        caret->SelectOut(selection);
        break;
    }

    if (move_into_view)
        document->UpdateCaretView();
    document->UpdateLastSelection();

    if (!select)
    {
        document->selection.Clear();
        document->UpdateLastSelection();
    }

    document->UpdateFormats();
    window->OnCaretMoved(document->MakeEditorState());
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
    document->current_paragraph_format = document->paragraph_formats->GetFormat("Text body");
    document->current_code_format = document->code_formats->GetFormat("Calculator");
    document->current_formula_format = document->formula_formats->GetFormat("Code");
    document->text.reset(new Text(text->document));
    document->text->Remake(true);
    document->MoveCaretToDocumentBegin(false);
    document->Redraw();
    return true;
}

//SaveTask

SaveTask::SaveTask(ElementPtr _text, const std::string _filename) :
    Task(_text),
    filename(_filename)
{
}

SaveTask::SaveTask(ElementPtr _text, std::u32string* _json_str) :
    Task(_text),
    json_str(_json_str)
{
}

bool SaveTask::Execute()
{
    rapidjson::Document json;
    auto& alloc = json.GetAllocator();
    json.SetObject();

    //add config
    rapidjson::Value config(rapidjson::kObjectType);
    document->config.ToJson(config, alloc);
    json.AddMember("config", config, alloc);

    //add string formats
    rapidjson::Value string_formats(rapidjson::kArrayType);
    document->string_formats->ToJson(string_formats, alloc);
    json.AddMember("string_formats", string_formats, alloc);

    //add paragraph formats
    rapidjson::Value paragraph_formats(rapidjson::kArrayType);
    document->paragraph_formats->ToJson(paragraph_formats, alloc);
    json.AddMember("paragraph_formats", paragraph_formats, alloc);

    document->saving = true;
    rapidjson::Value t(rapidjson::kObjectType);
    text->ToJson(t, alloc);
    json.AddMember("text", t, alloc);
    document->saving = false;

    //add caret and selection
    rapidjson::Value caret_state(rapidjson::kObjectType);
    auto c = document->caret->GetLogicalCaretState();
    c.ToJson(caret_state, alloc);
    json.AddMember("caret", caret_state, alloc);

    rapidjson::Value selection_state(rapidjson::kArrayType);
    auto s = document->selection.GetLogicalState();
    s.ToJson(selection_state, alloc);
    json.AddMember("selection", selection_state, alloc);

    rapidjson::StringBuffer buffer;
    if (document->config.pretty_json)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
    }
    std::string str = buffer.GetString();

    if (json_str)
        *json_str = ToUtfString(str);
    else
    {
        try
        {
            std::ofstream file(filename);
            file.exceptions(~std::ofstream::goodbit);
            file << str;
            file.close();
        }
        catch (const std::ios_base::failure& ex)
        {
            window->OnSaveResult(id, IOResult::InputStreamError);
            LOG_ERROR("Error saving file '{}': {}", filename, ex.what());
            return false;
        }
    }

    document->save_task_id = document->last_modify_task_id;

    window->OnSaveResult(id, IOResult::Success);
    return true;
}

//LoadTask

LoadTask::LoadTask(ElementPtr _text, const std::string _filename) :
    Task(_text),
    filename(_filename)
{
}

LoadTask::LoadTask(ElementPtr _text, const std::u32string& _json_str, const int _document_id) :
    Task(_text),
    json_str(_json_str),
    document_id(_document_id)
{
}

bool LoadTask::Execute()
{
    ElementPtr t;
    std::string str;
    rapidjson::Document doc;

    if (!json_str.empty())
    {
        auto str = ToBasicString(json_str);
        if (doc.Parse<0>(str.c_str()).HasParseError() || !doc.IsObject() || !LoadJson(doc))
        {
            window->OnLoadResult(id, IOResult::InputStreamError, document_id);
            return false;
        }

        //load text
        rapidjson::Value _text = doc["text"].GetObject();
        t = ElementPtr(CreateFromJson(nullptr, document, _text, doc.GetAllocator()));
    }
    else if (filename.substr(filename.find_last_of(".") + 1) == "yut")
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            window->OnLoadResult(id, IOResult::InputStreamError, document_id);
            LOG_ERROR("Error loading file '{}': File not open", filename);
            return false;
        }

        rapidjson::IStreamWrapper isw{file};

        doc.ParseStream(isw);
        if (doc.HasParseError() || !doc.IsObject() || !LoadJson(doc))
        {
            window->OnLoadResult(id, IOResult::InputStreamError, document_id);
            LOG_ERROR("Error parsing file '{}'", filename);
            return false;
        }

        //load text
        rapidjson::Value _text = doc["text"].GetObject();
        t = ElementPtr(CreateFromJson(nullptr, document, _text, doc.GetAllocator()));
    }
    else //".txt" and others load as text
    {
        try
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                window->OnLoadResult(id, IOResult::InputStreamError, document_id);
                LOG_ERROR("Error loading file '{}': File not open", filename);
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
            window->OnLoadResult(id, IOResult::InputStreamError, document_id);
            LOG_ERROR("Error loading file '{}': {}", filename, ex.what());
            return false;
        }

        t.reset(new Text(document));
    }

    if (!t)
    {
        LOG_ERROR("Error loading file '{}': document not parsed", filename);
        window->OnLoadResult(id, IOResult::InputStreamError, document_id);
        return false;
    }
    
    document->ResetTasks();
    document->RemoveUserIdentifiers();
    document->text = t;
    if (!str.empty())
    {
        document->MoveCaretToDocumentBegin(false);
        document->InsertString(str, false);
    }
    document->text->Remake(true);
    document->text->ReSolve();

    bool r = false;
    if (doc.IsObject())
    {
        LogicalCaretState c;
        LogicalSelectionState s;
        if (doc.HasMember("caret") && doc["caret"].IsObject())
        {
            if (c.FromJson(doc["caret"], doc.GetAllocator()))
            {
                if (doc.HasMember("selection") && doc["selection"].IsArray())
                {
                    if (s.FromJson(doc["selection"], doc.GetAllocator()))
                    {
                        ElementId _id = yutovo::GetParent(c.id);
                        if (document->GetLogicalElement(_id) == nullptr) //check caret state
                        {
                            ElementPtr p = nullptr;
                            while (!p && !_id.empty())
                            {
                                p = document->GetLogicalElement(_id);
                                if (p && !p->HasCaretState())
                                    p.reset();
                                _id = yutovo::GetParent(_id);
                            }
                            if (p)
                                document->caret->SetState(p->id);
                            else
                            {
                                CaretState c;
                                document->text->GetFirstCaretState(c, nullptr);
                                document->caret->SetState(c);
                            }
                        }
                        else
                        {
                            bool r = true;
                            if (s.state.size() > 0) //check selection state
                            {
                                for (auto& s : s.state)
                                {
                                    std::vector<ElementPtr> elements;
                                    document->GetElements(s.id, elements);
                                    int c = 0;
                                    for (auto& el : elements)
                                        c += el->elements->Count();
                                    if (elements.empty() || c < s.start || c < s.start + s.size)
                                    {
                                        r = false;
                                        break;
                                    }
                                }
                            }
                            if (r)
                            {
                                LogicalEditorState editor_state{c, s};
                                document->SetEditorState(editor_state);
                            }
                            else
                            {
                                auto el = document->GetLogicalElement(GetParent(c.id));
                                if (el)
                                    document->caret->SetState(el->id);
                                else
                                {
                                    CaretState c;
                                    document->text->GetFirstCaretState(c, nullptr);
                                    document->caret->SetState(c);
                                }
                            }
                        }
                        r = true;
                    }
                }
            }
        }
    }

    if (!r)
        document->MoveCaretToDocumentBegin(false);

    document->Redraw();
    window->OnLoadResult(id, IOResult::Success, document_id);
    return true;
}

bool LoadTask::LoadJson(rapidjson::Document& doc)
{
    auto& alloc = doc.GetAllocator();

    if (doc.HasMember("config") && doc["config"].IsObject())
    {
        //load config
        document->config.FromJson(doc["config"], alloc);
        document->solver.SetLocale(document->config.language);
        document->SetLocale(document->config.language, false);
    }

    if (doc.HasMember("string_formats") && doc["string_formats"].IsArray())
    {
        //load string formats
        document->string_formats->FromJson(doc["string_formats"], alloc);
    }

    if (doc.HasMember("paragraph_formats"))
    {
        //load paragraph formats
        document->paragraph_formats->FromJson(document, doc["paragraph_formats"], alloc);
    }

    if (!doc.HasMember("text") || !doc["text"].IsObject())
    {
        LOG_ERROR("File '{}' does not contain text", filename);
        return false;
    }

    return true;
}

//CopyTask

CopyTask::CopyTask(ElementPtr _text, std::u32string& _out_json, std::u32string& _out_text, bool _cut) :
    Task(_text),
    out_json(_out_json),
    out_text(_out_text),
    cut(_cut)
{
}

bool CopyTask::Execute()
{
    auto before_state = document->MakeEditorState();
    SelectionState& selection_state = before_state.selection_state;
    if (selection_state.IsEmpty())
    {
        window->OnCopyResult(CopyResult::EmptySelection);
        return false;
    }

    std::vector<ElementPtr> copy;
    for (int i = 0; i < selection_state.state.size(); ++i)
    {
        ElementId _id = selection_state.state[i].id;
        document->GetElement(_id)->Copy(copy);
    }

    if (copy.size() == 1 && copy[0]->type == ElementType::TEXT)
    {
        ElementPtr t = copy[0];
        for (int i = 0; i < t->elements->Count(); ++i)
            copy.push_back(t->elements->Get(i));
        copy.erase(copy.begin());
    }

    //concatenate rows of one paragraph
    for (size_t i = 1; i < copy.size();)
    {
        ElementPtr el1 = copy[i - 1];
        ElementPtr el2 = copy[i];
        if (document->IsRow(el1) && document->IsRow(el2) && el1->parent->id == el2->parent->id)
        {
            for (int j = 0; j < el2->elements->Count();)
                el1->elements->Move(el2->elements->Get(j), el1->elements->Count());
            copy.erase(copy.begin() + i);
        }
        else
            ++i;
    }

    auto s = selection_state.state[selection_state.state.size() - 1];
    ElementId _id = GetChild(s.id, s.start + s.size - 1);
    std::vector<ElementPtr> _copy;
    //transform all the non-empty paragraphs into empty ones plus rows
    for (size_t i = 0; i < copy.size(); ++i)
    {
        ElementPtr el = copy[i];
        if (el->type == ElementType::PARAGRAPH)
        {
            if (i > 0 && copy[i - 1]->type != ElementType::PARAGRAPH)
                _copy.push_back(ElementPtr(new Paragraph(document, true)));
            if (!el->IsEmpty())
                _copy.push_back(((Paragraph*)el.get())->GetPlainRow());
            if (i < copy.size() - 1)
                _copy.push_back(ElementPtr(new Paragraph(document, true)));
            else
            {
                if (!document->GetElement(_id)->parent->elements->IsLast(_id))
                    _copy.push_back(ElementPtr(new Paragraph(document, true)));
            }
        }
        else
        {
            if (i > 0)
            {
                auto& _el = copy[i - 1];
                if (!document->IsParagraph(_el))
                {
                    ElementId c_id = GetCommonParent(_el->id, el->id);
                    ElementPtr c_el = document->GetElement(c_id);
                    if (c_el && (c_el->type == ElementType::TEXT || c_el->type == ElementType::CODE_BLOCK))
                        _copy.push_back(document->CreateParagraph(el->id));
                }
            }
            _copy.push_back(el);
        }
    }

    copy = _copy;

    for (auto& el : copy)
    {
        out_text += el->ToText();
        if (document->IsParagraph(el))
            out_text += U"\n";
        el->parent = nullptr; //these elements have no parent
    }

    rapidjson::Document json;
    auto& alloc = json.GetAllocator();
    json.SetObject();

    //add string formats
    rapidjson::Value string_formats(rapidjson::kArrayType);
    document->string_formats->ToJson(string_formats, alloc);
    json.AddMember("string_formats", string_formats, alloc);

    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& el : copy)
    {
        rapidjson::Value t(rapidjson::kObjectType);
        el->ToJson(t, alloc);
        arr.PushBack(t, alloc);
    }
    json.AddMember("copy", arr, alloc);

    rapidjson::StringBuffer buffer;
    if (document->config.pretty_json)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
    }
    out_json = ToUtfString(buffer.GetString());

    window->OnCopyResult(CopyResult::Success);

    if (cut)
        document->DeleteElements(true, true);
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
    case ElementType::REAL_RESULT:
    case ElementType::INTEGER_RESULT:
    case ElementType::RATIONAL_RESULT:
    case ElementType::COMPLEX_RESULT:
    {
        ResultRow* r = dynamic_cast<ResultRow*>(el.get());
        if (!r)
            return false;
        document->RemoveErrorMarks(r->id);
        r->PutResult(result);
        break;
    }
    case ElementType::ASSIGNMENT:
    {
        Assignment* r = dynamic_cast<Assignment*>(el.get());
        if (!r)
            return false;
        document->RemoveErrorMarks(r->id);
        r->PutResult(result);
        break;
    }
    default:
        return false;
    }

    if (!result.error.id.empty() && result.error.error_code != ErrorCode::SOLVER_RESTARTED_ERROR && result.error.error_code != ErrorCode::OK)
    {
        ElementPtr p = document->FindElementOrParent(result.error.id, ElementType::ASSIGNMENT);
        if (p)
        {
            //put error mark
            Assignment* el = (Assignment*)p.get();
            ElementId err_id = el->last_expression.GetElement(result.error.pos);
            if (!err_id.empty())
            {
                auto el = document->GetElement(err_id);
                if (el)
                {
                    int s = 1;
                    uint p = el->parent->elements->GetElementPos(el->id);
                    if (el->type == ElementType::CODE_STRING)
                    {
                        //add all the strings to the error mark
                        uint n = p;
                        while (el->parent->elements->Count() > ++n && el->parent->elements->Get(n)->type == ElementType::CODE_STRING)
                            ++s;
                    }
                    if (s > 1)
                        document->AddErrorMark(el->parent->id, p, s);
                    else
                        document->AddErrorMark(err_id, 0, el->elements->Count());
                    document->Redraw(el->parent->id, false);
                }
            }
        }
    }

    auto _el = document->FindParent(el->id, ElementType::EQUATION);
    if (_el)
        Remake(_el->id, true);
    else
    {
        _el = document->FindParent(el->id, ElementType::ASSIGNMENT);
        if (_el)
            Remake(_el->id, true);
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
    std::vector<ElementId> code_blocks;
    if (id.empty())
    {
        //recalculate all the code blocks in the document
        text->GetElements(ElementType::CODE_BLOCK, code_blocks); //find all code blocks
        for (ElementId _id : code_blocks)
        {
            auto el = document->GetElement(_id);
            CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
            c->ReSolve(false, true);
            if (!document->changed_elements.empty())
            {
                for (auto ch : document->changed_elements)
                    Remake(ch, true);
                document->changed_elements.clear();
            }
        }
        return true;
    }

    auto el = document->FindParent(id, ElementType::CODE_BLOCK);
    if (!el)
    {
        if (id != text->id)
            return false;
        //resolve all code blocks
        text->GetElements(ElementType::CODE_BLOCK, code_blocks);
        for (ElementId _id : code_blocks)
        {
            auto el = document->GetElement(_id);
            CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
            if (c)
            {
                c->ReSolve(); //resolve all the connected code blocks above and the current one
                if (!document->changed_elements.empty())
                {
                    for (auto ch : document->changed_elements)
                        Remake(ch, true);
                    document->changed_elements.clear();
                }
            }
        }
        return true;
    }
    
    CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
    uint code_id = c->code_id;
    text->GetElements(ElementType::CODE_BLOCK, code_blocks); //find all code blocks
    for (ElementId _id : code_blocks)
    {
        auto el = document->GetElement(_id);
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        if (c && c->code_id == code_id)
        {
            c->ReSolve(); //resolve all the connected code blocks above and the current one
            if (!document->changed_elements.empty())
            {
                for (auto ch : document->changed_elements)
                    Remake(ch, true);
                document->changed_elements.clear();
            }
        }
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
    std::vector<ElementId> code_blocks;
    std::vector<ElementId> solvings;
    std::vector<std::string> id_arr;
    boost::split(id_arr, identifier, boost::is_any_of("()"));

    if (el)
    {
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        uint code_id = c->code_id;

        auto p = document->FindElementOrParent(after_id, ElementType::EQUATION);
        if (p)
            solvings.push_back(p->id);
        
        c->GetElementsBelow(after_id, ElementType::EQUATION, solvings); //get equations below in the current code block
        for (ElementId _id : solvings)
        {
            auto _el = document->GetElement(_id);
            Equation* eq = dynamic_cast<Equation*>(_el.get());
            for (auto& s : id_arr)
            {
                if (eq->Depends(s))
                {
                    eq->last_expression.Reset();
                    eq->ReSolve();
                    Remake(eq->id, false);
                }
            }
        }

        solvings.clear();
        c->GetElementsBelow(after_id, ElementType::ASSIGNMENT, solvings); //get assignments below in the current code block
        for (ElementId _id : solvings)
        {
            auto _el = document->GetElement(_id);
            Assignment* s = dynamic_cast<Assignment*>(_el.get());
            for (auto& d : id_arr)
            {
                if (s->Depends(d))
                    document->RemoveErrorMarks(s->id);
            }
        }

        text->GetElementsBelow(c->id, ElementType::CODE_BLOCK, code_blocks); //find all code blocks below
        for (ElementId _id : code_blocks)
        {
            auto el = document->GetElement(_id);
            CodeBlock* _c = dynamic_cast<CodeBlock*>(el.get());
            if (_c && _c->code_id == code_id)
            {
                solvings.clear();
                c->GetElements(ElementType::EQUATION, solvings);
                for (ElementId _id : solvings)
                {
                    auto _el = document->GetElement(_id);
                    Equation* eq = dynamic_cast<Equation*>(_el.get());
                    for (auto& s : id_arr)
                    {
                        if (eq->Depends(s))
                            eq->ReSolve();
                    }
                }

                solvings.clear();
                c->GetElements(ElementType::ASSIGNMENT, solvings); //get assignments below in the current code block
                for (ElementId _id : solvings)
                {
                    auto _el = document->GetElement(_id);
                    Assignment* s = dynamic_cast<Assignment*>(_el.get());
                    for (auto& d : id_arr)
                    {
                        if (s->Depends(d))
                            document->RemoveErrorMarks(s->id);
                    }
                }
            }
        }
    }
    else
    {
        el = document->GetElement(GetParent(after_id));
        if (!el)
            return false;
        text->GetElementsBelow(el->id, ElementType::CODE_BLOCK, code_blocks); //find all code blocks below
        for (ElementId _id : code_blocks)
        {
            auto _el = document->GetElement(_id);
            CodeBlock* _c = dynamic_cast<CodeBlock*>(_el.get());
            if (_c)
            {
                solvings.clear();
                _c->GetElements(ElementType::EQUATION, solvings);
                for (ElementId _id : solvings)
                {
                    _el = document->GetElement(_id);
                    Equation* eq = dynamic_cast<Equation*>(_el.get());
                    for (auto& s : id_arr)
                    {
                        if (eq->Depends(s))
                            eq->ReSolve(false, true);
                    }
                }

                solvings.clear();
                _c->GetElements(ElementType::ASSIGNMENT, solvings);
                for (ElementId _id : solvings)
                {
                    auto _el = document->GetElement(_id);
                    Assignment* s = dynamic_cast<Assignment*>(_el.get());
                    for (auto& d : id_arr)
                    {
                        if (s->Depends(d))
                            document->RemoveErrorMarks(s->id);
                    }
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

//SetResultTask

SetResultTask::SetResultTask(ElementPtr _text, ElementId _id, ResultType _result_type, bool _with_undo) :
    Task(_text),
    id(_id),
    result_type(_result_type)
{
    with_undo = _with_undo;
}

bool SetResultTask::Execute()
{
    if (before_state.IsEmpty())
        before_state = document->MakeEditorState();
    else
        document->SetEditorState(before_state); //it is redo

    auto el = document->FindParent(id, ElementType::EQUATION);
    if (!el)
        return false;
    if (!((Equation*)el.get())->SetResult(result_type, with_undo))
        return false;
    Remake(el->id, true);
    return true;
}

//SetResultParamsTask

SetResultParamsTask::SetResultParamsTask(ElementPtr _text, ElementId _id, Notation _default_notation, Notation _result_notation, bool _with_undo) :
    Task(_text),
    id(_id),
    default_notation(_default_notation),
    result_notation(_result_notation)
{
    with_undo = _with_undo;
}

SetResultParamsTask::SetResultParamsTask(ElementPtr _text, ElementId _id, FractionForm _fraction_form, bool _with_undo) :
    Task(_text),
    id(_id),
    fraction_form(_fraction_form)
{
    with_undo = _with_undo;
}

SetResultParamsTask::SetResultParamsTask(ElementPtr _text, ElementId _id, uint _precision, uint _exp, AngleMeasure _result_angle_measure, bool _with_undo) :
    Task(_text),
    id(_id),
    precision(_precision),
    exp(_exp),
    result_angle_measure(_result_angle_measure)
{
    with_undo = _with_undo;
}

SetResultParamsTask::SetResultParamsTask(ElementPtr _text, ElementId _id, ComplexForm _complex_form, bool _with_undo) :
    Task(_text),
    id(_id),
    complex_form(_complex_form)
{
    with_undo = _with_undo;
}

SetResultParamsTask::SetResultParamsTask(ElementPtr _text, ElementId _id, yutovo_calculator::Unit _unit, bool _with_undo) :
    Task(_text),
    id(_id),
    unit(_unit)
{
    with_undo = _with_undo;
}

bool SetResultParamsTask::Execute()
{
    if (before_state.IsEmpty())
        before_state = document->MakeEditorState();
    else
        document->SetEditorState(before_state); //it is redo
    
    auto el = document->FindParent(id, ElementType::EQUATION);
    if (!el)
        return false;
    
    Equation* eq = (Equation*)el.get();
    if (precision != -1 || exp != -1 || result_angle_measure != AngleMeasure::None)
    {
        if (eq->SetConfig(precision, exp, result_angle_measure, with_undo))
        {
            Remake(el->id, true);
            return true;
        }
    }
    if (default_notation != Notation::None && result_notation != Notation::None)
    {
        if (eq->SetConfig(default_notation, result_notation, with_undo))
        {
            Remake(el->id, true);
            return true;
        }
    }
    if (fraction_form != FractionForm::None)
    {
        if (eq->SetConfig(fraction_form, with_undo))
        {
            Remake(el->id, true);
            return true;
        }
    }
    if (complex_form != ComplexForm::None)
    {
        if (eq->SetConfig(complex_form, with_undo))
        {
            Remake(el->id, true);
            return true;
        }
    }
    if (!unit.IsEmpty())
    {
        if (eq->SetConfig(unit, with_undo))
        {
            Remake(el->id, true);
            return true;
        }
    }

    return false;
}

//SetStringTask

SetStringTask::SetStringTask(ElementPtr _text, const std::u32string& _str, ElementId _element_id) :
    Task(_text),
    str(_str),
    element_id(_element_id)
{
}

bool SetStringTask::Execute()
{
    auto s = document->GetElement(element_id);
    if (!document->IsString(s))
        return false;
    auto* _str = (String*)s.get();
    _str->translate = false;
    _str->SetString(str);
    Remake(s->id, true);
    return true;
}

//SetConfigTask

SetConfigTask::SetConfigTask(ElementPtr _text, const Config& _config, bool _with_undo) :
    Task(_text),
    config(_config)
{
    with_undo = _with_undo;
}

SetConfigTask::SetConfigTask(ElementPtr _text, const std::string& _config_str, bool _with_undo) :
    Task(_text),
    config_str(_config_str)
{
    with_undo = _with_undo;
}

bool SetConfigTask::Execute()
{
    if (!config_str.empty())
    {
        config = document->config;
        if (!config.FromJson(config_str)) //update only actual part from the string
            return false;
    }

    if (with_undo)
        document->StoreUndo(document->config);
    
    bool remake = false;
    Config& c = document->config;
    if (config.use_numbers_gaps != c.use_numbers_gaps || config.binary_gap != c.binary_gap || config.octal_gap != c.octal_gap || 
        config.decimal_gap != c.decimal_gap || config.hexadecimal_gap != c.hexadecimal_gap)
    {
        remake = true;
    }
    if (config.language != c.language)
    {
        document->solver.SetLocale(config.language);

        //update identifiers for all code blocks
        std::vector<ElementId> code_blocks;
        text->GetElements(ElementType::CODE_BLOCK, code_blocks);
        for (auto& c : code_blocks)
        {
            auto el = document->GetElement(c);
            CodeBlock* _el = dynamic_cast<CodeBlock*>(el.get());
            document->ListIdentifiers(_el->code_id);
        }
        remake = true;
    }
    document->config = config;
    document->current_code_format->border_color = config.code_block_border_color;

    document->logger->SetLevel((int)config.log_level);

    if (remake)
        Remake(text->id, false);
    document->Redraw(text->id, false);

    return true;
}

}
