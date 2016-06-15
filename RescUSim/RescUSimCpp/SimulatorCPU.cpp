#define _USE_MATH_DEFINES

#include "SimulatorCPU.h"
#include "RescueUnitCPU.h"
#include <algorithm>
#include <iostream>
#include <chrono>


SimulatorCPU::SimulatorCPU(std::shared_ptr<Weather> weather) : Simulator(weather)
{

}

void SimulatorCPU::addStationaryRU(std::shared_ptr<Helicopter> ru)
{
	stationaryRUs.push_back(new HelicopterCPU(ru));
}

void SimulatorCPU::addStationaryRU(std::shared_ptr<ERV> ru)
{
	stationaryRUs.push_back(new ERVCPU(ru));
}

void SimulatorCPU::simulateResponseSample(double *res)
{
	//std::vector<double> res;
	auto start = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < samples.size(); i++) {
	//for (std::vector<Sample>::const_iterator sample = samples.begin(); sample != samples.end(); ++sample, i++) {
		Sample sample = samples[i];
		std::list<RUTime<RescueUnitCPU *>> lRut;
		for (std::list<RescueUnitCPU *>::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
			double t = (*ru)->getTravelTimeTo(*sample.poi, sample.scenario, weather);
			lRut.push_back(RUTime<RescueUnitCPU *>(t, *ru));
		}
		lRut.sort([](const RUTime< RescueUnitCPU *> &a, const RUTime< RescueUnitCPU *> &b) { return a.t < b.t; });
		unsigned int interferingUnits = 0;
		double resCap = 0;
		for (std::list<RUTime<RescueUnitCPU *>>::const_iterator rut = lRut.begin(); rut != lRut.end(); ++rut) {
			double coordOverhead = rut->ru->getCoordOverhead(interferingUnits);
			double pickupTime;
			pickupTime = rut->ru->getPickupTime( *(sample.poi), sample.scenario, weather);

			double c = (timelimit - rut->ru->getData()->getMobilizationTime() -
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
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Elapsed time is :  " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " mys " << std::endl;
}





//#pragma optimize("",off)
void SimulatorCPU::addRU(std::shared_ptr<Helicopter> ru) {
//#pragma omp parallel for schedule(dynamic)
//	for (int scenario = 0; scenario < weather.getNumScenarios()-1; scenario++) {
//
//		if (!(scenario % 500)) {
//			std::cout << "Scenario " << scenario << std::endl;
//		}
//		int i = 0;
//		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
//
//			if (weather.getBounds().within(ru->getPos())) {
//				double t = ru->getTravelTimeTo(*point, scenario, weather);
//
//				if (t <= 120) {
//
//					RUTime h;
//					h.t = t;
//					h.ru = ru;
//					resTim[scenario][i].push_back(h);
//				}
//			}
//
//		}
//	}
}

void SimulatorCPU::addRU(std::shared_ptr<ERV> ru) {

}

SimulatorCPU::~SimulatorCPU()
{
}
