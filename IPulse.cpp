#include "Alexandra.h"

namespace Graph
{
	IPulse::IPulse(PulseType type) : type(type) {}

	Graph::IPulse* IPulse::open() { return NULL; }

	bool IPulse::operator==(IPulse&) { return false; };

	IPulse* IPulse::operator+(IPulse&) { return NULL; }

	std::vector <Graph::UnitPulse*> IPulse::getUnits() { return std::vector<Graph::UnitPulse*>(); }

	std::vector <Graph::IPulse*> IPulse::getPulses() { return std::vector<IPulse*>(); }

	std::vector<IUnit> IPulse::getVariables() { return std::vector<IUnit>(); }

	std::string IPulse::print() { return ""; }

	bool IPulse::isNegative() { return false; }

	Graph::IPulse* IPulse::negate() { return NULL; }
}