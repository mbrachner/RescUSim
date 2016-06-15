#include "SimulatorOpenCL.h"
#include <iostream>
#include <fstream>
#include <chrono>

inline void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

SimulatorOpenCL::SimulatorOpenCL(std::shared_ptr<Weather> weather) : Simulator(weather)
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

	cl::Platform default_platform = all_platforms[1];
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

	//transferWeather();
	//uploadKernel("C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\RescUSimCpp\\kernel.cl", "transitTimeHelicopter");
}

void SimulatorOpenCL::addStationaryRU(std::shared_ptr<Helicopter> ru)
{
	std::cout << "Adding Helicopter" << std::endl;
	Simulator::addStationaryRU(ru);

}

void SimulatorOpenCL::addStationaryRU(std::shared_ptr<ERV> ru)
{
	std::cout << "Adding ERV" << std::endl;
	Simulator::addStationaryRU(ru);
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
	std::cout << (weather->getMemSize() / (1024 * 1024)) << " MB" << std::endl;
	cl_int err;
	buffer_Weather = cl::Buffer(context, CL_MEM_READ_WRITE, weather->getMemSize(), 0, &err);
	checkErr(err, "Buffer::Buffer()");
	queue.enqueueWriteBuffer(buffer_Weather, CL_TRUE, 0, weather->getMemSize(), weather->getWeatherDataPtr());
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
	std::cout << sizeof(Position)*pois.size() << " kB" << std::endl;
	buffer_POIs = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Position)*pois.size(), 0, &err);
	checkErr(err, "buffer_POIs Buffer:Buffer()");
	queue.enqueueWriteBuffer(buffer_POIs, CL_TRUE, 0, sizeof(Position)*pois.size(), pois.data());
	buffer_res = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*pois.size()*weather->getNumScenarios(), 0, &err);
	checkErr(err, "buffer_res Buffer:Buffer()");
	res = (float *)malloc(sizeof(float)*pois.size()*weather->getNumScenarios());
	queue.finish();
}

cl::Kernel SimulatorOpenCL::uploadKernel(std::string filename, std::string kernelname) {
	cl::Program::Sources sources;
	std::ifstream infile{ filename };
	std::string kernel_code{ std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };
	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl_int err;
	cl::Program program = cl::Program(context, sources, &err);
	checkErr(err, "Program::Program()");
	if (program.build({ default_device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}
	cl::Kernel k = cl::Kernel(program, kernelname.c_str(), &err);
	checkErr(err, "Kernel::Kernel()");
	return k;
}

void SimulatorOpenCL::execute(std::shared_ptr<RescueUnit> ru) {
//	int argC = 0;
//	cl_int err;
//	kernel_add.setArg(argC++, buffer_Weather);
//	kernel_add.setArg(argC++, (unsigned int)weather.getDimX());
//	kernel_add.setArg(argC++, (unsigned int)weather.getDimY());
//	kernel_add.setArg(argC++, buffer_POIs);
//	kernel_add.setArg(argC++, buffer_res);
//	kernel_add.setArg(argC++, (float)weather.getBounds().minx);
//	kernel_add.setArg(argC++, (float)weather.getBounds().miny);
//	kernel_add.setArg(argC++, (float)ru->getPos().x);
//	kernel_add.setArg(argC++, (float)ru->getPos().y);
//	kernel_add.setArg(argC++, (float)ru->getMobilizationTime());
//	kernel_add.setArg(argC++, (float)ru->getSpeed());
//
//	err = queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(weather.getNumScenarios(), pois.size()), cl::NullRange);
//	//err = queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(3, pois.size()), cl::NullRange);
//	checkErr(err, "queue.enqueueNDRangeKernel()");
//	err = queue.enqueueReadBuffer(buffer_res, CL_TRUE, 0, sizeof(float)*pois.size()*weather.getNumScenarios(), res);
//	checkErr(err, "queue.enqueueReadBuffer()");
//	queue.finish();
//
//#pragma omp parallel for schedule(dynamic) num_threads(4)
//	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
//
//		/*if (!(scenario % 500)) {
//			std::cout << "Scenario " << scenario << std::endl;
//		}*/
//		int i = 0;
//		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
//
//			if (weather.getBounds().within(ru->getPos())) {
//				double t = res[scenario*pois.size()+i];
//
//				if (t <= 120) {
//
//					RUTime h;
//					h.t = t;
//					h.ru = ru;
//					resTim[scenario][i].push_back(h);
//				}
//			}
//
//		}
//	}

}


void SimulatorOpenCL::simulateResponseSample(double *res)
{

	std::vector<RescueUnitStruct> rm;
	for (RUList::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
		RescueUnitStruct rs;
		//std::cout << "RSSize: " << sizeof(rs) << std::endl;
		rs.type = (*ru)->getType();
		Position pos = (*ru)->getPos();
		rs.pos.x = pos.x;
		rs.pos.y = pos.y;
		rs.speed = (*ru)->getSpeed();
		rs.maxCapacity = (*ru)->getMaxCapacity();
		rs.mobilizationTime = (*ru)->getMobilizationTime();
		//printf("Type: %d  Speed: %f  Pos: (%f,%f) MobTime: %f\n", rs.type, rs.speed,
		//	rs.pos.x, rs.pos.y, rs.mobilizationTime);
		rm.push_back(rs);
	}
	std::vector<SampleStructOpenCL> oclSamples;
	for (int i = 0; i < samples.size(); i++) {
		Sample sample = samples[i];
		//std::printf("%f %f %f\n", sample.scenario, sample.poi->x, sample.poi->y);
		SampleStructOpenCL sstr = SampleStructOpenCL{ sample.poi->x, sample.poi->y, sample.scenario, 0 };
		//std::cout << sizeof(sstr) << std::endl;
		oclSamples.push_back(sstr);
	}
	cl::Kernel kernel = uploadKernel("C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\RescUSimCpp\\texture.cl", "simulateSample");

	int argC = 0;
	cl_int err;
	cl::Buffer buffer_Samples =
		cl::Buffer(context, CL_MEM_READ_ONLY, samples.size()*sizeof(SampleStructOpenCL), 0, &err);
	checkErr(err, "buffer_Samples Buffer:Buffer()");
	queue.enqueueWriteBuffer(buffer_Samples, CL_TRUE, 0, samples.size()*sizeof(SampleStructOpenCL), oclSamples.data());
	cl::Buffer buffer_RUs =
		cl::Buffer(context, CL_MEM_READ_ONLY, rm.size()*sizeof(RescueUnitStruct), 0, &err);
	checkErr(err, "buffer_RescueUnitStruct Buffer:Buffer()");
	queue.enqueueWriteBuffer(buffer_RUs, CL_TRUE, 0, rm.size()*sizeof(RescueUnitStruct), rm.data());

	cl_float *values = (cl_float *)malloc(4* weather->getDimX() *weather->getDimY() *2920*sizeof(cl_float));
	for (int x = 0; x < weather->getDimX(); x++) {
		for (int y = 0; y < weather->getDimY(); y++) {
			for (int z = 0; z < 2920; z++) {
				size_t ix = weather->getDimY()* x + y + weather->getDimX()*weather->getDimY()*z;
				//values[(z * 20 * 10 + y * 20 + x) * 4] = x + y + 100 * z;
				values[(z * 20 * 10 + y * 20 + x) * 4] = (weather->getWeatherDataPtr() + ix)->wind_xcomp;
				values[(z * 20 * 10 + y * 20 + x) * 4+1] = (weather->getWeatherDataPtr() + ix)->wind_ycomp;
				values[(z * 20 * 10 + y * 20 + x) * 4+2] = (weather->getWeatherDataPtr() + ix)->hs;
				values[(z * 20 * 10 + y * 20 + x) * 4+3] = (weather->getWeatherDataPtr() + ix)->light;
			}
		}
	}
	printf("Host WindAt %f,%f \n", (weather->getWeatherDataPtr())->wind_xcomp, (weather->getWeatherDataPtr())->wind_ycomp);
	printf("Host WindAt %f,%f \n", values[0], values[1]);
	cl::ImageFormat FormatA(CL_RGBA, CL_FLOAT);
	//cl::Image3D weatherTex(context, CL_MEM_READ_ONLY , FormatA, weather->getDimX(), weather->getDimY(), 2920, 0, 0, values, NULL);
	cl::Image3D weatherTex(context, CL_MEM_READ_ONLY , FormatA, weather->getDimX(), weather->getDimY(), 2920);
	
	cl::size_t<3> o;
	o[0] = 0;
	o[1] = 0;
	o[2] = 0;
	cl::size_t<3> r;
	r[0] = weather->getDimX();
	r[1] = weather->getDimY();
	r[2] = 2920;
	queue.enqueueWriteImage(weatherTex, CL_TRUE, o, r, 0, 0, values);
	queue.finish();

	//cl::Image3D weatherTex(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, cl::ImageFormat(CL_R, CL_FLOAT), weather->getDimX(), weather->getDimY(), 2, weather->getDimX(), weather->getDimX(), (void *)rm.data());
	//__debugbreak();
	//std::cout << weatherTex. << std::endl;
	//queue.enqueueWriteImage(weatherTex, CL_TRUE,)

	/*
	WeatherStructOpenCL *oclweather;
	std::printf("weathersize host: %zd \n", sizeof(WeatherStructOpenCL));
	size_t wsize = weather->getDimX()*weather->getDimY()*weather->getNumScenarios()*sizeof(WeatherStructOpenCL);
	oclweather = (WeatherStructOpenCL *)malloc(wsize);
	for (size_t i = 0; i < weather->getDimX()*weather->getDimY()*weather->getNumScenarios(); i++) {
		WeatherStructOpenCL w = WeatherStructOpenCL();
		w.wind.x = (weather->getWeatherDataPtr() + i)->wind_xcomp;
		w.wind.y = (weather->getWeatherDataPtr() + i)->wind_ycomp;
		w.hs = (weather->getWeatherDataPtr()+i)->hs;
		w.light = (weather->getWeatherDataPtr()+i)->light;
		oclweather[i] = w;
	}

	buffer_Weather = cl::Buffer(context, CL_MEM_READ_WRITE, wsize, 0, &err);
	checkErr(err, "Buffer::Weather()");
	queue.enqueueWriteBuffer(buffer_Weather, CL_TRUE, 0, wsize, oclweather);
	*/
	//oclweather = (WeatherStructOpenCL *)malloc(wsize);
	/*for (size_t i = 0; i < weather->getDimX()*weather->getDimY()*2920; i++) {
		//WeatherStructOpenCL w = WeatherStructOpenCL();
		w.wind.x = (weather->getWeatherDataPtr() + i)->wind_xcomp;
		w.wind.y = (weather->getWeatherDataPtr() + i)->wind_ycomp;
		w.hs = (weather->getWeatherDataPtr() + i)->hs;
		w.light = (weather->getWeatherDataPtr() + i)->light;
		//oclweather[i] = w;
	}*/

	queue.finish();
	kernel.setArg(argC++, buffer_Samples);
	kernel.setArg(argC++, buffer_RUs);
	kernel.setArg(argC++, (cl_ushort)rm.size());
	//kernel.setArg(argC++, weatherTex);
	
	//kernel.setArg(argC++, buffer_Weather);
	kernel.setArg(argC++, weatherTex);
	cl::Sampler Samp;
	Samp() = clCreateSampler(context(), CL_FALSE, CL_ADDRESS_REPEAT, CL_FILTER_LINEAR, NULL);
	kernel.setArg(argC++, Samp);

	
	cl_uint2 t = { weather->getDimX(), weather->getDimY() };
	kernel.setArg(argC++, t );
	cl_double2 m = { weather->getBounds().minx, weather->getBounds().miny };
	kernel.setArg(argC++, m);
	kernel.setArg(argC++, weather->getResolution());
	Sample sample = samples[0];
	//BilinearWeights bw = weather->getBilinearWeights(sample.poi->x, sample.poi->y, sample.scenario);
	//printf("Point %f,%f BW %f,%f,%f,%f,%f,%f,%f,%f", sample.poi->x, sample.poi->y, bw.w1, bw.w2, bw.w3, bw.w4, bw.w5, bw.w6, bw.w7, bw.w8);
	std::tuple<double, double> wind = weather->windAt(0, weather->getBounds().minx, weather->getBounds().miny);
	printf("Host WindAt %f,%f \n", std::get<0>(wind), std::get<1>(wind));
	double hs = weather->hsAt(sample.scenario, sample.poi->x, sample.poi->y);
	//printf("Host HsAt %f\n", hs);
	
	auto start = std::chrono::steady_clock::now();
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(samples.size()), cl::NullRange);
	checkErr(err, "enqueueNDRangeKernel()");
	queue.finish();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Elapsed time is :  " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " mys " << std::endl;

}

SimulatorOpenCL::~SimulatorOpenCL()
{
}