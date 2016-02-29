
#define _USE_MATH_DEFINES

#include "RescueUnits.h"
#include <algorithm>
#include <math.h>

RescueUnit::RescueUnit() :name("RU"), pos({ 0,0 }), speed(0), pickuptime(0)
{
}

RescueUnit::RescueUnit(const std::string & name) :name(name), pos({ 0,0 }), speed(0), pickuptime(0)
{
}

RescueUnit & RescueUnit::setName(const std::string & name_) { name = name_; return *this;  }

const std::string & RescueUnit::getName() { return name; }

RescueUnit & RescueUnit::setPos(double posX_, double posY_) { pos = { posX_, posY_ }; return *this;
}

RescueUnit & RescueUnit::setSpeed(double speed_) { speed = speed_; return *this;
}

RescueUnit & RescueUnit::setPickupTime(double val){	pickuptime = val; return *this;
}

double RescueUnit::getPickupTime()
{
	return pickuptime;
}

double RescueUnit::getSpeed() {return speed;}

const std::tuple<double, double> RescueUnit::getPosTuple()
{
	return std::make_tuple(pos.x, pos.y);
}

const Position RescueUnit::getPos()
{
	return pos;
}



Helicopter::Helicopter(const std::string & name):RescueUnit(name)
{
	setPickupTime(3);
}

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

		size_t ix = (size_t)(act.x / DISTSTEP);
		size_t iy = (size_t)(act.y / DISTSTEP);

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

ERV::ERV(const std::string & name) :RescueUnit(name)
{	
	setPickupTime(5);
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

		size_t ix = (size_t)(act.x / DISTSTEP);
		size_t iy = (size_t)(act.y / DISTSTEP);

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
