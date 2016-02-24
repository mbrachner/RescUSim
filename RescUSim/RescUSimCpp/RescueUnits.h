#pragma once
#include <string>

class RescueUnit {
public:
	RescueUnit(const std::string &name);
	void setName(const std::string &name_);
	const std::string &getName();

private:
	std::string name;
};

class Helicopter: public RescueUnit {
public:
	Helicopter(const std::string &name) : RescueUnit(name) { }
};

class ERV : public RescueUnit {
public:
	ERV(const std::string &name) : RescueUnit(name) { }


};