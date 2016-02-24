#pragma once
class Weather
{
public:
	Weather(float *wd, float *wsp, float *hs, size_t numScenarios, size_t dimX, size_t dimY);
	float wdAt(size_t scenario, size_t x, size_t y);
	float wspAt(size_t scenario, size_t x, size_t y);
	float hsAt(size_t scenario, size_t x, size_t y);
	~Weather();

private:
	float *wd;
	float *wsp;
	float *hs;
	size_t dimX;
	size_t dimY;
	size_t numScenarios;
};

