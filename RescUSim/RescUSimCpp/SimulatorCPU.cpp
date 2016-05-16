#include "SimulatorCPU.h"
#include <iostream>


SimulatorCPU::SimulatorCPU(Weather weather) : Simulator(weather)
{
}


void SimulatorCPU::addRU(std::shared_ptr<RescueUnit> ru) {
#pragma omp parallel for schedule(dynamic) num_threads(4)
	for (int scenario = 0; scenario < weather.getNumScenarios(); scenario++) {

		if (!(scenario % 500)) {
			std::cout << "Scenario " << scenario << std::endl;
		}
		int i = 0;
		for (PositionList::const_iterator point = pois.begin(); point != pois.end(); ++point, i++) {

			if (weather.getBounds().within(ru->getPos())) {
				double t = ru->getTravelTimeTo(*point, scenario, weather);

				if (t <= 120) {

					RUTime h;
					h.t = t;
					h.ru = ru;
					resTim[scenario][i].push_back(h);
				}
			}

		}
	}
}

SimulatorCPU::~SimulatorCPU()
{
}
