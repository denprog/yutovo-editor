#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "caret_state.h"
#include "selection.h"

namespace yutovo
{

struct EditorState
{
    bool operator==(const EditorState& s);
    bool operator!=(const EditorState& s);

    bool IsEmpty();
    
#ifdef TEST
    std::string ToString() const;
#endif

    CaretState caret_state;
    SelectionState selection_state;
};

struct LogicalEditorState
{
    bool IsEmpty();
    
    LogicalCaretState caret_state;
    LogicalSelectionState selection_state;
};

}

#endif
