#pragma once
#include <CL/cl.hpp>
#include "Simulator.h"

class SimulatorOpenCL :
	public Simulator
{
public:
	SimulatorOpenCL(std::shared_ptr<Weather> weather);
	void addStationaryRU(std::shared_ptr<Helicopter> ru);
	void addStationaryRU(std::shared_ptr<ERV> ru);
	void addRU(std::shared_ptr<Helicopter> ru);
	void addRU(std::shared_ptr<ERV> ru);
	void transferWeather();
	void transferPOIs();
	cl::Kernel uploadKernel(std::string filename, std::string kernelname);
	void execute(std::shared_ptr<RescueUnit> ru);
	void simulateResponseSample(double *res);
	~SimulatorOpenCL();
private:
	cl::Device default_device;
	cl::Context context;
	cl::CommandQueue queue;
	float *res;
	cl::Buffer buffer_POIs;
	cl::Buffer buffer_res;
	cl::Buffer buffer_Weather;
	//cl::Image3D img_Weather;
	cl::Kernel kernel_add;
};

struct SampleStructOpenCL {
	cl_double3 sample;
};

typedef struct {
	cl_double2 pos;
	cl_double speed;
	cl_double mobilizationTime;
	cl_uint type;
	cl_uint maxCapacity;
	cl_ulong dummy1;
} RescueUnitStruct;

typedef struct {
	cl_float2 wind;
	cl_float hs;
	cl_uint light;
} WeatherStructOpenCL;