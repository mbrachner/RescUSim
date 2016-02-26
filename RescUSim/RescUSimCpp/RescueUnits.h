#pragma once
#include <string>
#include <tuple>
#include <memory>
#include "Weather.h"
#include "common.h"

class RescueUnit {
public:
	RescueUnit(const std::string &name);
	void setName(const std::string &name_);
	const std::string &getName();
	void setPos(double posX_, double posY_);
	void setSpeed(double speed);
	void setPickupTime(double val);
	double getPickupTime();
	double getSpeed();
	const std::tuple<double, double> getPosTuple();
	const Position getPos();
	virtual double getTravelTimeTo(Position point, size_t scenario, Weather weather)=0;


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
	Helicopter(const std::string &name) : RescueUnit(name) { 
		setPickupTime (3);
	}
	virtual double getTravelTimeTo(Position point, size_t scenario, Weather weather);
};

class ERV : public RescueUnit {
public:
	ERV(const std::string &name) : RescueUnit(name) {
		setPickupTime(5);
	}
	virtual double getTravelTimeTo(Position point, size_t scenario, Weather weather);


};