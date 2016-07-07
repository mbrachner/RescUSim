#pragma once
#include "Position.h"
#include "Bounds.h"
#include <stdint.h>
#include <tuple>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

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
	Weather(py::array_t<double, py::array::c_style | py::array::forcecast> wdata, double minx, double miny, unsigned int resolution);
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
	void load(const std::string & name);
	~Weather();

private:
	py::object infoWdata;
	WeatherData *weatherData;
	size_t dimX;
	size_t dimY;
	unsigned int resolution;
	size_t numScenarios;
	Bounds bounds;
};

