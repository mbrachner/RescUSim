#include "Weather.h"
#include <iostream>


Weather::Weather(WeatherData *weatherData, size_t numScenarios, size_t dimX, size_t dimY, Bounds bounds)
	:weatherData(weatherData), numScenarios(numScenarios), dimX(dimX), dimY(dimY), bounds(bounds)
{
	
}


float Weather::wdAt(size_t scenario, size_t x, size_t y) {
	return weatherData[y + dimY*x + dimX*dimY*scenario].wd;
}

float Weather::wspAt(size_t scenario, size_t x, size_t y) {
	return weatherData[y + dimY*x + dimX*dimY*scenario].wsp;
}

float Weather::hsAt(size_t scenario, size_t x, size_t y) {
	return weatherData[y + dimY*x + dimX*dimY*scenario].hs;
}

bool Weather::lightAt(size_t scenario, size_t x, size_t y)
{
	return weatherData[y + dimY*x + dimX*dimY*scenario].light!=0;
}

WeatherData * Weather::getWeatherDataPtr()
{
	return weatherData;
}

size_t Weather::getNumScenarios() { return numScenarios; }

size_t Weather::getMemSize() {
	return dimX*dimY*getNumScenarios()*sizeof(WeatherData);
}

size_t Weather::getDimX()
{
	return dimX;
}

size_t Weather::getDimY()
{
	return dimY;
}

Bounds Weather::getBounds()
{
	return bounds;
}


Weather::~Weather()
{
}


