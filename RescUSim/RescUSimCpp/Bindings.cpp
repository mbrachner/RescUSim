#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include "RescueUnits.h"
#include "SimulatorCPU.h"
#include "SimulatorOpenCL.h"
#include "Weather.h"
#include "PointWeather.h"
#include "Position.h"
#include "Bounds.h"

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

PYBIND11_PLUGIN(RescUSimCpp) {
	py::module m("RescUSimCpp");
	py::class_<RescueUnit, std::shared_ptr<RescueUnit>> rescueUnit(m, "RescueUnit");
	rescueUnit
		.def("setName", &RescueUnit::setName)
		.def("getName", &RescueUnit::getName)
		.def("setPos", &RescueUnit::setPos)
		.def("getPos", &RescueUnit::getPos)
		.def("getPosTuple", &RescueUnit::getPosTuple)
		.def("setSpeed", &RescueUnit::setSpeed)
		.def("getSpeed", &RescueUnit::getSpeed)
		.def("setMaxCapacity", &RescueUnit::setMaxCapacity)
		.def("getMaxCapacity", &RescueUnit::getMaxCapacity)
		.def("setMobilizationTime", &RescueUnit::setMobilizationTime)
		.def("getMobilizationTime", &RescueUnit::getMobilizationTime)
		;
	py::class_<Helicopter, std::shared_ptr<Helicopter>>(m, "Helicopter", rescueUnit)
		.def(py::init<const std::string &>());
	py::class_<ERV, std::shared_ptr<ERV>>(m, "ERV", rescueUnit)
		.def(py::init<const std::string &>());
	
	py::class_<Bounds, std::shared_ptr<Bounds>>(m, "Bounds")
		.def(py::init<const double, const double, const double, const double>());

	py::class_<Weather, std::shared_ptr<Weather>>(m, "Weather")
		.def("__init__", [](Weather &m, py::buffer wdata, double minx, double miny, unsigned int resolution) {
			py::buffer_info infoWdata = wdata.request();
			std::cout << "Bind Itemsize: " << infoWdata.itemsize << std::endl;
			std::cout << "Bind Format: " << infoWdata.format << std::endl;
			Bounds bounds = { minx, minx + infoWdata.shape[1] * resolution, miny, miny + infoWdata.shape[2] * resolution };
			new (&m) Weather((WeatherData *)infoWdata.ptr,
				infoWdata.shape[0], infoWdata.shape[1], infoWdata.shape[2], resolution, bounds);
		})
		.def("windAt", &Weather::windAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("wdAt", &Weather::wdAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("wspAt", &Weather::wspAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("hsAt", &Weather::hsAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("lightAt", &Weather::lightAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("getNumScenarios", &Weather::getNumScenarios)
		;


	py::class_<Simulator>(m, "Simulator")
		.def("sample", &Simulator::sample)
		.def("addRU", (void (Simulator::*)(std::shared_ptr<Helicopter>)) &Simulator::addRU)
		.def("addRU", (void (Simulator::*)(std::shared_ptr<ERV>)) &Simulator::addRU)
		///.def("addStationaryRU", &Simulator::addStationaryRU)
		.def("addStationaryRU", (void (Simulator::*)(std::shared_ptr<Helicopter>)) &Simulator::addStationaryRU)
		.def("addStationaryRU", (void (Simulator::*)(std::shared_ptr<ERV>))&Simulator::addStationaryRU)

		.def("simulateResponseSample", [](Simulator &sim) {

			auto resArr = py::array(py::buffer_info(
				nullptr,
				sizeof(double),
				py::format_descriptor<double>::value(),
				1,
				{ sim.getSampleSize() },
				{ sizeof(double) }
			));

			auto bufRes = resArr.request();
			
			sim.simulateResponseSample((double *)bufRes.ptr);
			
			return resArr;
		})
		.def("getResCap", [](Simulator &sim) {
			auto result = py::array(py::buffer_info(
				sim.getResCap(),            /* Pointer to data (nullptr -> ask NumPy to allocate!) */
				sizeof(double),     /* Size of one item */
				py::format_descriptor<double>::value(), /* Buffer format */
				2,          /* How many dimensions? */
				{ sim.getNumPois(), sim.getWeather()->getNumScenarios() },  /* Number of elements for each dimension */
				{ sim.getWeather()->getNumScenarios()*sizeof(double), sizeof(double) }  /* Strides for each dimension */
			));
			return result;
		})
		;
		

	py::class_<SimulatorCPU>(m, "SimulatorCPU", py::base<Simulator>())
		.def(py::init<std::shared_ptr<Weather>>())

		.def("simulateTravel", &SimulatorCPU::simulateTravel)
		.def("simulateResponse", &SimulatorCPU::simulateResponse)
		//.def("addStationaryRU", (void (SimulatorCPU::*)(std::shared_ptr<Helicopter>)) &Simulator::addStationaryRU)
		//.def("addStationaryRU", (void (SimulatorCPU::*)(std::shared_ptr<ERV>))&Simulator::addStationaryRU)
		.def("addTemporaryRU", &SimulatorCPU::addTemporaryRU)
		.def("removeRU", &SimulatorCPU::removeRU)
		.def("addPoi", [] (SimulatorCPU &sim, py::buffer poiList){
			py::buffer_info infoPoiList = poiList.request();
			size_t rowStride = infoPoiList.strides[0] / infoPoiList.itemsize;
			size_t colStride = infoPoiList.strides[1] / infoPoiList.itemsize;
			for (size_t i = 0; i < infoPoiList.shape[0]; i++) {

				float px = *((float *)infoPoiList.ptr + i * rowStride);
				float py = *((float *)infoPoiList.ptr + i * rowStride + colStride);
				sim.addPoi(Position(px,py));
			}
			sim.initResTim();
		});

		py::class_<SimulatorOpenCL>(m, "SimulatorOpenCL", py::base<Simulator>())
			.def(py::init<std::shared_ptr<Weather>>())

			.def("simulateTravel", &SimulatorOpenCL::simulateTravel)
			.def("simulateResponse", &Simulator::simulateResponse)
			.def("getResCap", [](SimulatorOpenCL &sim) {
			//std::cout << (*sim.getPois()).size << std::endl;
			auto result = py::array(py::buffer_info(
				sim.getResCap(),            /* Pointer to data (nullptr -> ask NumPy to allocate!) */
				sizeof(double),     /* Size of one item */
				py::format_descriptor<double>::value(), /* Buffer format */
				2,          /* How many dimensions? */
				{ sim.getNumPois(), sim.getWeather()->getNumScenarios() },  /* Number of elements for each dimension */
				{ sim.getWeather()->getNumScenarios() * sizeof(double), sizeof(double) }  /* Strides for each dimension */
			));
			return result;
		})
			.def("addTemporaryRU", &SimulatorOpenCL::addTemporaryRU)
			.def("removeRU", &SimulatorOpenCL::removeRU)
			.def("addPoi", [](SimulatorOpenCL &sim, py::buffer poiList) {
			py::buffer_info infoPoiList = poiList.request();
			size_t rowStride = infoPoiList.strides[0] / infoPoiList.itemsize;
			size_t colStride = infoPoiList.strides[1] / infoPoiList.itemsize;
			for (size_t i = 0; i < infoPoiList.shape[0]; i++) {

				float px = *((float *)infoPoiList.ptr + i * rowStride);
				float py = *((float *)infoPoiList.ptr + i * rowStride + colStride);
				sim.addPoi(Position(px, py));
			}
			sim.initResTim();
			//sim.transferPOIs();
		})
			;
	return m.ptr();
}