#pragma once
#include "Position.h"
#include "Bounds.h"
#include <stdint.h>

struct WeatherData {
	float wsp;
	float wd;
	float hs;
	unsigned int light;
};

class Weather
{
public:
	Weather();
	Weather(WeatherData *weatherData, size_t numScenarios, size_t dimX, size_t dimY, Bounds bounds);
	float wdAt(size_t scenario, size_t x, size_t y);
	float wspAt(size_t scenario, size_t x, size_t y);
	float hsAt(size_t scenario, size_t x, size_t y);
	bool lightAt(size_t scenario, size_t x, size_t y);
	WeatherData *getWeatherDataPtr();
	size_t getNumScenarios();
	size_t getMemSize();
	size_t getDimX();
	size_t getDimY();
	Bounds getBounds();
	~Weather();

private:
	WeatherData *weatherData;
	size_t dimX;
	size_t dimY;
	size_t numScenarios;
	Bounds bounds;
};

