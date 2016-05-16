#include <iostream>
#include <fstream>
#include <vector>
#include "OpenCLSim.h"

inline void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

OpenCLSim::OpenCLSim() {
	std::vector<cl::Platform> all_platforms;

	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	for (std::vector<cl::Platform>::iterator pform = all_platforms.begin(); pform != all_platforms.end(); ++pform) {
		std::cout << "Platform: " << pform->getInfo<CL_PLATFORM_NAME>() << "\n";
	}
	cl_int err;

	/*cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(all_platforms[0])(), 0 };
	context = cl::Context(
		CL_DEVICE_TYPE_GPU,
		cprops,
		NULL,
		NULL,
		&err);
	checkErr(err, "Context::Context()");
	std::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");
	for (std::vector<cl::Device>::iterator dev = devices.begin(); dev != devices.end(); ++dev) {
		std::cout << "  Device: " << dev->getInfo<CL_DEVICE_NAME>() << "\n";
	}*/
	
	
	cl::Platform default_platform = all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
	context=cl::Context(default_device,0,0,0,&err);
	checkErr(err, "Context::Context()");
	queue = cl::CommandQueue(context, default_device,0,&err);
	checkErr(err, "Queue::Queue()");

	
}

cl::Context OpenCLSim::getContext()
{
	return context;
}

void OpenCLSim::uploadKernel() {
	cl::Program::Sources sources;
	std::ifstream infile{ "C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\RescUSimCpp\\kernel.cl" };
	std::string kernel_code{ std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };
	std::cout << kernel_code.c_str() << std::endl;

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });
	//__debugbreak();
	cl_int err;
	cl::Program program = cl::Program(context, sources, &err);
	checkErr(err, "Program::Program()");
	if (program.build({ default_device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}
	kernel_add = cl::Kernel(program, "simple_add",&err);
	checkErr(err, "Kernel::Kernel()");

}

void OpenCLSim::transferWeather(Weather *_weather) {
	weather = _weather;
	std::cout << (weather->getMemSize()/(1024*1024)) << " MB"<<std::endl;
	cl_int err;
	buffer_Weather = cl::Buffer(context, CL_MEM_READ_WRITE, weather->getMemSize(),0,&err);
	checkErr(err, "Buffer::Buffer()");
	queue.enqueueWriteBuffer(buffer_Weather, CL_TRUE, 0, weather->getMemSize(), weather->getWeatherDataPtr());
	queue.finish();
}

void OpenCLSim::transferPOIs(PositionList *_pois) {
	pois = _pois;
	cl_int err;
	std::cout << sizeof(Position)*pois->size()/(1024) << " kB" << std::endl;
	buffer_POIs = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Position)*pois->size(), 0, &err);
	checkErr(err, "buffer_POIs Buffer:Buffer()");
	queue.enqueueWriteBuffer(buffer_POIs, CL_TRUE, 0, sizeof(Position)*pois->size(), pois->data());
	buffer_res = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float)*pois->size()*weather->getNumScenarios(), 0, &err);
	res = (float *)malloc(sizeof(float)*pois->size()*weather->getNumScenarios());
}

void OpenCLSim::execute(std::shared_ptr<RescueUnit> ru) {
	int argC = 0;
	kernel_add.setArg(argC++, buffer_Weather);
	kernel_add.setArg(argC++, (unsigned int)weather->getDimX());
	kernel_add.setArg(argC++, (unsigned int)weather->getDimY());

	kernel_add.setArg(argC++, buffer_POIs);
	kernel_add.setArg(argC++, buffer_res);
	kernel_add.setArg(argC++, (float)weather->getBounds().minx);
	kernel_add.setArg(argC++, (float)weather->getBounds().miny);
	kernel_add.setArg(argC++, (float)ru->getPos().x);
	kernel_add.setArg(argC++, (float)ru->getPos().y);
	kernel_add.setArg(argC++, (float)ru->getMobilizationTime());
	kernel_add.setArg(argC++, (float)ru->getSpeed());

	queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(weather->getNumScenarios(), pois->size()), cl::NullRange);
	//queue.finish();
	queue.enqueueReadBuffer(buffer_res, CL_TRUE, 0, sizeof(Position)*pois->size()*weather->getNumScenarios(), res);
	queue.finish();
	std::cout << "Result: " << res[0];
}

float * OpenCLSim::getRes()
{
	return res;
}

OpenCLSim::~OpenCLSim()
{
}

