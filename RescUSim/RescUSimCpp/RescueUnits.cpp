
#include "RescueUnits.h"

RescueUnit::RescueUnit(const std::string & name) :name(name)
{
}

void RescueUnit::setName(const std::string & name_) { name = name_; }

const std::string & RescueUnit::getName() { return name; }








