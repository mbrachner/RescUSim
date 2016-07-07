#define _USE_MATH_DEFINES

#include "SimulatorCPU.h"
#include "RescueUnitCPU.h"
#include <algorithm>
#include <iostream>
#include <functional>
#include <chrono>


SimulatorCPU::SimulatorCPU(std::shared_ptr<Weather> weather) : Simulator(weather)
{

}

void SimulatorCPU::sample(size_t sampleSize)
{
	Simulator::sample(sampleSize);
	initCache();
}

std::shared_ptr<Helicopter> SimulatorCPU::addStationaryRU(std::shared_ptr<Helicopter> ru)
{
	stationaryRUs.push_back(new HelicopterCPU(ru));
	return ru;
}

std::shared_ptr<ERV> SimulatorCPU::addStationaryRU(std::shared_ptr<ERV> ru)
{
	stationaryRUs.push_back(new ERVCPU(ru));
	return ru;
}

void SimulatorCPU::removeStationaryRU(std::shared_ptr<RescueUnit> rur)
{
	//__debugbreak();

	auto start = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < samples.size(); i++) {
		std::shared_ptr<RescueUnit> rux=rur;
		lRutVec[i].remove_if([rux](RUTime<RescueUnitCPU *> a) { return a.ru->getData() == rux; });
	}
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Elapsed time is :  " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " mys " << std::endl;

}


void SimulatorCPU::simulateResponseSample(double *res)
{
	//std::vector<double> res;
	//auto start = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < samples.size(); i++) {
	//for (std::vector<Sample>::const_iterator sample = samples.begin(); sample != samples.end(); ++sample, i++) {
		Sample sample = samples[i];
		//std::list<RUTime<RescueUnitCPU *>> lRut;
		for (std::list<RescueUnitCPU *>::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
			if ((*ru)->getData()->isDirty()) {
				std::shared_ptr<RescueUnit> rux = (*ru)->getData();
				lRutVec[i].remove_if([rux](RUTime<RescueUnitCPU *> a) { return a.ru->getData() == rux; });
				double t = (*ru)->getTravelTimeTo(*sample.poi, sample.scenario, weather);
				lRutVec[i].push_back(RUTime<RescueUnitCPU *>(t, *ru));
			}
		}
		lRutVec[i].sort([](const RUTime< RescueUnitCPU *> &a, const RUTime< RescueUnitCPU *> &b) { return a.t < b.t; });
		unsigned int interferingUnits = 0;
		double resCap = 0;
		for (std::list<RUTime<RescueUnitCPU *>>::const_iterator rut = lRutVec[i].begin(); rut != lRutVec[i].end(); ++rut) {
			double coordOverhead = rut->ru->getCoordOverhead(interferingUnits);
			double pickupTime;
			pickupTime = rut->ru->getPickupTime( *(sample.poi), sample.scenario, weather);

			double c = (timelimit - rut->ru->getMobilizationTime() -
				rut->t) / (coordOverhead*pickupTime);
			c = (std::min)((std::max)(c, 0.), (double)rut->ru->getData()->getMaxCapacity());

			if (c > 0) {
				resCap += c;
				if (rut->ru->getData()->isInterfering()) {
					interferingUnits += 1;
				}
			}
		}
		res[i] = resCap;
	}
	for (std::list<RescueUnitCPU *>::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
		(*ru)->getData()->resetDirty();
	}
	//auto end = std::chrono::steady_clock::now();
	//auto diff = end - start;
	//std::cout << "Elapsed time is :  " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " mys " << std::endl;
}

void SimulatorCPU::initCache()
{
	lRutVec.resize(samples.size());
	for (std::list<RescueUnitCPU *>::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
		(*ru)->getData()->setDirty();
	}
}



SimulatorCPU::~SimulatorCPU()
{
}
