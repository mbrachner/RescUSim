#pragma once
#include "Position.h"
#include "Bounds.h"
#include <stdint.h>
#include <tuple>

struct WeatherData {
	//float wsp;
	//float wd;
	float wind_xcomp;
	float wind_ycomp;
	float hs;
	unsigned char light;
};

struct BilinearWeights {
	double w1;
	double w2;
	double w3;
	double w4;
	double w5;
	double w6;
	double w7;
	double w8;
};

class Weather
{
public:
	Weather();
	Weather(WeatherData *weatherData, size_t numScenarios, size_t dimX, size_t dimY, unsigned int resolution, Bounds bounds);
	BilinearWeights getBilinearWeights(double x, double y, double z);
	size_t getIndex(size_t ix, size_t iy, size_t iz);
	std::tuple<double, double> windAt(double scenario, double x, double y);
	float wdAt(double scenario, double x, double y);
	float wspAt(double scenario, double x, double y);
	double hsAt(double scenario, double x, double y);
	bool lightAt(double scenario, double x, double y);
	WeatherData *getWeatherDataPtr();
	size_t getNumScenarios();
	size_t getMemSize();
	size_t getDimX();
	size_t getDimY();
	unsigned int getResolution();
	Bounds getBounds();
	~Weather();

private:
	WeatherData *weatherData;
	size_t dimX;
	size_t dimY;
	unsigned int resolution;
	size_t numScenarios;
	Bounds bounds;
};

