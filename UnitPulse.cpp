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
}