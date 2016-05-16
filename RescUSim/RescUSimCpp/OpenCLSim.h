#pragma once
//#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.hpp>
#include "Weather.h"
//#include "Simulator.h"
#include "Position.h"
#include <memory>
#include "RescueUnits.h"

class OpenCLSim {
public:
	OpenCLSim();
	cl::Context getContext();
	void uploadKernel();
	void transferWeather(Weather *_weather);
	void transferPOIs(PositionList *_pois);
	void execute(std::shared_ptr<RescueUnit> ru);
	float *getRes();
	~OpenCLSim();
private:
	Weather *weather;
	PositionList *pois;
	cl::Context context;
	cl::Buffer buffer_Weather;
	cl::Buffer buffer_POIs;
	cl::Buffer buffer_res;
	cl::CommandQueue queue;
	cl::Device default_device;
	cl::Kernel kernel_add;
	float *res;
};