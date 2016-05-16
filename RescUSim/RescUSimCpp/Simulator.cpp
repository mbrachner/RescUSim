#include "Simulator.h"
#include <iostream>
#include <omp.h>
#include <random>
#include <exception>
#include <algorithm>


//#pragma optimize("",off)
Simulator::Simulator(Weather weather):weather(weather), resCap(nullptr), timelimit(120)
{
	temporaryRUs.resize(weather.getNumScenarios());

	std::fill(temporaryRUs.begin(), temporaryRUs.end(), new RUList);
	/*for (int i = 0; i < weather.getNumScenarios(); i++) {
		temporaryRUs.push_back(new RUList);
	}*/
	//if (resCap != nullptr) { delete resCap; }


	
	
}


//#pragma optimize("",off)
double Simulator::simulateTravel()
{
	double sum = 0;

	std::mt19937 generator(123);
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	//__debugbreak();
	
#pragma omp parallel for schedule(dynamic) num_threads(4)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
		
		if (!(scenario % 100)) {
			//double randomRealBetweenZeroAndOne = dis(generator);
			//std::cout << randomRealBetweenZeroAndOne << std::endl;
			std::cout << "Scenario " << scenario << std::endl;
		}
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
			

			for (RUList::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
				if (weather.getBounds().within((**ru).getPos())) {
					if ((**ru).isAvailable(dis(generator))) {
						double t = (**ru).getTravelTimeTo(*point, scenario, weather);
						if (t <= 120) {
							RUTime h;
							h.t = t;
							h.ru = *ru;
							resTim[scenario][i].push_back(h);
						}
					}
				}
			}
			

			for (RUList::const_iterator ru = temporaryRUs[scenario]->begin(); ru != temporaryRUs[scenario]->end(); ++ru) {
				if (weather.getBounds().within((**ru).getPos())) {
					if ((**ru).isAvailable(dis(generator))) {
						double t = (**ru).getTravelTimeTo(*point, scenario, weather);
						if (t <= 120) {
							RUTime h;
							h.t = t;
							h.ru = *ru;
							resTim[scenario][i].push_back(h);
						}
					}
				}
			}
		}
	}
	return sum;
}



//#pragma optimize("",off)
void Simulator::simulateResponse()
{
	memset(resCap, 0, weather.getNumScenarios()*pois.size() * sizeof(double));
	//__debugbreak();

#pragma omp parallel for schedule(dynamic) num_threads(4)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {

		if (!(scenario % 500)) {
			std::cout << "Scenario " << scenario << std::endl;
		}
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {

			resTim[scenario][i].sort([](const RUTime &a, const RUTime &b) { return a.t < b.t; });
			unsigned int interferingUnits = 0;

			for (std::list<RUTime>::const_iterator rut = resTim[scenario][i].begin(); rut != resTim[scenario][i].end(); ++rut) {
				double coordOverhead = rut->ru->getCoordOverhead(interferingUnits);

				double c = (timelimit - rut->ru->getMobilizationTime() - 
					rut->t) / (coordOverhead*rut->ru->getPickupTime(*point, scenario, weather));
				c = (std::min)((std::max)(c, 0.), (double)rut->ru->getMaxCapacity());
				
				if (c > 0) {
					resCap[weather.getNumScenarios()*i + scenario] += c;
					if (rut->ru->isInterfering()) {
						interferingUnits += 1;
					}
				}
			}

		}
	}

}

void Simulator::addStationaryRU(std::shared_ptr<RescueUnit> ru)
{
	std::cout << "Adding " << (*ru).getName() << std::endl;
	stationaryRUs.push_back(ru);
}





void Simulator::removeRU(std::shared_ptr<RescueUnit> ru)
{
#pragma omp parallel for schedule(dynamic) num_threads(4)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
			resTim[scenario][i].remove_if([&](const RUTime a) { return a.ru == ru; });
		}
	}
}

void Simulator::addTemporaryRU(std::shared_ptr<RescueUnit> ru, size_t scenario)
{
	temporaryRUs[scenario]->push_back(ru);
}



void Simulator::addPoi(Position p)
{
	pois.push_back(p);
}

void Simulator::initResTim() {
	resTim.resize(weather.getNumScenarios());
	for (int i = 0; i < weather.getNumScenarios(); i++) {
		resTim[i].resize(pois.size());
	}
	resCap = new double[weather.getNumScenarios()*pois.size()];
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

