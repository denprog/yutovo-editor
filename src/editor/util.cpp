#include "util.h"
#include <limits>
#include <cmath>
#include <boost/archive/binary_iarchive.hpp>
#include "row.h"
#include "str.h"
#include "page.h"
#include "paragraph.h"
#include "element.h"

namespace yutovo
{

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

bool IsChild(const ElementId& parent_id, const ElementId& child_id)
{
    if (child_id.size() < parent_id.size() || parent_id.empty())
        return false;
    for (size_t i = 0; i < parent_id.size(); ++i)
    {
        if (child_id[i] != parent_id[i])
            return false;
    }
    return true;
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

template<>
void RegisterTypes(UserDataAdapter<DocumentUserData, boost::archive::binary_iarchive>& archive)
{
    boost::serialization::void_cast_register<yutovo::Text, yutovo::Element>(static_cast<yutovo::Text*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Page, yutovo::Element>(static_cast<yutovo::Page*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Paragraph, yutovo::Element>(static_cast<yutovo::Paragraph*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Row, yutovo::Element>(static_cast<yutovo::Row*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::String, yutovo::Element>(static_cast<yutovo::String*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::StringElements, yutovo::Elements>(static_cast<yutovo::StringElements*>(NULL), static_cast<yutovo::Elements*>(NULL));

    archive.template register_type<yutovo::Text>();
    archive.template register_type<yutovo::Page>();
    archive.template register_type<yutovo::Paragraph>();
    archive.template register_type<yutovo::Row>();
    archive.template register_type<yutovo::String>();
    archive.template register_type<yutovo::StringElements>();
}

template<>
void RegisterTypes(boost::archive::binary_iarchive& archive)
{
    boost::serialization::void_cast_register<yutovo::Text, yutovo::Element>(static_cast<yutovo::Text*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Page, yutovo::Element>(static_cast<yutovo::Page*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Paragraph, yutovo::Element>(static_cast<yutovo::Paragraph*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Row, yutovo::Element>(static_cast<yutovo::Row*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::String, yutovo::Element>(static_cast<yutovo::String*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::StringElements, yutovo::Elements>(static_cast<yutovo::StringElements*>(NULL), static_cast<yutovo::Elements*>(NULL));

    archive.template register_type<yutovo::Text>();
    archive.template register_type<yutovo::Page>();
    archive.template register_type<yutovo::Paragraph>();
    archive.template register_type<yutovo::Row>();
    archive.template register_type<yutovo::String>();
    archive.template register_type<yutovo::StringElements>();
}

template<>
void RegisterTypes(boost::archive::binary_oarchive& archive)
{
    boost::serialization::void_cast_register<yutovo::Text, yutovo::Element>(static_cast<yutovo::Text*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Page, yutovo::Element>(static_cast<yutovo::Page*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Paragraph, yutovo::Element>(static_cast<yutovo::Paragraph*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::Row, yutovo::Element>(static_cast<yutovo::Row*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::String, yutovo::Element>(static_cast<yutovo::String*>(NULL), static_cast<yutovo::Element*>(NULL));
    boost::serialization::void_cast_register<yutovo::StringElements, yutovo::Elements>(static_cast<yutovo::StringElements*>(NULL), static_cast<yutovo::Elements*>(NULL));

    archive.template register_type<yutovo::Text>();
    archive.template register_type<yutovo::Page>();
    archive.template register_type<yutovo::Paragraph>();
    archive.template register_type<yutovo::Row>();
    archive.template register_type<yutovo::String>();
    archive.template register_type<yutovo::StringElements>();
}

}
