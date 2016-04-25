
#define _USE_MATH_DEFINES

#include "RescueUnits.h"
#include <algorithm>
#include <math.h>
#include "Bounds.h"

RescueUnit::RescueUnit() :name("RU"), pos({ 0,0 }), speed(0), pickupTimeNormVisibility(0), mobilizationTime(0), maxCapacity(std::numeric_limits<unsigned int>::max())
{
}

RescueUnit::RescueUnit(const std::string & name) : name(name), pos({ 0,0 }), speed(0), pickupTimeNormVisibility(0), pickupTimeLowVisibility(0), mobilizationTime(0), maxCapacity(std::numeric_limits<unsigned int>::max())
{
}

RescueUnit & RescueUnit::setName(const std::string & name_) { name = name_; return *this; }

const std::string & RescueUnit::getName() { return name; }

RescueUnit & RescueUnit::setPos(double posX_, double posY_) {
	pos = { posX_, posY_ }; return *this;
}

RescueUnit & RescueUnit::setSpeed(double speed_) {
	speed = speed_; return *this;
}

RescueUnit & RescueUnit::setPickupTimeNormVisibility(double val) {
	pickupTimeNormVisibility = val; return *this;
}

RescueUnit & RescueUnit::setPickupTimeLowVisibility(double val)
{
	pickupTimeLowVisibility = val; return *this;
}

RescueUnit & RescueUnit::setMobilizationTime(double val)
{
	mobilizationTime = val;
	return *this;
}

RescueUnit & RescueUnit::setMaxCapacity(unsigned int val)
{
	maxCapacity = val;
	return *this;
}

RescueUnit & RescueUnit::setAvailability(double val)
{
	availability = val;
	return *this;
}

double RescueUnit::getMobilizationTime()
{
	return mobilizationTime;
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
	return pickupTimeNormVisibility;
}

double RescueUnit::getPickupTimeLowVisibility()
{
	return pickupTimeLowVisibility;
}

double RescueUnit::getSpeed() {return speed;}

double RescueUnit::getAvailability()
{
	return availability;
}

bool RescueUnit::isAvailable(double randomNumber)
{
	return randomNumber < getAvailability();
}

unsigned int RescueUnit::getMaxCapacity()
{
	return maxCapacity;
}

const std::tuple<double, double> RescueUnit::getPosTuple()
{
	return std::make_tuple(pos.x, pos.y);
}

Position RescueUnit::getPos()
{
	return pos;
}

double RescueUnit::getCapacityTo(Position dest, size_t scenario, Weather weather, double timelimit)
{
	double travelTime = getTravelTimeTo(dest, scenario, weather);
	double c = (timelimit - getMobilizationTime() - travelTime) / getPickupTime(dest,scenario,weather);
	c = std::min(std::max(c, 0.), (double)getMaxCapacity());
	return c;
}



Helicopter::Helicopter(const std::string & name):RescueUnit(name)
{
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
	double dist = sqrt(pow(dest.x - start.x, 2) + pow(dest.y - start.y, 2));

	for (int step = 0; step <= (int)dist/DISTSTEP; step++) {
		double distStep = std::min((double)DISTSTEP, dist - step * DISTSTEP);

		Position act = {
			start.x + step * DISTSTEP * cos(crs),
			start.y + step * DISTSTEP * sin(crs)
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
	setPickupTimeNormVisibility(5);
	setPickupTimeLowVisibility(5);
	setAvailability(1.);
	setMobilizationTime(5);
	//setSpeed(10.2889);
	setSpeed(3.0866);
	setMaxCapacity(6);
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
			start.x + step * DISTSTEP * cos(crs),
			start.y + step * DISTSTEP * sin(crs)
		};
		

		size_t ix = (size_t)((act.x - weather.getBounds().minx) / DISTSTEP);
		size_t iy = (size_t)((act.y - weather.getBounds().miny) / DISTSTEP);

		double hs = weather.hsAt(scenario, ix, iy);

		double coeff = (hs <= 3) ? (9.311444 - getSpeed()) / 3 : 
									(9.054222 - getSpeed()) / 5;


		double gs = getSpeed(); // +coeff*hs;
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