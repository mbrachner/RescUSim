#pragma once
#include <string>
#include <tuple>
#include <memory>
#include <list>
#include "Weather.h"
#include "Position.h"

#define CONST_TYPE_HELICOPTER 1
#define CONST_TYPE_ERV 2



class RescueUnit {
public:
	RescueUnit();
	RescueUnit(const std::string &name);
	RescueUnit & setName(const std::string &name_);
	const std::string &getName();
	RescueUnit & setPos(float posX_, float posY_);
	RescueUnit & setSpeed(double speed);
	RescueUnit & setPickupTimeNormVisibility(double val);
	RescueUnit & setPickupTimeLowVisibility(double val);
	RescueUnit & setMobilizationTime(double val);
	RescueUnit & setMaxCapacity(unsigned int val);
	RescueUnit & setAvailability(double val);
	double getMobilizationTime();
	//virtual double getPickupTime(Position dest, size_t scenario, Weather weather)=0;
	double getPickupTimeNormVisibility();
	double getPickupTimeLowVisibility();
	double getSpeed();
	double getAvailability();
	virtual const unsigned int getType();
	bool isAvailable(double randomNumber);
	unsigned int getMaxCapacity();
	const std::tuple<double, double> getPosTuple();
	Position getPos();

	//virtual double getTravelTimeTo(Position dest, size_t scenario, Weather weather)=0;
	//virtual double getCoordOverhead(int numHelis)=0;
	virtual bool isInterfering() = 0;
	//double getCapacityTo(Position dest, size_t scenario, Weather weather, double timelimit);


protected:
	const int DISTSTEP = 10000;
	void setType(unsigned int type);


private:
	unsigned int type = 0;
	std::string name;
	Position pos = { 0,0 };
	double speed;
	double pickupTimeNormVisibility;
	double pickupTimeLowVisibility;
	double mobilizationTime;
	double availability;
	unsigned int maxCapacity;
};

class Helicopter: public RescueUnit {
public:
	Helicopter(const std::string &name);
	double getTravelTimeTo(Position dest, size_t scenario, Weather weather);
	//double getCoordOverhead(int numHelis);
	double getPickupTime(Position dest, size_t scenario, Weather weather);
	bool isInterfering();
};

class ERV : public RescueUnit {
public:
	ERV(const std::string &name);
	double getTravelTimeTo(Position dest, size_t scenario, Weather weather);
	//double getCoordOverhead(int numHelis);
	double getPickupTime(Position dest, size_t scenario, Weather weather);
	bool isInterfering();
};

typedef std::vector<std::shared_ptr<RescueUnit>> RUList;