#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include "RescueUnits.h"
#include "Simulator.h"
#include "Weather.h"

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

	py::class_<Weather>(m, "Weather")
		.def("__init__", [](Weather &m, py::buffer wd, py::buffer wsp, py::buffer hs) {
			py::buffer_info infoWd = wd.request();
			py::buffer_info infoWsp = wsp.request();
			py::buffer_info infoHs = hs.request();

			new (&m) Weather((float *)infoWd.ptr, (float *)infoWsp.ptr, (float *)infoHs.ptr,
				infoWd.shape[0], infoWd.shape[1], infoWd.shape[2]);
		})
		.def("wdAt", &Weather::wdAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("wspAt", &Weather::wspAt, py::arg("scenario"), py::arg("x"), py::arg("y"))
		.def("hsAt", &Weather::hsAt, py::arg("scenario"), py::arg("x"), py::arg("y"));

	py::class_<Simulator>(m, "Simulator")
		.def(py::init<const Weather &>())
		.def("addStationaryRU", &Simulator::addStationaryRU)
		;
	return m.ptr();
}