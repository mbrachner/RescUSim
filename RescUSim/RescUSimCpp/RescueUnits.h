#pragma once
#include <string>
#include <tuple>
#include <memory>
#include "Weather.h"
#include "Position.h"

class RescueUnit {
public:
	RescueUnit();
	RescueUnit(const std::string &name);
	RescueUnit & setName(const std::string &name_);
	const std::string &getName();
	RescueUnit & setPos(double posX_, double posY_);
	RescueUnit & setSpeed(double speed);
	RescueUnit & setPickupTime(double val);
	RescueUnit & setMobilizationTime(double val);
	RescueUnit & setMaxCapacity(unsigned int val);
	double getMobilizationTime();
	double getPickupTime();
	double getSpeed();
	unsigned int getMaxCapacity();
	const std::tuple<double, double> getPosTuple();
	Position getPos();

	virtual double getTravelTimeTo(Position dest, size_t scenario, Weather weather)=0;
	double getCapacityTo(Position dest, size_t scenario, Weather weather, double timelimit);


protected:
	const int DISTSTEP = 10000;


private:
	std::string name;
	Position pos;
	double speed;
	double pickupTime;
	double mobilizationTime;
	unsigned int maxCapacity;
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