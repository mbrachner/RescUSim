#include "Simulator.h"
#include <iostream>
#include <omp.h>
#include <algorithm>


Simulator::Simulator(Weather weather):weather(weather), resCap(nullptr), timelimit(120)
{
	temporaryRUs.reserve(weather.getNumScenarios());
	for (int i = 0; i < weather.getNumScenarios(); i++) {
		temporaryRUs[i] = new RUList;
	}
}



double Simulator::simulate()
{
	double sum = 0;
	if (resCap != nullptr) { delete resCap;  }
	resCap = new double[weather.getNumScenarios()*pois.size()];
	memset(resCap, 0, weather.getNumScenarios()*pois.size()*sizeof(double));
	
#pragma omp parallel for schedule(dynamic) num_threads(3)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
		if (!(scenario % 100)) {
			std::cout << "Scenario " << scenario << std::endl;
		}
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
			for (RUList::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
				if (weather.getBounds().within((**ru).getPos())) {
					resCap[weather.getNumScenarios()*i + scenario] +=
						(**ru).getCapacityTo(*point, scenario, weather, getTimelimit());
				}
			}
			for (RUList::const_iterator ru = temporaryRUs[scenario]->begin(); ru != temporaryRUs[scenario]->end(); ++ru) {
				if (weather.getBounds().within((**ru).getPos())) {
					resCap[weather.getNumScenarios()*i + scenario] +=
						(**ru).getCapacityTo(*point, scenario, weather, getTimelimit());
				}
			}
		}
	}
	return sum;
}

void Simulator::addStationaryRU(std::shared_ptr<RescueUnit> ru)
{
	std::cout << "Adding " << (*ru).getName() << std::endl;
	stationaryRUs.push_back(ru);
}

void Simulator::addTemporaryRU(std::shared_ptr<RescueUnit> ru, size_t scenario)
{
	temporaryRUs[scenario]->push_back(ru);
}



void Simulator::addPoi(Position p)
{
	pois.push_back(p);
}

size_t Simulator::getNumPois()
{
	return pois.size();
}


void Simulator::setWeather(Weather weather_) {weather=weather_;}

Weather Simulator::getWeather()
{
	return weather;
}

void Simulator::setTimelimit(double val)
{
	timelimit = val;
}

double Simulator::getTimelimit()
{
	return timelimit;
}


double * Simulator::getResCap()
{
	return resCap;
}


Simulator::~Simulator()
{
	if (resCap != nullptr) { delete resCap; }
}

