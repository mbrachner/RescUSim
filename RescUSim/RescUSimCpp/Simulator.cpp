#include "Simulator.h"
#include <iostream>
#include <omp.h>
#include <algorithm>
#include <random>

struct RUTime {
	double t;
	std::shared_ptr<RescueUnit> ru;
};

Simulator::Simulator(Weather weather):weather(weather), resCap(nullptr), timelimit(120)
{
	temporaryRUs.reserve(weather.getNumScenarios());
	for (int i = 0; i < weather.getNumScenarios(); i++) {
		//temporaryRUs[i] = new RUList;
		temporaryRUs.push_back(new RUList);
	}
}


#pragma optimize("",off)
double Simulator::simulate()
{
	double sum = 0;
	if (resCap != nullptr) { delete resCap;  }
	
	resCap = new double[weather.getNumScenarios()*pois.size()];

	memset(resCap, 0, weather.getNumScenarios()*pois.size()*sizeof(double));
	std::mt19937 generator(123);
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	
#pragma omp parallel for schedule(dynamic) num_threads(3)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {

		if (!(scenario % 100)) {
			//double randomRealBetweenZeroAndOne = dis(generator);
			//std::cout << randomRealBetweenZeroAndOne << std::endl;
			std::cout << "Scenario " << scenario << std::endl;
		}
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
			std::list<RUTime> resTim;

			for (RUList::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
				if (weather.getBounds().within((**ru).getPos())) {
					if ((**ru).isAvailable(dis(generator))) {
						/*resCap[weather.getNumScenarios()*i + scenario] +=
							(**ru).getCapacityTo(*point, scenario, weather, getTimelimit());
						(**ru).getTravelTimeTo(*point, scenario, weather);*/
						double t = (**ru).getTravelTimeTo(*point, scenario, weather);
						if (t <= 120) {
							RUTime *h = new RUTime();
							h->t = t;
							h->ru = *ru;
							resTim.push_back(*h);
						}
					}
				}
			}
			

			for (RUList::const_iterator ru = temporaryRUs[scenario]->begin(); ru != temporaryRUs[scenario]->end(); ++ru) {
				if (weather.getBounds().within((**ru).getPos())) {
					if ((**ru).isAvailable(dis(generator))) {
						/*resCap[weather.getNumScenarios()*i + scenario] +=
							(**ru).getCapacityTo(*point, scenario, weather, getTimelimit());*/
						double t = (**ru).getTravelTimeTo(*point, scenario, weather);
						if (t <= 120) {
							RUTime *h = new RUTime();
							h->t = t;
							h->ru = *ru;
							resTim.push_back(*h);
						}
					}
				}
			}

			//resTim.sort();
			resTim.sort([](const RUTime & a, const RUTime & b) { return a.t < b.t; });
			unsigned int interferingUnits = 0;

			for (std::list<RUTime>::const_iterator rut = resTim.begin(); rut != resTim.end(); ++rut) {
				double coordOverhead = (*(*rut).ru).getCoordOverhead(interferingUnits);
				
				double c = (timelimit - (*(*rut).ru).getMobilizationTime() - 
					(*rut).t) / (coordOverhead*(*(*rut).ru).getPickupTime(*point, scenario, weather));
				c = std::min(std::max(c, 0.), (double)(*(*rut).ru).getMaxCapacity());
				if (c > 0) {
					resCap[weather.getNumScenarios()*i + scenario] += c;
					if ((*(*rut).ru).isInterfering()) {
						interferingUnits += 1;
					}
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

