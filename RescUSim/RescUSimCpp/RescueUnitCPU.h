#pragma once
#include "Weather.h"
#include "Position.h"
#include "RescueUnits.h"

class RescueUnitCPU
{
public:
	RescueUnitCPU();
	virtual double getTravelTimeTo(Position dest, double scenario, std::shared_ptr<Weather> weather)=0;
	virtual double getPickupTime(Position pos, double scenario, std::shared_ptr<Weather> weather)=0;
	virtual double getCoordOverhead(int interferingUnits)=0;
	std::shared_ptr <RescueUnit> getData();
	~RescueUnitCPU();
protected:
	const int DISTSTEP = 10000;
	std::shared_ptr <RescueUnit> data;
};

class HelicopterCPU : public RescueUnitCPU {
public:
	HelicopterCPU(std::shared_ptr<Helicopter>);
	double getTravelTimeTo(Position dest, double scenario, std::shared_ptr<Weather> weather);
	double getPickupTime(Position pos, double scenario, std::shared_ptr<Weather> weather);
	double getCoordOverhead(int interferingUnits);

};

class ERVCPU : public RescueUnitCPU {
public:
	ERVCPU(std::shared_ptr<ERV>);
	double getTravelTimeTo(Position dest, double scenario, std::shared_ptr<Weather> weather);
	double getPickupTime(Position pos, double scenario, std::shared_ptr<Weather> weather);
	double getCoordOverhead(int interferingUnits);
};
