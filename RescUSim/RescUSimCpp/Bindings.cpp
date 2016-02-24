#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "RescueUnits.h"
#include "Simulator.h"

namespace py = pybind11;

PYBIND11_PLUGIN(RescUSimCpp) {
	py::module m("RescUSimCpp");
	py::class_<RescueUnit> rescueUnit(m, "RescueUnit");
	rescueUnit
		.def(py::init<const std::string &>())
		.def("setName", &RescueUnit::setName)
		.def("getName", &RescueUnit::getName);
	py::class_<Helicopter>(m, "Helicopter", rescueUnit)
		.def(py::init<const std::string &>());
	py::class_<ERV>(m, "ERV", rescueUnit)
		.def(py::init<const std::string &>());

	py::class_<Simulator>(m, "Simulator")
		.def(py::init())
		.def("addStationaryRU", &Simulator::addStationaryRU);
	return m.ptr();
}