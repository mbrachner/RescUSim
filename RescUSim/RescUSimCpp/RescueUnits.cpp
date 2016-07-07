
#define _USE_MATH_DEFINES

#include "RescueUnits.h"
#include <algorithm>
#include <math.h>
#include "Bounds.h"

RescueUnit::RescueUnit()
{
	name = "RU";
	pos = { 0,0 };
	speed = 0;
	pickupTimeLowVisibility = 0;
	pickupTimeNormVisibility = 0;
	maxCapacity = std::numeric_limits<unsigned int>::max();
}

RescueUnit::RescueUnit(const std::string & _name, std::shared_ptr<Weather> _weather) 
{
	name = _name;
	pos = { 0,0 };
	speed = 0;
	pickupTimeLowVisibility = 0;
	pickupTimeNormVisibility = 0;
	maxCapacity = std::numeric_limits<unsigned int>::max();
	weather = _weather;
}

RescueUnit & RescueUnit::setName(const std::string & name_) { name = name_; return *this; }

const std::string & RescueUnit::getName() { return name; }

RescueUnit & RescueUnit::setPos(double posX_, double posY_) {
	if ((posX_ >= weather->getBounds().minx) && (posX_ <= weather->getBounds().maxx) &&
		(posY_ >= weather->getBounds().miny) && (posY_ <= weather->getBounds().maxy)) {
		setDirty();
		pos = { posX_, posY_ };
	}
	else {
		throw 100;
	}
	return *this;
}

RescueUnit & RescueUnit::setPosU(double posX_, double posY_)
{
	// TODO: insert return statement here
	Position p = getPos();
	double px = weather->getBounds().minx + posX_*(weather->getBounds().maxx - weather->getBounds().minx);
	double py = weather->getBounds().miny + posY_*(weather->getBounds().maxy - weather->getBounds().miny);
	setPos(px, py);
	return *this;
}

RescueUnit & RescueUnit::setSpeed(double speed_) {
	setDirty();
	speed = speed_; return *this;
}

RescueUnit & RescueUnit::setPickupTimeNormVisibility(double val) {
	setDirty();
	pickupTimeNormVisibility = val; return *this;
}

RescueUnit & RescueUnit::setPickupTimeLowVisibility(double val)
{
	setDirty();
	pickupTimeLowVisibility = val; return *this;
}

RescueUnit & RescueUnit::setMobilizationTime(double val)
{
	setDirty();
	mobilizationTime = val;
	return *this;
}

RescueUnit & RescueUnit::setMaxCapacity(unsigned int val)
{
	setDirty();
	maxCapacity = val;
	return *this;
}

RescueUnit & RescueUnit::setAvailability(double val)
{
	setDirty();
	availability = val;
	return *this;
}

double RescueUnit::getMobilizationTime()
{
	return mobilizationTime;
}




bool Helicopter::isInterfering()
{
	return true;
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

void RescueUnit::setDirty()
{
	dirty = true;
}

void RescueUnit::resetDirty()
{
	dirty = false;
}

bool RescueUnit::isDirty()
{
	return dirty;
}

const unsigned int RescueUnit::getType()
{
	return type;
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

const std::tuple<double, double> RescueUnit::getPosUTuple()
{
	Position p = getPosU();
	return std::make_tuple(p.x, p.y);
}

Position RescueUnit::getPos()
{
	return pos;
}

Position RescueUnit::getPosU()
{
	Position p = getPos();
	Position pU = Position(
		(p.x - weather->getBounds().minx) / (weather->getBounds().maxx - weather->getBounds().minx),
		(p.y - weather->getBounds().miny) / (weather->getBounds().maxy - weather->getBounds().miny)
		);
	return pU;
}


void RescueUnit::setType(unsigned int _type)
{
	setDirty();
	type = _type;
}



Helicopter::Helicopter(const std::string & name, std::shared_ptr<Weather> weather):RescueUnit(name,weather)
{
	setType(CONST_TYPE_HELICOPTER);
	setPickupTimeNormVisibility(3);
	setPickupTimeLowVisibility(4);
	setMobilizationTime(15);
	setAvailability(1.);
	setSpeed(72.2222222);
}


//#pragma optimize("",off)
ERV::ERV(const std::string & name, std::shared_ptr<Weather> weather) :RescueUnit(name,weather)
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


