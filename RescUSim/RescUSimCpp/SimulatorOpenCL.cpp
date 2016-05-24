#include "SimulatorOpenCL.h"
#include <iostream>
#include <fstream>

inline void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

SimulatorOpenCL::SimulatorOpenCL(Weather weather) : Simulator(weather)
{
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
	context = cl::Context(default_device, 0, 0, 0, &err);
	checkErr(err, "Context::Context()");
	queue = cl::CommandQueue(context, default_device, 0, &err);
	checkErr(err, "Queue::Queue()");

	transferWeather();
	uploadKernel();
}

void SimulatorOpenCL::addRU(std::shared_ptr<Helicopter> ru)
{
	execute(ru);
}

void SimulatorOpenCL::addRU(std::shared_ptr<ERV> ru)
{
	execute(ru);
}

void SimulatorOpenCL::transferWeather() {
	std::cout << (weather.getMemSize() / (1024 * 1024)) << " MB" << std::endl;
	cl_int err;
	buffer_Weather = cl::Buffer(context, CL_MEM_READ_WRITE, weather.getMemSize(), 0, &err);
	checkErr(err, "Buffer::Buffer()");
	queue.enqueueWriteBuffer(buffer_Weather, CL_TRUE, 0, weather.getMemSize(), weather.getWeatherDataPtr());
	//cl::size_t<3> origin;
	//cl::size_t<3> region;
	//origin[0] = weather.getBounds().minx;
	//origin[1] = weather.getBounds().miny;
	//origin[2] = 0;
	//img_Weather = cl::Image3D(context, CL_MEM_READ_ONLY, cl_image_format({ CL_RGBA,CL_FLOAT }),
	//	weather.getMemSize(), 10, 2920, 0Ui64, 0Ui64, NULL, &err);

	//queue.enqueueWriteImage(img_Weather, CL_TRUE, origin, region,0,0, weather.getWeatherDataPtr());
	queue.finish();
}

void SimulatorOpenCL::transferPOIs() {
	cl_int err;
	std::cout << sizeof(Position)*pois.size() / (1024) << " kB" << std::endl;
	buffer_POIs = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Position)*pois.size(), 0, &err);
	checkErr(err, "buffer_POIs Buffer:Buffer()");
	queue.enqueueWriteBuffer(buffer_POIs, CL_TRUE, 0, sizeof(Position)*pois.size(), pois.data());
	buffer_res = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*pois.size()*weather.getNumScenarios(), 0, &err);
	checkErr(err, "buffer_res Buffer:Buffer()");
	res = (float *)malloc(sizeof(float)*pois.size()*weather.getNumScenarios());
	queue.finish();
}

void SimulatorOpenCL::uploadKernel() {
	cl::Program::Sources sources;
	std::ifstream infile{ "C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\RescUSimCpp\\kernel.cl" };
	std::string kernel_code{ std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl_int err;
	cl::Program program = cl::Program(context, sources, &err);
	checkErr(err, "Program::Program()");
	if (program.build({ default_device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}
	kernel_add = cl::Kernel(program, "transitTimeHelicopter", &err);
	checkErr(err, "Kernel::Kernel()");

}

void SimulatorOpenCL::execute(std::shared_ptr<RescueUnit> ru) {
	int argC = 0;
	cl_int err;
	kernel_add.setArg(argC++, buffer_Weather);
	kernel_add.setArg(argC++, (unsigned int)weather.getDimX());
	kernel_add.setArg(argC++, (unsigned int)weather.getDimY());
	kernel_add.setArg(argC++, buffer_POIs);
	kernel_add.setArg(argC++, buffer_res);
	kernel_add.setArg(argC++, (float)weather.getBounds().minx);
	kernel_add.setArg(argC++, (float)weather.getBounds().miny);
	kernel_add.setArg(argC++, (float)ru->getPos().x);
	kernel_add.setArg(argC++, (float)ru->getPos().y);
	kernel_add.setArg(argC++, (float)ru->getMobilizationTime());
	kernel_add.setArg(argC++, (float)ru->getSpeed());

	err = queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(weather.getNumScenarios(), pois.size()), cl::NullRange);
	//err = queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(3, pois.size()), cl::NullRange);
	checkErr(err, "queue.enqueueNDRangeKernel()");
	err = queue.enqueueReadBuffer(buffer_res, CL_TRUE, 0, sizeof(float)*pois.size()*weather.getNumScenarios(), res);
	checkErr(err, "queue.enqueueReadBuffer()");
	queue.finish();

#pragma omp parallel for schedule(dynamic) num_threads(4)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {

		/*if (!(scenario % 500)) {
			std::cout << "Scenario " << scenario << std::endl;
		}*/
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {

			if (weather.getBounds().within(ru->getPos())) {
				double t = res[scenario*pois.size()+i];

				if (t <= 120) {

					RUTime h;
					h.t = t;
					h.ru = ru;
					resTim[scenario][i].push_back(h);
				}
			}

		}
	}

}

SimulatorOpenCL::~SimulatorOpenCL()
{
}