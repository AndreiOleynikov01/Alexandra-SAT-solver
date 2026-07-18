#include "Alexandra.h"

namespace Graph
{
	AggregatedPulse::AggregatedPulse() : IPulse(PulseType::AggregatedPulse), exclusionSet(), definitiveSet(), size(0) {}

	AggregatedPulse::AggregatedPulse(Graph::Pulse& left, Graph::Pulse& right) : IPulse(PulseType::AggregatedPulse), exclusionSet(), definitiveSet(), size(0)
	{
		if (left.isNegative() && right.isNegative())
		{
			std::vector<int> common_variables, left_variables, right_variables;
			std::vector<IPulse*> left_buffer = left.getvalues();
			std::vector<IPulse*> right_buffer = right.getvalues();
			std::vector<IUnit> left_exclusive = left.getVariables();
			std::vector<IUnit> right_exclusive = right.getVariables();

			for (IPulse* lp : left_buffer)
			{
				bool common = false;
				for (IPulse* rp : right_buffer)
				{
					if (*lp == *rp)
					{
						common = true;
						common_variables.push_back(dynamic_cast<Graph::UnitPulse*>(lp)->variable);
					}
				}
				if (!common)
				{
					left_variables.push_back(dynamic_cast<Graph::UnitPulse*>(lp)->variable);
				}
			}
			for (IPulse* rp : right_buffer)
			{
				int rv = dynamic_cast<Graph::UnitPulse*>(rp)->variable;
				bool common = false;
				for (int lv : common_variables)
				{
					if (lv == rv)
					{
						common = true;
					}
				}
				if (!common)
				{
					right_variables.push_back(rv);
				}
			}

			for (int lv : left_variables)
			{
				Graph::UnitPulse* lp = new Graph::UnitPulse(ANY, lv);
				right_exclusive.push_back(*lp);
				definitiveSet.push_back(lp);
			}
			for (int rv : right_variables)
			{
				Graph::UnitPulse* lp = new Graph::UnitPulse(ANY, rv);
				left_exclusive.push_back(*lp);
				definitiveSet.push_back(lp);
			}
			for (int v : common_variables)
			{

				definitiveSet.push_back(new Graph::UnitPulse(ANY, v));
			}

			exclusionSet.push_back(left_exclusive);
			exclusionSet.push_back(right_exclusive);
			size = common_variables.size() + left_variables.size() + right_variables.size();
		}
	}

	bool AggregatedPulse::operator==(IPulse& pulse)
	{
		std::vector<IUnit> target_variables = pulse.getVariables();
		std::vector<IUnit> variables = getVariables();
		for (IUnit& unit : variables)
		{
			for (IUnit& target_unit : target_variables)
			{
				if (unit.variable == target_unit.variable)
				{
					return true;
				}
			}
		}
		return false;
	}

	IPulse* AggregatedPulse::operator+ (IPulse& pulse)
	{
		
	}

	std::vector <IPulse*> AggregatedPulse::getvalues()
	{
		std::vector<IPulse*> result;
		for (std::vector<IUnit> exclude : exclusionSet)
		{
			std::vector<IPulse*> buffer;
			for (IUnit variable : exclude)
			{
				buffer.push_back(new Graph::UnitPulse(variable.value, variable.variable));
			}
			result.push_back(new Graph::Pulse(true, buffer));
		}
		return result;
	}

	std::vector<IUnit> AggregatedPulse::getVariables()
	{
		std::vector<IUnit> vec;

		for (IPulse* pulse : definitiveSet)
		{
			for (IUnit unit : pulse->getVariables())
			{
				vec.push_back(unit);
			}
		}

		return vec;
	}

	IPulse* AggregatedPulse::open()
	{
		return new Graph::Pulse(false, definitiveSet);
	}

	std::string AggregatedPulse::print()
	{
		std::string result;
		result += "Not { ";
		for (int i = 0; i < exclusionSet.size(); i++)
		{
			result += "( ";
			std::vector<IPulse*> buffer;
			for (IUnit variable : exclusionSet[i])
			{
				buffer.push_back(new Graph::UnitPulse(variable.value, variable.variable));
			}
			result += Graph::Pulse(false, buffer).print();
			result += " )";
			if (i < exclusionSet.size() - 1)
			{
				result += ", ";
			}
		}
		result += " }";
		return result;
	}

	bool AggregatedPulse::isNegative()
	{
		return true;
	}

}