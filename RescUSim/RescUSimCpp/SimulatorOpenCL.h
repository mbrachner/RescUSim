#pragma once
#include <CL/cl.hpp>
#include "Simulator.h"

class SimulatorOpenCL :
	public Simulator
{
public:
	SimulatorOpenCL(Weather weather);
	void addRU(std::shared_ptr<RescueUnit> ru);
	void transferWeather();
	void transferPOIs();
	void uploadKernel();
	void execute(std::shared_ptr<RescueUnit> ru);
	~SimulatorOpenCL();
private:
	cl::Device default_device;
	cl::Context context;
	cl::CommandQueue queue;
	float *res;
	cl::Buffer buffer_POIs;
	cl::Buffer buffer_res;
	cl::Buffer buffer_Weather;
	cl::Kernel kernel_add;
};

