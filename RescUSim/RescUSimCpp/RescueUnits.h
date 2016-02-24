#pragma once
#include <string>
#include <tuple>

class RescueUnit {
public:
	RescueUnit(const std::string &name);
	void setName(const std::string &name_);
	const std::string &getName();
	void setPos(double posX_, double posY_);
	const std::tuple<double, double> getPos();
	virtual void calculateDistanceMatrix() = 0;

private:
	std::string name;
	double posX, posY;
};

class Helicopter: public RescueUnit {
public:
	Helicopter(const std::string &name) : RescueUnit(name) { }
	void calculateDistanceMatrix();
};

class ERV : public RescueUnit {
public:
	ERV(const std::string &name) : RescueUnit(name) { }
	void calculateDistanceMatrix();


};