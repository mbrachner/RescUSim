#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>


class RescueUnit {
public:
	RescueUnit(const std::string &name) : name(name) { }
	void setName(const std::string &name_) { name = name_; }
	const std::string &getName() const { return name; }

private:
	std::string name;
};

namespace py = pybind11;

PYBIND11_PLUGIN(RescUSimCpp) {
	py::module m("RescUSimCpp", "pybind11 example plugin");
	py::class_<RescueUnit>(m, "RescueUnit")
		.def(py::init<const std::string &>())
		.def("setName", &RescueUnit::setName)
		.def("getName", &RescueUnit::getName);

	return m.ptr();
}