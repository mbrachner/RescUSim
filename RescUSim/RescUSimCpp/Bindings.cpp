#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include "RescueUnits.h"
#include "Simulator.h"
#include "Weather.h"
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
		.def("__init__", [](Weather &m, py::buffer wdata, Bounds bounds) {
			py::buffer_info infoWdata = wdata.request();

			new (&m) Weather((WeatherData *)infoWdata.ptr,
				infoWdata.shape[0], infoWdata.shape[1], infoWdata.shape[2], bounds);
		})
		.def("wdAt", &Weather::wdAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("wspAt", &Weather::wspAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("hsAt", &Weather::hsAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("lightAt", &Weather::lightAt, py::arg("scenario"), py::arg("x"), py::arg("y"));

	py::class_<Simulator>(m, "Simulator")
		.def(py::init<const Weather &>())
		.def("simulateTravel", &Simulator::simulateTravel)
		.def("simulateResponse", &Simulator::simulateResponse)
		.def("getResCap", [](Simulator &sim) {
			//std::cout << (*sim.getPois()).size << std::endl;
			auto result = py::array(py::buffer_info(
				sim.getResCap(),            /* Pointer to data (nullptr -> ask NumPy to allocate!) */
				sizeof(double),     /* Size of one item */
				py::format_descriptor<double>::value(), /* Buffer format */
				2,          /* How many dimensions? */
				{ sim.getNumPois(), sim.getWeather().getNumScenarios() },  /* Number of elements for each dimension */
				{ sim.getWeather().getNumScenarios()*sizeof(double), sizeof(double) }  /* Strides for each dimension */
			));
			return result;
		})
		.def("addStationaryRU", &Simulator::addStationaryRU)
		.def("addTemporaryRU", &Simulator::addTemporaryRU)
		.def("addRU", &Simulator::addRU)
		.def("initOpenCL", &Simulator::initOpenCL)
		.def("addRUOpenCL", &Simulator::addRUOpenCL)
		.def("removeRU", &Simulator::removeRU)
		.def("addPoi", [] (Simulator &sim, py::buffer poiList){
			py::buffer_info infoPoiList = poiList.request();
			size_t rowStride = infoPoiList.strides[0] / infoPoiList.itemsize;
			size_t colStride = infoPoiList.strides[1] / infoPoiList.itemsize;
			for (size_t i = 0; i < infoPoiList.shape[0]; i++) {

				float px = *((float *)infoPoiList.ptr + i * rowStride);
				float py = *((float *)infoPoiList.ptr + i * rowStride + colStride);
				sim.addPoi(Position(px,py));
			}
			sim.initResTim();
		})
		;
	return m.ptr();
}