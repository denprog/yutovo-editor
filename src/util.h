#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <boost/locale.hpp>
#include <yutovo_calculator/parser_exception.h>
#include <yutovo_calculator/math_helper.h>
#include <yutovo_service/types.h>
#include <boost/uuid/uuid.hpp>
#include <rapidjson/document.h>

typedef unsigned int uint;

namespace yutovo
{

using namespace yutovo_calculator;

class Window;

enum class ElementType
{
    NONE = 0,
    TEXT,
    PARAGRAPH,
    ROW,
    STRING,
    CODE_BLOCK,
    CODE_PARAGRAPH,
    CODE_ROW,
    CODE_STRING,
    CODE_COLUMN,
    SHAPE,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVISION,
    POWER,
    SQUARE_ROOT,
    NTH_ROOT,
    EQUATION,
    OPEN_FENCE,
    CLOSE_FENCE,
    REAL_RESULT,
    INTEGER_RESULT,
    RATIONAL_RESULT,
    COMPLEX_RESULT,
    AUTO_RESULT,
    ERROR_RESULT,
    ASSIGNMENT,
    SUBSCRIPT,
    EXCLAMATION,
    AND,
    OR,
    XOR,
    PERCENT,
    IMAGE
};

class Document;
class Element;

typedef std::vector<uint> ElementId;
typedef std::vector<uint> LogicalId; //logical Id does not include row id, so it does not depend on the formatting of rows

struct Point
{
    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return !operator==(other);
    }

    void Set(const int _x, const int _y)
    {
        x = _x;
        y = _y;
    }
    
    int x = 0;
    int y = 0;
};

struct Size
{
    bool operator==(const Size& other) const
    {
        return width == other.width && height == other.height;
    }

    bool operator!=(const Size& other) const
    {
        return !operator==(other);
    }

    void Set(const int _width, const int _height)
    {
        width = _width;
        height = _height;
    }

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

    bool operator==(const Rect& compare) const
    {
        return left == compare.left && top == compare.top && width == compare.width && height == compare.height;
    }

    bool operator!=(const Rect& compare) const
    {
        return !operator==(compare);
    }

    Point GetTopLeft()
    {
        return Point{left, top};
    }

    int GetRight() const
    {
        return left + width;
    }

    int GetBottom() const
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

    void SetSize(int _width, int _height)
    {
        width = _width;
        height = _height;
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
    bool IsPointInside(const int x, const int y);
    bool Intersects(const Rect& rect);

    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;
};

struct Color
{
    bool operator==(const Color& compare) const
    {
        return a == compare.a && r == compare.r && g == compare.g && b == compare.b;
    }

    uint32_t ToInt() const
    {
        return (a << 24) + (r << 16) + (g << 8) + b;
    }

    std::string ToString() const
    {
        return "rgba(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a) + ")";
    }

    std::string ToHex() const
    {
        std::stringstream s;
        s << "#";
        s << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)r << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)g << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)b;
        return s.str();
    }

    static Color FromInt(uint32_t c)
    {
        uint8_t a = (c & 0xff000000) >> 24;
        uint8_t r = (c & 0x00ff0000) >> 16;
        uint8_t g = (c & 0x0000ff00) >> 8;
        uint8_t b = (c & 0x000000ff);
        return Color{a, r, g, b};
    }

    static Color FromHex(std::string c)
    {
        if ((c.size() != 9 && c.size() != 7) || c[0] != '#')
            return Color{};
        c.erase(0, 1);
        const unsigned long val = stoul(c, nullptr, 16);
        if (c.size() == 7)
            return Color{0xff, uint8_t((val >> 16) & 0xff), uint8_t((val >> 8) & 0xff), uint8_t((val) & 0xff)};
        return Color{uint8_t((val >> 24) & 0xff), uint8_t((val >> 16) & 0xff), uint8_t((val >> 8) & 0xff), uint8_t((val) & 0xff)};
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

    uint8_t a = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

struct FormattingScope
{
    FormattingScope(Window* _window);
    ~FormattingScope();

    Window* window;
};

std::string IdToString(const ElementId& id);
ElementId IdFromString(const std::string& id);

bool IsChild(const ElementId& parent_id, const ElementId& child_id);
bool IsDirectChild(const ElementId& parent_id, const ElementId& child_id);

ElementId GetParent(const ElementId& id);
ElementId GetChild(const ElementId& id, uint pos);
int GetChildPos(const ElementId& id);
int GetChildPos(const ElementId& parent_id, const ElementId& child_id);
ElementId GetPrevPos(const ElementId& id);

ElementId GetWithParent(const ElementId id, const ElementId parent_id);

ElementId GetCommonParent(const ElementId& id1, const ElementId& id2);
ElementId GetCommonParent(const std::vector<ElementId>& ids);

Element* CreateFromJson(Element* parent, Document* document, rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

std::u32string ToUtfString(const std::string& str);
std::string ToBasicString(const std::u32string& str);

std::string ErrorCodeToString(const yutovo_service::ErrorCode error_code);
std::string ErrorCodeToString(const yutovo_calculator::ParserExceptionCode parser_error_code);

std::string AngleMeasureToString(const AngleMeasure angle_measure);
std::string NotationToString(const Notation notation);

struct ErrorMark
{
    ElementId id;
    int start = 0;
    int size = 0;
};

bool IsLess(const ElementId& id1, const ElementId& id2);

}

#endif
