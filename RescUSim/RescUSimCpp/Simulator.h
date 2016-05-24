#pragma once
#include "RescueUnits.h"
#include "Weather.h"
#include "Position.h"

struct RUTime {
	double t;
	std::shared_ptr<RescueUnit> ru;
};

class Simulator
{
public:
	Simulator(Weather weather);
	double simulateTravel();
	void simulateResponse();
	void addStationaryRU(std::shared_ptr<RescueUnit> ru);
	virtual void addRU(std::shared_ptr<Helicopter> ru) = 0;
	virtual void addRU(std::shared_ptr<ERV> ru) = 0;
	void removeRU(std::shared_ptr<RescueUnit> ru);
	void addTemporaryRU(std::shared_ptr<RescueUnit> ru, size_t scenario);
	void addPoi(Position p);
	void initResTim();
	size_t getNumPois();
	void setWeather(Weather weather);
	Weather getWeather();
	void setTimelimit(double val);
	double getTimelimit();
	double *getResCap();
	~Simulator();

protected:
	Weather weather;
	std::vector<std::vector<std::list<RUTime>>> resTim;
	PositionList pois;
	RUList stationaryRUs;
	std::vector<RUList*> temporaryRUs;
	double *resCap;
	double timelimit;
}; 

