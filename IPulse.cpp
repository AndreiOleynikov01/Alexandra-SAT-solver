#include "Alexandra.h"

namespace Graph 
{
	IPulse::IPulse(PulseType type) : type(type) {}

	Graph::AggregatedPulse* IPulse::toAggregatedPulse() {}

	Graph::Pulse* IPulse::toPulse() {}

	Graph::UnitPulse* IPulse::toUnitPulse() {}

	Graph::IPulse* IPulse::open() {}

	bool IPulse::operator==(IPulse&){}

	IPulse* IPulse::operator+(IPulse&) {}

	std::vector <IPulse*> IPulse::getvalues() {}

	std::vector<IUnit> IPulse::getVariables() {}

	std::string IPulse::print() {}

	bool IPulse::isNegative() {}
}