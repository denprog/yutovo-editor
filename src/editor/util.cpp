#include "util.h"
#include <limits>
#include <cmath>

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

std::string ToString(const ElementId& id)
{
	std::string res;
	for (size_t i = 0; i < id.size(); ++i)
	{
		res += std::to_string(i);
		if (i < id.size() - 1)
			res += ",";
	}
	return res;
}

}
