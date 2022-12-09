#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include "caret_state.h"

namespace yutovo
{

class Document;
class Element;

struct Point
{
    int x = 0;
    int y = 0;
};

struct Size
{
    int width = 0;
    int height = 0;
};

struct Rect
{
    Rect() = default;
    
    Rect(int _left, int _top, int _width, int _height) :
        left(_left),
        top(_top),
        width(_width),
        height(_height)
    {
    }

    Point GetTopLeft()
    {
        return Point{left, top};
    }

    int GetRight()
    {
        return left + width;
    }

    int GetBottom()
    {
        return top + height;
    }

    void SetRect(int _left, int _top, int _right, int _bottom)
    {
        left = _left;
        top = _top;
        width = _right - _left;
        height = _bottom - _top;
    }

    void Move(int _left, int _top)
    {
        left = _left;
        top = _top;
    }

    void Reset()
    {
        left = 0;
        top = 0;
        width = 0;
        height = 0;
    }

    bool IsEmpty()
    {
        return width == 0 && height == 0;
    }
    
    int DistToPoint(const int x, const int y);
    int DistToSegment(const int x, const int y, const int seg_x1, const int seg_y1, const int seg_x2, const int seg_y2);

    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;
};

struct Color
{
    uint32_t ToInt() const
    {
        return (a << 24) + (r << 16) + (g << 8) + b;
    }

    static Color Red()
    {
        return Color{0xff, 0xff, 0, 0};
    }

    static Color Green()
    {
        return Color{0xff, 0, 0xff, 0};
    }

    static Color Blue()
    {
        return Color{0xff, 0, 0, 0xff};
    }

    static Color Black()
    {
        return Color{0xff, 0x0, 0x0, 0x0};
    }

    static Color White()
    {
        return Color{0xff, 0xff, 0xff, 0xff};
    }

    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

std::string IdToString(const ElementId& id);

bool IsChild(const ElementId& parent_id, const ElementId& child_id);

struct DocumentUserData
{
    Document* document = nullptr;
};

template <class UserData, class Archive>
UserData& GetUserData(Archive&);

template <class UserData, class Archive>
class UserDataAdapter : public Archive
{
public:
    template <class ... Args>
    UserDataAdapter(UserData& _user_data, Args&& ... args) :
        Archive(std::forward<Args>(args) ...),
        user_data(_user_data)
    {
    }

private:
    friend UserData& GetUserData<UserData>(Archive& ar);
    UserData& user_data;
};

template <class UserData, class Archive>
UserData& GetUserData(Archive& ar)
{
    return dynamic_cast<UserDataAdapter<UserData, Archive>&>(ar).user_data;
}

template<class Archive>
void RegisterTypes(Archive& archive);

}

#endif
