#include "Alexandra.h"

namespace Graph
{
	IUnit::IUnit(State value, int variable) : value(value), variable(variable) {}

	IUnit::IUnit(IUnit* unit) : value(unit->value), variable(unit->variable) {}

	IUnit& IUnit::operator=(IUnit& unit)
	{
		return *new IUnit(unit);
	}

	UnitPulse::UnitPulse(State value, int variable) : IPulse(PulseType::UnitPulse), IUnit(value,variable) {}

	IPulse* UnitPulse::operator+(IPulse& pulse)
	{
		if (pulse.type == PulseType::UnitPulse)
		{
			UnitPulse& up = dynamic_cast<UnitPulse&>(pulse);
			if (up.variable != this->variable)
			{
				if (up.value == CONFLICT)
				{
					return &pulse;
				}
				else
				{
					std::vector<IPulse*> pulses;
					pulses.push_back(this);
					pulses.push_back(&pulse);
					return new Graph::Pulse(false, pulses);
				}
			}
			else
			{
				if (up.value == this->value|| up.value == ANY)
				{
					return this;
				}
				else if (this-> value == ANY)
				{
					return &pulse;
				}
				else
				{
					return new UnitPulse(CONFLICT, 0);
				}
			}
		}

		return pulse + *this;
	}

	IPulse* UnitPulse::open() 
	{
		return new UnitPulse(ANY, variable);
	}

	bool UnitPulse::operator==(IPulse& lPulse)
	{
		for (IUnit unit : lPulse.getVariables())
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
		vec.push_back(IUnit(value,variable));
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
		std::string result = "";
		result += std::to_string(variable);
		result += " = ";
		switch (value)
		{
		case TRUE:
			result += "TRUE";
			break;
		case FALSE:
			result += "FALSE";
			break;
		case CONFLICT:
			result += "CONFLICT";
			break;
		case ANY:
			result += "ANY";
		}
		return result;
	}

	bool UnitPulse::isNegative() { return false; }

	Graph::IPulse* UnitPulse::negate()
	{
		State new_value;
		switch (this->value)
		{
		case TRUE:
			new_value = FALSE;
			break;
		case FALSE:
			new_value = TRUE;
			break;
		default:
			new_value = this->value;
		}
		return new UnitPulse(new_value, this->variable);
	}
}