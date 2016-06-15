#include "Weather.h"
#include <iostream>
#include <iomanip>

Weather::Weather() :bounds({ 0,0,0,0 }) {

}

Weather::Weather(WeatherData *weatherData, size_t numScenarios, size_t dimX, size_t dimY, unsigned int resolution, Bounds bounds)
	:weatherData(weatherData), numScenarios(numScenarios), dimX(dimX), dimY(dimY), resolution(resolution), bounds(bounds)
{
	
}

//#pragma optimize("",off)
BilinearWeights Weather::getBilinearWeights(double x, double y, double z) {
	// http://paulbourke.net/miscellaneous/interpolation/

	x = (x - bounds.minx) / resolution;
	y = (y - bounds.miny) / resolution;

	BilinearWeights bw;
	int px = (int)x; // floor of x
	int py = (int)y; // floor of y
	int pz = (int)z; // floor of z


	// Calculate the weights for each pixel
	double fx = x - px;
	double fy = y - py;
	double fz = z - pz;
	double fx1 = 1.0f - fx;
	double fy1 = 1.0f - fy;
	double fz1 = 1.0f - fz;

	bw.w1 = fx1 * fy1 * fz1 ;
	bw.w2 = fx  * fy1 * fz1 ;
	bw.w3 = fx1 * fy  * fz1 ;
	bw.w4 = fx1 * fy1 * fz  ;
	bw.w5 = fx  * fy1 * fz  ;
	bw.w6 = fx1 * fy  * fz  ;
	bw.w7 = fx  * fy  * fz1 ;
	bw.w8 = fx  * fy  * fz  ;

	return bw;
}


inline size_t Weather::getIndex(size_t ix, size_t iy, size_t iz) {
	return dimY* ix + iy + dimX*dimY* iz;
}

//#pragma optimize("",off)
std::tuple<double,double> Weather::windAt(double scenario, double x, double y) {
	int ix = (int)(x - bounds.minx) / resolution;
	int iy = (int)(y-bounds.miny)/resolution;
	int iz = (int)scenario;
	//printf("Windat %d %d %d\n", ix, iy, iz);

	//std::cout << (bounds.maxx - bounds.minx) / getDimX() << (bounds.maxy - bounds.miny) / getDimY() << std::endl;
	//std::cout << std::setprecision(9);
	//std::cout << bounds.minx << " " << bounds.miny << " " << bounds.maxx << " " << bounds.maxy << std::endl;
	BilinearWeights bw = getBilinearWeights(x, y, scenario);
	double vx1 = weatherData[getIndex( ix   , iy   , iz   )].wind_xcomp;
	double vx2 = weatherData[getIndex((ix+1), iy   , iz   )].wind_xcomp;
	double vx3 = weatherData[getIndex( ix   ,(iy+1), iz   )].wind_xcomp;
	double vx4 = weatherData[getIndex( ix   , iy   ,(iz+1))].wind_xcomp;
	double vx5 = weatherData[getIndex((ix+1), iy   ,(iz+1))].wind_xcomp;
	double vx6 = weatherData[getIndex( ix   ,(iy+1),(iz+1))].wind_xcomp;
	double vx7 = weatherData[getIndex((ix+1),(iy+1), iz   )].wind_xcomp;
	double vx8 = weatherData[getIndex((ix+1),(iy+1),(iz+1))].wind_xcomp;

		
	double vy1 = weatherData[getIndex( ix   , iy   , iz   )].wind_ycomp;
	double vy2 = weatherData[getIndex((ix+1), iy   , iz   )].wind_ycomp;
	double vy3 = weatherData[getIndex( ix   ,(iy+1), iz   )].wind_ycomp;
	double vy4 = weatherData[getIndex( ix   , iy   ,(iz+1))].wind_ycomp;
	double vy5 = weatherData[getIndex((ix+1), iy   ,(iz+1))].wind_ycomp;
	double vy6 = weatherData[getIndex( ix   ,(iy+1),(iz+1))].wind_ycomp;
	double vy7 = weatherData[getIndex((ix+1),(iy+1), iz   )].wind_ycomp;
	double vy8 = weatherData[getIndex((ix+1),(iy+1),(iz+1))].wind_ycomp;

	double wind_xcomp = vx1*bw.w1 + vx2*bw.w2 + vx3*bw.w3 + vx4*bw.w4 + vx5*bw.w5 + vx6*bw.w6 + vx7*bw.w7 + vx8*bw.w8;
	double wind_ycomp = vy1*bw.w1 + vy2*bw.w2 + vy3*bw.w3 + vy4*bw.w4 + vy5*bw.w5 + vy6*bw.w6 + vy7*bw.w7 + vy8*bw.w8;

	double wsp = sqrt(pow(wind_xcomp, 2) + pow(wind_ycomp, 2));
	double wd = atan2(wind_ycomp, wind_xcomp);

	return  std::make_tuple(wsp,wd);
}

float Weather::wdAt(double scenario, double x, double y) {
	//return weatherData[(size_t)y + dimY*(size_t)x + dimX*dimY*(size_t)scenario].wsp;
	return 0;
}

float Weather::wspAt(double scenario, double x, double y) {
	//return weatherData[(size_t)y + dimY*(size_t)x + dimX*dimY*(size_t)scenario].wsp;
	return 0;
}

double Weather::hsAt(double scenario, double x, double y) {
	int ix = (int)(x - bounds.minx) / resolution;
	int iy = (int)(y - bounds.miny) / resolution;
	int iz = (int)scenario;

	BilinearWeights bw = getBilinearWeights(x, y, scenario);
	double v1 = weatherData[getIndex(ix, iy, iz)].hs;
	double v2 = weatherData[getIndex((ix + 1), iy, iz)].hs;
	double v3 = weatherData[getIndex(ix, (iy + 1), iz)].hs;
	double v4 = weatherData[getIndex(ix, iy, (iz + 1))].hs;
	double v5 = weatherData[getIndex((ix + 1), iy, (iz + 1))].hs;
	double v6 = weatherData[getIndex(ix, (iy + 1), (iz + 1))].hs;
	double v7 = weatherData[getIndex((ix + 1), (iy + 1), iz)].hs;
	double v8 = weatherData[getIndex((ix + 1), (iy + 1), (iz + 1))].hs;

	double hs = v1*bw.w1 + v2*bw.w2 + v3*bw.w3 + v4*bw.w4 + v5*bw.w5 + 
		v6*bw.w6 + v7*bw.w7 + v8*bw.w8;


	return hs;
}

bool Weather::lightAt(double scenario, double x, double y)
{
	int ix = (int)(x - bounds.minx) / resolution;
	int iy = (int)(y - bounds.miny) / resolution;
	int iz = (int)scenario;

	return weatherData[getIndex(ix, iy, iz)].light!=0;
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

unsigned int Weather::getResolution()
{
	return resolution;
}

Bounds Weather::getBounds()
{
	return bounds;
}


Weather::~Weather()
{
}


