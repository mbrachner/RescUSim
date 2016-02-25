
#include "RescueUnits.h"

RescueUnit::RescueUnit(const std::string & name) :name(name), posX(0), posY(0)
{
}

void RescueUnit::setName(const std::string & name_) { name = name_; }

const std::string & RescueUnit::getName() { return name; }

void RescueUnit::setPos(double posX_, double posY_) { posX = posX_; posY = posY_; }

const std::tuple<double, double> RescueUnit::getPos()
{
	return std::make_tuple( posX, posY );
}

double RescueUnit::getTravelTimeTo(std::tuple<double, double> point, Weather weather)
{
	return 0.0;
}


double Helicopter::getTravelTimeTo(std::tuple<double, double> point, Weather weather)
{
	return 1;
}

double ERV::getTravelTimeTo(std::tuple<double, double> point, Weather weather)
{
	return 2;
}
