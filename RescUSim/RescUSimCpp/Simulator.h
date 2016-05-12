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
	Simulator(Weather wether);
	double simulateTravel();
	void simulateResponse();
	void addStationaryRU(std::shared_ptr<RescueUnit> ru);
	void addRU(std::shared_ptr<RescueUnit> ru);
	void addRUOpenCL(std::shared_ptr<RescueUnit> ru);
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

private:
	Weather weather;
	RUList stationaryRUs;
	PositionList pois;
	std::vector<RUList*> temporaryRUs;
	std::vector<std::vector<std::list<RUTime>>> resTim;
	double *resCap;
	double timelimit;
}; 

