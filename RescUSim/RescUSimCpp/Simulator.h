#pragma once
#include "RescueUnits.h"
#include "Weather.h"
#include "Position.h"
#include <random>

template <typename T>
struct RUTime {
	RUTime() {
		t = nan("");
		ru = nullptr;
	}
	RUTime(double _t, T _ru) {
		t = _t;
		ru = _ru;
	}
	double t;
	T ru;
};

struct Sample {
	Sample(double _scenario, Position *_poi) {
		scenario = _scenario; poi = _poi;
	}
	double scenario;
	Position *poi;
};

class Simulator
{
public:
	Simulator(std::shared_ptr<Weather> weather);
	virtual void sample(size_t sampleSize);
	size_t getSampleSize();
	double simulateTravel();
	void simulateResponse();
	virtual void simulateResponseSample(double *res) = 0;
	//virtual void addStationaryRU(std::shared_ptr<RescueUnit> ru);
	virtual std::shared_ptr<Helicopter> addStationaryRU(std::shared_ptr<Helicopter> ru);
	virtual std::shared_ptr<ERV> addStationaryRU(std::shared_ptr<ERV> ru);
	void removeRU(std::shared_ptr<RescueUnit> ru);
	void addTemporaryRU(std::shared_ptr<RescueUnit> ru, size_t scenario);
	void addPoi(Position p);
	size_t getNumPois();
	void setWeather(std::shared_ptr<Weather> weather);
	std::shared_ptr<Weather> getWeather();
	void setTimelimit(double val);
	double getTimelimit();
	double *getResCap();
	~Simulator();

protected:
	std::shared_ptr<Weather> weather;
	//std::vector<std::vector<std::list<RUTime>>> resTim;
	PositionList pois;
	RUList stationaryRUs;
	std::vector<RUList*> temporaryRUs;
	std::vector<Sample> samples;
	double *resCap;
	double timelimit;
	std::mt19937 generator;
}; 

