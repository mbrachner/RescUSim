#pragma once
#include <string>
#include <tuple>
#include <memory>
#include "Weather.h"
#include "common.h"

class RescueUnit {
public:
	RescueUnit();
	RescueUnit(const std::string &name);
	RescueUnit & setName(const std::string &name_);
	const std::string &getName();
	RescueUnit & setPos(double posX_, double posY_);
	RescueUnit & setSpeed(double speed);
	RescueUnit & setPickupTime(double val);
	double getPickupTime();
	double getSpeed();
	const std::tuple<double, double> getPosTuple();
	const Position getPos();
	virtual double getTravelTimeTo(Position dest, size_t scenario, Weather weather)=0;


protected:
	const int DISTSTEP = 10000;


private:
	std::string name;
	Position pos;
	double speed;
	double pickuptime;
};

class Helicopter: public RescueUnit {
public:
	Helicopter(const std::string &name);
	double getTravelTimeTo(Position dest, size_t scenario, Weather weather);
};

class ERV : public RescueUnit {
public:
	ERV(const std::string &name);
	double getTravelTimeTo(Position dest, size_t scenario, Weather weather);

};