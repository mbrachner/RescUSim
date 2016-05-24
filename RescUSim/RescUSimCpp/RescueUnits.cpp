
#define _USE_MATH_DEFINES

#include "RescueUnits.h"
#include <algorithm>
#include <math.h>
#include "Bounds.h"

RescueUnit::RescueUnit()
{
	data.name = "RU";
	data.pos = { 0,0 };
	data.speed = 0;
	data.pickupTimeLowVisibility = 0;
	data.pickupTimeNormVisibility = 0;
	data.maxCapacity = std::numeric_limits<unsigned int>::max();
}

RescueUnit::RescueUnit(const std::string & name) 
{
	data.name = name;
	data.pos = { 0,0 };
	data.speed = 0;
	data.pickupTimeLowVisibility = 0;
	data.pickupTimeNormVisibility = 0;
	data.maxCapacity = std::numeric_limits<unsigned int>::max();
}

RescueUnit & RescueUnit::setName(const std::string & name_) { data.name = name_; return *this; }

const std::string & RescueUnit::getName() { return data.name; }

RescueUnit & RescueUnit::setPos(float posX_, float posY_) {
	data.pos = { posX_, posY_ }; return *this;
}

RescueUnit & RescueUnit::setSpeed(double speed_) {
	data.speed = speed_; return *this;
}

RescueUnit & RescueUnit::setPickupTimeNormVisibility(double val) {
	data.pickupTimeNormVisibility = val; return *this;
}

RescueUnit & RescueUnit::setPickupTimeLowVisibility(double val)
{
	data.pickupTimeLowVisibility = val; return *this;
}

RescueUnit & RescueUnit::setMobilizationTime(double val)
{
	data.mobilizationTime = val;
	return *this;
}

RescueUnit & RescueUnit::setMaxCapacity(unsigned int val)
{
	data.maxCapacity = val;
	return *this;
}

RescueUnit & RescueUnit::setAvailability(double val)
{
	data.availability = val;
	return *this;
}

double RescueUnit::getMobilizationTime()
{
	return data.mobilizationTime;
}


double Helicopter::getPickupTime(Position pos, size_t scenario, Weather weather)
{
	size_t ix = (size_t)((pos.x - weather.getBounds().minx) / DISTSTEP);
	size_t iy = (size_t)((pos.y - weather.getBounds().miny) / DISTSTEP);


	if (weather.hsAt(scenario, ix, iy) < 6) {
		if (weather.lightAt(scenario, ix, iy))
			return 2;
		else
			return 3;
	}
	else {
		if (weather.lightAt(scenario, ix, iy))
			return 3;
		else 
			return 4;
	}
}

bool Helicopter::isInterfering()
{
	return true;
}


double ERV::getPickupTime(Position pos, size_t scenario, Weather weather)
{
	size_t ix = (size_t)((pos.x - weather.getBounds().minx) / DISTSTEP);
	size_t iy = (size_t)((pos.y - weather.getBounds().miny) / DISTSTEP);

	if (weather.hsAt(scenario, ix, iy) < 2) {
		if (weather.lightAt(scenario, ix, iy))
			return 1;
		else
			return 2;
	}
	else {
		if (weather.hsAt(scenario, ix, iy) < 4.5) {
			if (weather.lightAt(scenario, ix, iy))
				return 2;
			else
				return 3;
		}
		else {
			if (weather.hsAt(scenario, ix, iy) < 6) {
				if (weather.lightAt(scenario, ix, iy))
					return 4;
				else
					return 6;
			}
			else {
				return 200;
			}
		}
	}
}

bool ERV::isInterfering()
{
	return false;
}

double RescueUnit::getPickupTimeNormVisibility()
{
	return data.pickupTimeNormVisibility;
}

double RescueUnit::getPickupTimeLowVisibility()
{
	return data.pickupTimeLowVisibility;
}

double RescueUnit::getSpeed() {return data.speed;}

double RescueUnit::getAvailability()
{
	return data.availability;
}

bool RescueUnit::isAvailable(double randomNumber)
{
	return randomNumber < getAvailability();
}

unsigned int RescueUnit::getMaxCapacity()
{
	return data.maxCapacity;
}

const std::tuple<double, double> RescueUnit::getPosTuple()
{
	return std::make_tuple(data.pos.x, data.pos.y);
}

Position RescueUnit::getPos()
{
	return data.pos;
}

double RescueUnit::getCapacityTo(Position dest, size_t scenario, Weather weather, double timelimit)
{
	double travelTime = getTravelTimeTo(dest, scenario, weather);
	double c = (timelimit - getMobilizationTime() - travelTime) / getPickupTime(dest,scenario,weather);
	c = std::min(std::max(c, 0.), (double)getMaxCapacity());
	return c;
}

void RescueUnit::setType(unsigned int type)
{
	data.type = type;
}



Helicopter::Helicopter(const std::string & name):RescueUnit(name)
{
	setType(CONST_TYPE_HELICOPTER);
	setPickupTimeNormVisibility(3);
	setPickupTimeLowVisibility(4);
	setMobilizationTime(15);
	setAvailability(1.);
	setSpeed(72.2222222);
}

//#pragma optimize("",off)
double Helicopter::getTravelTimeTo(Position dest, size_t scenario, Weather weather)
{
	Position start = getPos();
	double t = 0;

	double crs = atan2(dest.y - start.y, dest.x - start.x);
	float dist = sqrt(pow(dest.x - start.x, 2) + pow(dest.y - start.y, 2));

	for (int step = 0; step <= (int)dist/DISTSTEP; step++) {
		float distStep = (std::min)((float)DISTSTEP, dist - step * DISTSTEP);

		Position act = {
			start.x + step * DISTSTEP * (float)cos(crs),
			start.y + step * DISTSTEP * (float)sin(crs)
		};


		size_t ix = (size_t)((act.x-weather.getBounds().minx) / DISTSTEP);
		size_t iy = (size_t)((act.y-weather.getBounds().miny) / DISTSTEP);

		double wsp = weather.wspAt(scenario, ix, iy);
		double wd = weather.wdAt(scenario, ix, iy);

		double m = wd - fmod(M_PI_2 - crs, 2 * M_PI);
		double gs = getSpeed()*sqrt(1 - pow(wsp, 2) / pow(getSpeed(), 2) * pow(sin(m), 2)) - wsp*cos(m);
		t += distStep / (gs * 60);
		
		if (t > 120) {
			t = std::numeric_limits<double>::max();
			break;
		}
	}
	return t;
}

//#pragma optimize("",off)
ERV::ERV(const std::string & name) :RescueUnit(name)
{	
	setType(CONST_TYPE_ERV);
	setPickupTimeNormVisibility(5);
	setPickupTimeLowVisibility(5);
	setAvailability(1.);
	setMobilizationTime(5);
	//setSpeed(72.2222222);
	setSpeed(10.2889);
	//setSpeed(3.0866);
	//setMaxCapacity(6);
}

double ERV::getTravelTimeTo(Position dest, size_t scenario, Weather weather)
{
	Position start = getPos();
	double t = 0;

	double crs = atan2(dest.y - start.y, dest.x - start.x);
	double dist = sqrt(pow(dest.x - start.x, 2) + pow(dest.y - start.y, 2));
	for (int step = 0; step <= (int)dist / DISTSTEP; step++) {
		double distStep = std::min((double)DISTSTEP, dist - step * DISTSTEP);

		Position act = {
			start.x + step * DISTSTEP * (float)cos(crs),
			start.y + step * DISTSTEP * (float)sin(crs)
		};
		

		size_t ix = (size_t)((act.x - weather.getBounds().minx) / DISTSTEP);
		size_t iy = (size_t)((act.y - weather.getBounds().miny) / DISTSTEP);

		double hs = weather.hsAt(scenario, ix, iy);

		double coeff = (hs <= 3) ? (9.311444 - getSpeed()) / 3 : 
									(9.054222 - getSpeed()) / 5;


		double gs = getSpeed()+coeff*hs;
		t += distStep / (gs * 60);

		if (t > 120) {
			t = std::numeric_limits<double>::max();
			break;
		}
	}
	return t;
}

double Helicopter::getCoordOverhead(int interferingUnits)
{
	return (interferingUnits==0) ? 1 : 2;
}

double ERV::getCoordOverhead(int interferingUnits)
{
	return 1;
}