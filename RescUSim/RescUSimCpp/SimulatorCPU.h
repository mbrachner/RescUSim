#pragma once
#include "Simulator.h"
class SimulatorCPU :
	public Simulator
{
public:
	SimulatorCPU(Weather weather);
	void addRU(std::shared_ptr<Helicopter> ru);
	void addRU(std::shared_ptr<ERV> ru);
	~SimulatorCPU();
};

