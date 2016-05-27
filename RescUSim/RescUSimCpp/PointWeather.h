#pragma once
#include "Weather.h"


struct DPoint {
	DPoint(double x_, double y_) {
		x = x_; y = y_;
	}
	double x;
	double y;
};

class PointWeather :
	public Weather
{
public:
	PointWeather(DPoint *p, size_t numpoints);
	double wdAtP(double scenario, double x, double y);
	~PointWeather();
private:
	size_t numpoints;
	DPoint *points;
};

