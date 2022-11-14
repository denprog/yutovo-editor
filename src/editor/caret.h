#ifndef __CARET_H__
#define __CARET_H__

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "window.h"
#include "caret_state.h"
#include "element.h"

namespace yutovo
{

class Text;

struct CaretSettings
{
    uint blink_delay = 500;
};

//Caret of the document
class Caret
{
public:
    Caret(Window* _window, Text* _text);

    void SetState(const CaretState& caret_state, bool update_x_pos = false);

    CaretState GetCaretState();

    void SetVisible(bool _visible);

    void Show();
    void Hide();
    void Blink();

    void MoveToDocumentBegin(bool select);
    void MoveToDocumentEnd(bool select);
    void MoveHome(bool select);
    void MoveEnd(bool select);
    void MoveLeft(bool select);
    void MoveRight(bool select);
    void MoveUp(bool select);
    void MoveDown(bool select);
    void MoveWordLeft(bool select);
    void MoveWordRight(bool select);

    void UpdateXPos();

public:
    Element* current_element;
    uint current_pos;
    Selections selections;
    Selections last_selections;

private:
    bool show = false;
    bool visible = true;

    Window* window;
    Text* text;

    Rect caret_rect;

    //CaretState x_caret_state; //for moving vertical
    Element* last_x_element = nullptr;
    uint last_x_pos = 0;
};

}

#endif
