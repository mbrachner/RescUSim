#include "Bounds.h"

Bounds::Bounds(double minx, double maxx, double miny, double maxy) :minx(minx), maxx(maxx), miny(miny), maxy(maxy)
{
}

bool Bounds::within(Position p)
{
	return ((p.x >= minx) & (p.x <= maxx) & (p.y >= miny) & (p.y <= maxy));
}