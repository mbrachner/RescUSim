#include "Weather.h"
#include <iostream>
#include <iomanip>
#include <H5Cpp.h>
#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif

Weather::Weather() :bounds({ 0,0,0,0 }) {

}

Weather::Weather(WeatherData *weatherData, size_t numScenarios, size_t dimX, size_t dimY, unsigned int resolution, Bounds bounds)
	:weatherData(weatherData), numScenarios(numScenarios), dimX(dimX), dimY(dimY), resolution(resolution), bounds(bounds)
{
	
}

Weather::Weather(py::array_t<double, py::array::c_style | py::array::forcecast> wdata, double minx, double miny, unsigned int resolution) : bounds(Bounds{ 0,0,0,0 })
{
	std::cout << "Refcount: " << wdata.ref_count() << std::endl;
	infoWdata = py::object(wdata);
	py::buffer_info infoWdata1 = wdata.request();
	weatherData = (WeatherData *)infoWdata1.ptr;
	//wdata.inc_ref();
	std::cout << "Refcount: " << wdata.ref_count() << std::endl;
	bounds = { minx, minx + infoWdata1.shape[1] * resolution, miny, miny + infoWdata1.shape[2] * resolution };
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
	printf("Windat Param %f %f %f\n", scenario, x, y);
	printf("Windat Index %d %d %d\n", iz, ix, iy);

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
	throw 1;
	return 0;
}

float Weather::wspAt(double scenario, double x, double y) {
	//return weatherData[(size_t)y + dimY*(size_t)x + dimX*dimY*(size_t)scenario].wsp;
	throw 1;
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

size_t Weather::getNumScenarios() { 
	//std::cout << "RefCount: " << wdaa
	return numScenarios; }

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

void Weather::load(const std::string& name)
{
	std::cout << "Loading " << name << std::endl;
	H5File file(name, H5F_ACC_RDONLY);
	DataSet dataset = file.openDataSet("weather_data");
	std::cout << "Number of attributes: " << dataset.getNumAttrs() << std::endl;
	dataset.openAttribute("resolution").read(PredType::NATIVE_UINT, &resolution);
	//float bounds[4];
	dataset.openAttribute("bounds").read(PredType::NATIVE_DOUBLE, &bounds);

	std::cout << "Resolution: " << resolution << std::endl;
	std::cout << "Bounds: " << bounds.minx << "," << bounds.miny << "," << bounds.maxx << "," << bounds.maxy << std::endl;
	DataSpace ds = dataset.getSpace();
	int dim = ds.getSimpleExtentNdims();
	std::cout << "Dimensions: " << dim << std::endl;
	hsize_t dims_out[3];
	ds.getSimpleExtentDims(dims_out, NULL);
	std::cout << "Size: " << dims_out[0] << "," << dims_out[1] << "," << dims_out[2] << std::endl;
	dimX = dims_out[1];
	dimY = dims_out[2];
	numScenarios = dims_out[0];
	std::cout << "Size: " << dims_out[0] * dims_out[1] * dims_out[2] << std::endl;
	std::cout << "Dataset typeclass: " << dataset.getTypeClass() << "," << H5T_COMPOUND << std::endl;
	std::cout << "Dataset size: " << dataset.getInMemDataSize() << "," << H5T_COMPOUND << std::endl;


	CompType mtype2(sizeof(WeatherData));
	mtype2.insertMember("wind_xcomp", HOFFSET(WeatherData, wind_xcomp), PredType::NATIVE_FLOAT);
	mtype2.insertMember("wind_ycomp", HOFFSET(WeatherData, wind_ycomp), PredType::NATIVE_FLOAT);
	mtype2.insertMember("hs", HOFFSET(WeatherData, hs), PredType::NATIVE_FLOAT);
	mtype2.insertMember("light", HOFFSET(WeatherData, light), PredType::NATIVE_CHAR);
	//WeatherData wd[106938134];

	try {
		weatherData = (WeatherData *)malloc(ds.getSimpleExtentNpoints() * sizeof(WeatherData));
		dataset.read(weatherData, mtype2);
		std::cout << "Finished" << std::endl;


		//size_t ix = i1*dims_out[1] * dims_out[2] + i2 * dims_out[2] + i3;
		//printf("%f %f %f %d\n", wd[ix].wind_xcomp, wd[ix].wind_ycomp, wd[ix].hs, wd[ix].light);
	}
	catch (int e)
	{
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
}


Weather::~Weather()
{
}


