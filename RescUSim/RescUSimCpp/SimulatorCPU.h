#pragma once
#include "Simulator.h"
#include "RescueUnitCPU.h"

class SimulatorCPU :
	public Simulator
{
public:
	SimulatorCPU(std::shared_ptr<Weather> weather);
	virtual void sample(size_t sampleSize) override;
	std::shared_ptr<Helicopter> addStationaryRU(std::shared_ptr<Helicopter> ru) override;
	std::shared_ptr<ERV> addStationaryRU(std::shared_ptr<ERV> ru) override;
	void removeStationaryRU(std::shared_ptr<RescueUnit> ru);
	void simulateResponseSample(double * res);
	void initCache();
	~SimulatorCPU();

private:
	std::vector<std::list<RUTime<RescueUnitCPU *>>> lRutVec;
	std::list<RescueUnitCPU *> stationaryRUs;
};

