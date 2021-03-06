#include "Simulator.h"
#include <iostream>
#include <omp.h>

#include <exception>
#include <algorithm>


//#pragma optimize("",off)
Simulator::Simulator(std::shared_ptr<Weather> weather):weather(weather), resCap(nullptr), timelimit(120)
{
	temporaryRUs.resize(weather->getNumScenarios());

	std::fill(temporaryRUs.begin(), temporaryRUs.end(), new RUList);
	/*for (int i = 0; i < weather.getNumScenarios(); i++) {
		temporaryRUs.push_back(new RUList);
	}*/
	//if (resCap != nullptr) { delete resCap; }
	generator = std::mt19937();

	
	
}


void Simulator::sample(size_t sampleSize) {

	//std::uniform_real_distribution<double> urDis(0.0, 2920 - 2);
	std::uniform_real_distribution<double> urDis(0.0, double(weather->getNumScenarios() - 2));
	std::uniform_int_distribution<size_t> uiDis(0, pois.size()-1);
	samples.clear();
	for (size_t s = 0; s < sampleSize; s++) {
		double scenario = urDis(generator);
		size_t pointIndex = uiDis(generator);
		samples.push_back(Sample(scenario, &pois[pointIndex]));
	}
}

size_t Simulator::getSampleSize() {
	return samples.size();
}

//#pragma optimize("",off)
double Simulator::simulateTravel()
{
	double sum = 0;
//
//	//std::mt19937 generator(123);
//	//std::uniform_real_distribution<double> dis(0.0, 1.0);
//	//__debugbreak();
//	
//#pragma omp parallel for schedule(dynamic) num_threads(4)
//	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
//		
//		if (!(scenario % 100)) {
//			//double randomRealBetweenZeroAndOne = dis(generator);
//			//std::cout << randomRealBetweenZeroAndOne << std::endl;
//			std::cout << "Scenario " << scenario << std::endl;
//		}
//		int i = 0;
//		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
//			
//
//			for (RUList::const_iterator ru = stationaryRUs.begin(); ru != stationaryRUs.end(); ++ru) {
//				if (weather.getBounds().within((**ru).getPos())) {
//					//if ((**ru).isAvailable(dis(generator))) {
//						double t = (**ru).getTravelTimeTo(*point, scenario, weather);
//						if (t <= 120) {
//							RUTime h;
//							h.t = t;
//							h.ru = *ru;
//							resTim[scenario][i].push_back(h);
//						}
//					//}
//				}
//			}
//			
//
//			for (RUList::const_iterator ru = temporaryRUs[scenario]->begin(); ru != temporaryRUs[scenario]->end(); ++ru) {
//				if (weather.getBounds().within((**ru).getPos())) {
//					//if ((**ru).isAvailable(dis(generator))) {
//						double t = (**ru).getTravelTimeTo(*point, scenario, weather);
//						if (t <= 120) {
//							RUTime h;
//							h.t = t;
//							h.ru = *ru;
//							resTim[scenario][i].push_back(h);
//						}
//					//}
//				}
//			}
//		}
//	}
	return sum;
}



//#pragma optimize("",off)
void Simulator::simulateResponse()
{
//	memset(resCap, 0, weather.getNumScenarios()*pois.size() * sizeof(double));
//	//__debugbreak();
//
//#pragma omp parallel for schedule(dynamic) num_threads(4)
//	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
//
//		/*if (!(scenario % 500)) {
//			std::cout << "Scenario " << scenario << std::endl;
//		}*/
//		int i = 0;
//		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
//
//			resTim[scenario][i].sort([](const RUTime &a, const RUTime &b) { return a.t < b.t; });
//			unsigned int interferingUnits = 0;
//
//			for (std::list<RUTime>::const_iterator rut = resTim[scenario][i].begin(); rut != resTim[scenario][i].end(); ++rut) {
//				double coordOverhead = rut->ru->getCoordOverhead(interferingUnits);
//
//				double c = (timelimit - rut->ru->getMobilizationTime() - 
//					rut->t) / (coordOverhead*rut->ru->getPickupTime(*point, scenario, weather));
//				c = (std::min)((std::max)(c, 0.), (double)rut->ru->getMaxCapacity());
//				
//				if (c > 0) {
//					resCap[weather.getNumScenarios()*i + scenario] += c;
//					if (rut->ru->isInterfering()) {
//						interferingUnits += 1;
//					}
//				}
//			}
//
//		}
//	}
}



std::shared_ptr<Helicopter> Simulator::addStationaryRU(std::shared_ptr<Helicopter> ru)
{
	std::cout << "Adding " << (*ru).getName() << std::endl;
	stationaryRUs.push_back(ru);
	return ru;
}

std::shared_ptr<ERV> Simulator::addStationaryRU(std::shared_ptr<ERV> ru)
{
	std::cout << "Adding " << (*ru).getName() << std::endl;
	stationaryRUs.push_back(ru);
	return ru;
}





void Simulator::removeRU(std::shared_ptr<RescueUnit> ru)
{
//#pragma omp parallel for schedule(dynamic) num_threads(4)
//	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {
//		int i = 0;
//		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {
//			resTim[scenario][i].remove_if([&](const RUTime a) { return a.ru == ru; });
//		}
//	}
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


void Simulator::setWeather(std::shared_ptr<Weather> weather_) {weather=weather_;}

std::shared_ptr<Weather> Simulator::getWeather()
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

