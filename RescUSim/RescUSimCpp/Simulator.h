#pragma once
#include "RescueUnits.h"
#include "Weather.h"
#include <list>
#include <vector>
#include "Position.h"

typedef std::list<std::tuple<double, double>> DoubleTupleList;
typedef std::list<Position> PositionList;
typedef std::list<std::shared_ptr<RescueUnit>> RUList;


class Simulator
{
public:
	Simulator(Weather wether);
	double simulate();
	void addStationaryRU(std::shared_ptr<RescueUnit> ru);
	void addTemporaryRU(std::shared_ptr<RescueUnit> ru, size_t scenario);
	void addPoi(Position p);
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
	double *resCap;
	double timelimit;
}; 

