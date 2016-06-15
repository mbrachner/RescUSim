#include "PointWeather.h"
#include "Position.h"
#include <iostream>
#include <iomanip> 
//#include <nanoflann.hpp>

PointWeather::PointWeather(DPoint *p, size_t _numpoints)
{
	points = p;
	numpoints = _numpoints;
	/*std::cout << "Init Pointweather" << std::endl;
	std::cout << std::setprecision(14);

	for (size_t c = 0; c < numpoints; c++) {
		std::cout << p[c].x << " " << p[c].y << std::endl;
	}
	std::cout << numpoints << std::endl;*/
}

double PointWeather::wdAtP(double scenario, double x, double y) {
	for (size_t c = 0; c < numpoints; c++) {
		std::cout << points[c].x << " " << points[c].y << std::endl;
	}
	return points[0].x;
}



PointWeather::~PointWeather()
{
}
