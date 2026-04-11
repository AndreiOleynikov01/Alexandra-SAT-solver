#include "Alexandra.h"

namespace Graph
{
	IUnit::IUnit(State value, int variable) : value(value), variable(variable) {}

	UnitPulse::UnitPulse(State value, int variable) : IPulse(PulseType::UnitPulse), IUnit(value,variable) {}

	IPulse* UnitPulse::operator+(IPulse& pulse)
	{
		switch (pulse.type)
		{
		case PulseType::UnitPulse:
			UnitPulse* upulse = pulse.toUnitPulse();
			if (variable != upulse->variable)
			{
				if (upulse -> value != State::CONFLICT) 
				{
					std::vector<IPulse*> pulses;
					pulses.push_back(this);
					pulses.push_back(&pulse);
					return new Graph::Pulse(false,pulses);
				}
				else
				{
					return &pulse;
				}
				
			}
			else if (value == upulse->value || upulse->value == State::ANY)
			{
				return this;
			}
			else if  (value == State::ANY)
			{
				return &pulse;
			}
			else
			{
				return new UnitPulse(CONFLICT, 0);
			}
		default:
			return pulse + *this;
		}
	}

	Graph::UnitPulse* UnitPulse::toUnitPulse()
	{
		return this;
	}

	IPulse* UnitPulse::open() 
	{
		return new UnitPulse(ANY, variable);
	}

	bool UnitPulse::operator==(IPulse& lPulse)
	{
		for (IUnit& unit : lPulse.getVariables())
		{
			if (this->variable == unit.variable)
			{
				return true;
			}
		}
		return false;
	}

	std::vector<IUnit> UnitPulse::getVariables()
	{
		std::vector<IUnit> vec;
		vec.push_back(*this);
		return vec;
	}


	std::vector <IPulse*> UnitPulse::getvalues()
	{
		std::vector<IPulse*> vec;
		vec.push_back(this);
		return vec;
	}

	std::string UnitPulse::print()
	{
		return this->variable + " = " + this->value;
	}

	bool UnitPulse::isNegative() { return this->value == FALSE; }
}