#pragma once
#include "Position.h"

struct Bounds {
	Bounds(double minx, double maxx, double miny, double maxy);
	bool within(Position p);
	double minx;
	double miny;
	double maxx;
	double maxy;
};