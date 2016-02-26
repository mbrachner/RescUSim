#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include "RescueUnits.h"
#include "Simulator.h"
#include "Weather.h"
#include "common.h"

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
		;
	py::class_<Helicopter, std::shared_ptr<Helicopter>>(m, "Helicopter", rescueUnit)
		.def(py::init<const std::string &>());
	py::class_<ERV, std::shared_ptr<ERV>>(m, "ERV", rescueUnit)
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
		.def("simulate", &Simulator::simulate)
		.def("addStationaryRU", &Simulator::addStationaryRU)
		.def("addPoi", [] (Simulator &sim, py::buffer poiList){
			py::buffer_info infoPoiList = poiList.request();
			size_t rowStride = infoPoiList.strides[0] / infoPoiList.itemsize;
			size_t colStride = infoPoiList.strides[1] / infoPoiList.itemsize;
			for (size_t i = 0; i < infoPoiList.shape[0]; i++) {

				double px = *((double *)infoPoiList.ptr + i * rowStride);
				double py = *((double *)infoPoiList.ptr + i * rowStride + colStride);
				sim.addPoi(Position(px,py));
			}
		})
		;
	return m.ptr();
}