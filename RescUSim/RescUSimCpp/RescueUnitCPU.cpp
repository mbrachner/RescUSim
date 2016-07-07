#define _USE_MATH_DEFINES

#include "RescueUnitCPU.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

RescueUnitCPU::RescueUnitCPU()
{
}



std::shared_ptr <RescueUnit> RescueUnitCPU::getData()
{
	return data;
}

RescueUnitCPU::~RescueUnitCPU()
{
}

HelicopterCPU::HelicopterCPU(std::shared_ptr<Helicopter> ru)
{
	data = ru;
	generator = std::mt19937();
	mtDis= std::lognormal_distribution<double>(1.17170354376, 0.875468737354);
}

double HelicopterCPU::getTravelTimeTo(Position dest, double scenario, std::shared_ptr<Weather> weather) {
	Position start = getData()->getPos();
	double t = 0;

	double crs = atan2(dest.y - start.y, dest.x - start.x);
	double dist = sqrt(pow(dest.x - start.x, 2) + pow(dest.y - start.y, 2));

	for (int step = 0; step <= (int)dist / DISTSTEP; step++) {
		double distStep = (std::min)((double)DISTSTEP, dist - step * DISTSTEP);

		Position act = {
			start.x + step * DISTSTEP * (double)cos(crs),
			start.y + step * DISTSTEP * (double)sin(crs)
		};
		//printf("Position: %f %f ", act.x, act.y);
		std::tuple<double, double> wind = weather->windAt(scenario + t / 180, act.x, act.y);
		//printf("WindAt: %.9f,%.9f,%.9f ", act.x, act.y, scenario+t/180);
		double wsp = std::get<0>(wind);
		double wd = std::get<1>(wind);
		//printf("Wind: %.9f %.9f ", wsp, wd);
		double m = wd - fmod(M_PI_2 - crs, 2 * M_PI);
		double gs = data->getSpeed()*sqrt(1 - pow(wsp, 2) / pow(data->getSpeed(), 2) * pow(sin(m), 2)) - wsp*cos(m);
		
		
		t += distStep / (gs * 60);
		//std::cout << std::setprecision(10) << t << std::endl;
		if (t > 120) {
			t = nan("");
			break;
		}
	}
	//printf("T: %.9f\n", t);
	return t;
}

double HelicopterCPU::getPickupTime(Position pos, double scenario, std::shared_ptr<Weather> weather) {
	if (weather->hsAt(scenario, pos.x, pos.y) < 6) {
		if (weather->lightAt(scenario, pos.x, pos.y))
			return 2;
		else
			return 3;
	}
	else {
		if (weather->lightAt(scenario, pos.x, pos.y))
			return 3;
		else
			return 4;
	}
}

ERVCPU::ERVCPU(std::shared_ptr<ERV> ru)
{
	data = ru;
}

double ERVCPU::getTravelTimeTo(Position pos, double scenario, std::shared_ptr<Weather> weather) {
	Position start = getData()->getPos();
	double t = 0;
	//printf("To: %f %f \n", pos.x, pos.y);
	double crs = atan2(pos.y - start.y, pos.x - start.x);
	double dist = sqrt(pow(pos.x - start.x, 2) + pow(pos.y - start.y, 2));
	for (int step = 0; step <= (int)dist / DISTSTEP; step++) {
		double distStep = std::min((double)DISTSTEP, dist - step * DISTSTEP);

		Position act = {
			start.x + step * DISTSTEP * (double)cos(crs),
			start.y + step * DISTSTEP * (double)sin(crs)
		};
		//printf("Position: %f %f %f \n", act.x, act.y,t);

		double hs = weather->hsAt(scenario + t / 180, act.x, act.y);
		if (hs > 10) {
			//t = std::numeric_limits<double>::max();
			t = 120;
			break;
		}

		double coeff = (hs <= 3) ? (9.311444 - getData()->getSpeed()) / 3 :
			(9.054222 - getData()->getSpeed()) / 5;

		double gs = getData()->getSpeed() + coeff*hs;
		t += distStep / (gs * 60);

		if (t > 120) {
			//t = std::numeric_limits<double>::max();
			t = 120;
			break;
		}
	}
	//printf("T: %.9f\n", t);
	return t;
}

double ERVCPU::getPickupTime(Position pos, double scenario, std::shared_ptr<Weather> weather)
{
	if (weather->hsAt(scenario, pos.x, pos.y) < 2) {
		if (weather->lightAt(scenario, pos.x, pos.y))
			return 1;
		else
			return 2;
	}
	else {
		if (weather->hsAt(scenario, pos.x, pos.y) < 4.5) {
			if (weather->lightAt(scenario, pos.x, pos.y))
				return 2;
			else
				return 3;
		}
		else {
			if (weather->hsAt(scenario, pos.x, pos.y) < 6) {
				if (weather->lightAt(scenario, pos.x, pos.y))
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

double HelicopterCPU::getMobilizationTime() {
	return 10+mtDis(generator);
}

double HelicopterCPU::getCoordOverhead(int interferingUnits)
{
	return (interferingUnits == 0) ? 1 : 2;
}

double ERVCPU::getCoordOverhead(int interferingUnits)
{
	return 1;
}

double RescueUnitCPU::getMobilizationTime() {
	return data->getMobilizationTime();
}