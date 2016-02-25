#include "Weather.h"



Weather::Weather(float * wd, float * wsp, float * hs, size_t numScenarios, size_t dimX, size_t dimY)
	:wd(wd),wsp(wsp),hs(hs), numScenarios(numScenarios), dimX(dimX), dimY(dimY)
{
	
}

float Weather::wdAt(size_t scenario, size_t x, size_t y) {
	return wd[y+dimY*x+dimX*dimY*scenario];
}

float Weather::wspAt(size_t scenario, size_t x, size_t y) {
	return wsp[y + dimY*x + dimX*dimY*scenario];
}

float Weather::hsAt(size_t scenario, size_t x, size_t y) {
	return hs[y + dimY*x + dimX*dimY*scenario];
}

size_t Weather::getNumScenarios() { return numScenarios; }


Weather::~Weather()
{
}
