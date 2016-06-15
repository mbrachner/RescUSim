#pragma once
#include "Simulator.h"
#include "RescueUnitCPU.h"

class SimulatorCPU :
	public Simulator
{
public:
	SimulatorCPU(std::shared_ptr<Weather> weather);
	void addStationaryRU(std::shared_ptr<Helicopter> ru);
	void addStationaryRU(std::shared_ptr<ERV> ru);
	void simulateResponseSample(double * res);
	void addRU(std::shared_ptr<Helicopter> ru);
	void addRU(std::shared_ptr<ERV> ru);
	~SimulatorCPU();

private:

	std::list<RescueUnitCPU *> stationaryRUs;
};

