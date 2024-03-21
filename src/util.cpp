#include "util.h"
#include <limits>
#include <cmath>
#include "row.h"
#include "str.h"
#include "image.h"
#include "paragraph.h"
#include "element.h"
#include "formulas/code_block.h"
#include "formulas/code_paragraph.h"
#include "formulas/code_row.h"
#include "formulas/code_string.h"
#include "formulas/plus.h"
#include "formulas/minus.h"
#include "formulas/multiply.h"
#include "formulas/division.h"
#include "formulas/nth_root.h"
#include "formulas/square_root.h"
#include "formulas/result.h"
#include "formulas/power.h"
#include "formulas/equation.h"
#include "formulas/fences.h"
#include "formulas/assignment.h"
#include "formulas/subscript.h"
#include "formulas/exclamation.h"
#include "formulas/and.h"
#include "formulas/or.h"
#include "formulas/xor.h"
#include "formulas/percent.h"
#include "formulas/sum.h"
#include "formulas/product.h"
#include "formulas/unit.h"
#include "formulas/comma.h"

namespace yutovo
{

using namespace yutovo_service;

//Rect

int Rect::DistToPoint(const int x, const int y)
{
	int min_dist = std::numeric_limits<int>::max(), dist;
	
	//calculate distances to each side
	min_dist = DistToSegment(x, y, left, top, left, GetBottom());
	
	dist = DistToSegment(x, y, left, top, GetRight(), top);
	if (min_dist > dist)
		min_dist = dist;

	dist = DistToSegment(x, y, GetRight(), top, GetRight(), GetBottom());
	if (min_dist > dist)
		min_dist = dist;

	dist = DistToSegment(x, y, left, GetBottom(), GetRight(), GetBottom());
	if (min_dist > dist)
		min_dist = dist;
	
	return min_dist;
}

int Rect::DistToSegment(const int x, const int y, const int seg_x1, const int seg_y1, const int seg_x2, const int seg_y2)
{
	if (seg_x1 == seg_x2 && seg_y1 == seg_y2)
		return round(sqrt(pow(x - seg_x1, 2) + pow(y - seg_y1, 2)));

	double d1 = sqrt(pow(x - seg_x1, 2) + pow(y - seg_y1, 2));
	double d2 = sqrt(pow(x - seg_x2, 2) + pow(y - seg_y2, 2));
	double r = sqrt(pow(seg_x1 - seg_x2, 2) + pow(seg_y1 - seg_y2, 2));
	double p = (d1 + d2 + r) / 2;

	if (p - d1 < 0)
		d1 = p;
	if (p - d2 < 0)
		d2 = p;

	double h = 2 * sqrt(p * (p - r) * (p - d1) * (p - d2)) / r;
	double s = sqrt(pow(d1, 2) - pow(h, 2));

	if (s > r)
		return round(d2);

	s = sqrt(pow(d2, 2) - pow(h, 2));

	if (s > r)
		return round(d1);

	return round(h);
}

bool Rect::IsPointInside(const int x, const int y)
{
    return x >= left && x <= left + width && y >= top && y <= top + height;
}

bool Rect::Intersects(const Rect& rect)
{
    return !(left > rect.GetRight() || rect.left > GetRight() || top > rect.GetBottom() || rect.top > GetBottom());
}

//FormattingScope

FormattingScope::FormattingScope(Window* _window) :
    window(_window)
{
    window->OnFormattingStarted();
}

FormattingScope::~FormattingScope()
{
    window->OnFormattingFinished();
}

std::string IdToString(const ElementId& id)
{
	std::string res;
	for (size_t i = 0; i < id.size(); ++i)
	{
		res += std::to_string(id[i]);
		if (i < id.size() - 1)
			res += ",";
	}
	return res;
}

ElementId IdFromString(const std::string& id)
{
    ElementId res;
    std::stringstream s(id);
    for (int i; s >> i;)
    {
        res.push_back(i);
        if (s.peek() == ',')
            s.ignore();
    }
    return res;
}

bool IsChild(const ElementId& parent_id, const ElementId& child_id)
{
    if (child_id.size() <= parent_id.size() || parent_id.empty())
        return false;
    for (size_t i = 0; i < parent_id.size(); ++i)
    {
        if (child_id[i] != parent_id[i])
            return false;
    }
    return true;
}

bool IsDirectChild(const ElementId& parent_id, const ElementId& child_id)
{
    if (IsChild(parent_id, child_id) && child_id.size() == parent_id.size() + 1)
        return true;
    return false;
}

ElementId GetParent(const ElementId& id)
{
    ElementId _id(id);
    _id.erase(_id.end() - 1);
    return _id;
}

ElementId GetChild(const ElementId& id, uint pos)
{
    ElementId _id(id);
    _id.push_back(pos);
    return _id;
}

int GetChildPos(const ElementId& id)
{
    assert(!id.empty());
    return id[id.size() - 1];
}

int GetChildPos(const ElementId& parent_id, const ElementId& child_id)
{
    assert(parent_id.size() < child_id.size());
    return child_id[parent_id.size()];
}

ElementId GetPrevPos(const ElementId& id)
{
    ElementId _id(id);
    --_id[_id.size() - 1];
    return _id;
}

ElementId GetWithParent(const ElementId id, const ElementId parent_id)
{
    ElementId _id(parent_id);
    for (int i = _id.size(); i < id.size(); ++i)
        _id.push_back(id[i]);
    return _id;
}

ElementId GetCommonParent(const ElementId& id1, const ElementId& id2)
{
    int i = 0;
    while (id1.size() > i && id2.size() > i && id1[i] == id2[i])
        ++i;
    return ElementId(id1.begin(), id1.begin() + i);
}

ElementId GetCommonParent(const std::vector<ElementId>& ids)
{
    if (ids.empty())
        return {};

    int pos = 0;
    bool f = true;
    uint j = 0;
    while (f)
    {
        for (int i = 0; i < ids.size(); ++i)
        {
            const ElementId& _id = ids[i];
            if (_id.size() <= pos || _id[pos] != j)
            {
                f = false;
                break;
            }
        }
        if (!f || ids[0].size() <= ++pos)
            break;
        j = ids[0][pos];
    }

    return ElementId(ids[0].begin(), ids[0].begin() + pos);
}

Element* CreateFromJson(Element* parent, Document* document, rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    using CreateFunc = Element*(*)(Element*, Document*, const rapidjson::Value&, rapidjson::Document::AllocatorType&);
    static std::map<ElementType, CreateFunc> create_elements = 
        {
            {ElementType::TEXT, &Text::FromJson},
            {ElementType::PARAGRAPH, &Paragraph::FromJson},
            {ElementType::ROW, &Row::FromJson},
            {ElementType::STRING, &String::FromJson},
            {ElementType::IMAGE, &Image::FromJson},
            {ElementType::CODE_BLOCK, &CodeBlock::FromJson},
            {ElementType::CODE_PARAGRAPH, &CodeParagraph::FromJson},
            {ElementType::CODE_ROW, &CodeRow::FromJson},
            {ElementType::CODE_STRING, &CodeString::FromJson},
            {ElementType::SHAPE, &Shape::FromJson},
            {ElementType::PLUS, &Plus::FromJson},
            {ElementType::MINUS, &Minus::FromJson},
            {ElementType::MULTIPLY, &Multiply::FromJson},
            {ElementType::DIVISION, &Division::FromJson},
            {ElementType::POWER, &Power::FromJson},
            {ElementType::SQUARE_ROOT, &SquareRoot::FromJson},
            {ElementType::NTH_ROOT, &NthRoot::FromJson},
            {ElementType::EQUATION, &Equation::FromJson},
            {ElementType::OPEN_FENCE, &OpenFence::FromJson},
            {ElementType::CLOSE_FENCE, &CloseFence::FromJson},
            {ElementType::REAL_RESULT, &RealResult::FromJson},
            {ElementType::INTEGER_RESULT, &IntegerResult::FromJson},
            {ElementType::RATIONAL_RESULT, &RationalResult::FromJson},
            {ElementType::COMPLEX_RESULT, &ComplexResult::FromJson},
            {ElementType::AUTO_RESULT, &AutoResult::FromJson},
            {ElementType::ERROR_RESULT, &ErrorResult::FromJson},
            {ElementType::ASSIGNMENT, &Assignment::FromJson},
            {ElementType::SUBSCRIPT, &Subscript::FromJson},
            {ElementType::EXCLAMATION, &Exclamation::FromJson},
            {ElementType::AND, &And::FromJson},
            {ElementType::OR, &Or::FromJson},
            {ElementType::XOR, &Xor::FromJson},
            {ElementType::PERCENT, &Percent::FromJson},
            {ElementType::IMAGE, &Image::FromJson},
            {ElementType::SUM, &Sum::FromJson},
            {ElementType::PRODUCT, &Product::FromJson},
            {ElementType::UNIT, &Unit::FromJson},
            {ElementType::COMMA, &Comma::FromJson}
        };

    if (!value.HasMember("type") || !value["type"].IsInt())
        return nullptr;
    
    ElementType type = (ElementType)value["type"].GetInt();
    CreateFunc f = create_elements[type];
    if (!f)
        return nullptr;
    Element* el = f(parent, document, value, alloc);
    if (!el)
        return nullptr;
    if (value.HasMember("elements"))
    {
        if (!el->elements->FromJson(document, value, alloc) || !el->AfterFromJson())
        {
            delete el;
            return nullptr;
        }
    }
    return el;
}

std::u32string ToUtfString(const std::string& str)
{
    return boost::locale::conv::utf_to_utf<char32_t>(str);
}

std::string ToBasicString(const std::u32string& str)
{
    return boost::locale::conv::utf_to_utf<char>(str);
}

std::string ErrorCodeToString(const yutovo_service::ErrorCode error_code)
{
    static std::map<yutovo_service::ErrorCode, std::string> error_code_str = 
        {
            {ErrorCode::OK, "Ok"},
            {ErrorCode::UNKNOWN_COMMAND, "Unknown command"},
            {ErrorCode::JSON_ERROR, "Json error"},
            {ErrorCode::NO_FIELD_ERROR, "No field"},
            {ErrorCode::FIELD_ERROR, "Field error"},
            {ErrorCode::SOLVER_ERROR, "Solver error"},
            {ErrorCode::PARSER_ERROR, "Parser error"},
            {ErrorCode::OPERATION_ERROR, "Operation error"},
            {ErrorCode::SOLVER_RESTARTED_ERROR, "Solver restarted"}
        };
    return error_code_str[error_code];
}

std::string ErrorCodeToString(const yutovo_calculator::ParserExceptionCode parser_error_code)
{
    using namespace yutovo_calculator;
    static std::map<ParserExceptionCode, std::string> error_code_str = 
        {
            {ParserExceptionCode::None, "Ok"},
            {ParserExceptionCode::SyntaxError, "Syntax error"},
            {ParserExceptionCode::WrongArgumentsCount, "Wrong arguments count"},
            {ParserExceptionCode::UnknownIdentifier, "Unknown identifier"},
            {ParserExceptionCode::ExpressionExpected, "Expression expected"},
            {ParserExceptionCode::DivisionByZero, "Division by zero"},
            {ParserExceptionCode::Overflow, "Overflow"},
            {ParserExceptionCode::ArgumentIsOver, "Argument is over"},
            {ParserExceptionCode::UnitsAreIncompatible, "Units are incompatible"},
            {ParserExceptionCode::ConversionDoesNotFit, "Conversion does not fit"}
        };
    return error_code_str[parser_error_code];
}

std::string AngleMeasureToString(const AngleMeasure angle_measure)
{
    switch (angle_measure)
    {
    case AngleMeasure::Radian:
        return "rad";
    case AngleMeasure::Degree:
        return "deg";
    case AngleMeasure::Grad:
        return "grad";
    case AngleMeasure::None:
        return "";
    }
    return "";
}

std::string NotationToString(const Notation notation)
{
    switch (notation)
    {
    case Notation::Binary:
        return "bin";
    case Notation::Octal:
        return "oct";
    case Notation::Decimal:
        return "dec";
    case Notation::Hexadecimal:
        return "hex";
    case Notation::None:
        return "";
    }
    return "";
}

bool IsLess(const ElementId& id1, const ElementId& id2)
{
	for (size_t i = 0, j = 0; i < id1.size() && j < id2.size(); ++i, ++j)
	{
		if (id1[i] > id2[j])
			return false;
		if (id1[i] < id2[j])
			return true;
	}
	return id1.size() < id2.size();
}

}
