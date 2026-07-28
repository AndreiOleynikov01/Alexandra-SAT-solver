#include "Alexandra.h"

namespace Graph
{
	AggregatedPulse::AggregatedPulse(bool negative, std::vector<int> variables, std::vector<IPulse*> entries) : IPulse(PulseType::AggregatedPulse), negative(negative), variables(variables), entries(entries){}

	AggregatedPulse::AggregatedPulse(bool negative, Graph::Pulse& left, Graph::Pulse& right) : IPulse(PulseType::AggregatedPulse), negative(negative), variables(), entries()
	{
		std::vector<IUnit> left_variables = left.getVariables();
		std::vector<IUnit> right_variables = right.getVariables();
		std::vector<Graph::UnitPulse*>right_padding, left_padding;
		
		for (IUnit lu : left_variables)
		{
			bool found = false;
			for (IUnit ru : right_variables)
			{
				if (lu.variable == ru.variable)
				{
					found = true;
					variables.push_back(lu.variable);
				}
			}

			if (!found)
			{
				right_padding.push_back(new Graph::UnitPulse(ANY, lu.variable));
			}
		}

		for (IUnit ru : right_variables)
		{
			bool found = false;
			for (int i : variables)
			{
				if (ru.variable == i)
				{
					found = true;
				}
			}

			if (!found)
			{
				left_padding.push_back(new Graph::UnitPulse(ANY, ru.variable));
			}
		}
		Graph::Pulse left_padding_pulse(false, std::vector<IPulse*>(), left_padding);
		Graph::IPulse* left_entry = *left.negate()+left_padding_pulse;

		Graph::Pulse right_padding_pulse(false, std::vector<IPulse*>(), right_padding);
		Graph::IPulse* right_entry = *left.negate() + right_padding_pulse;

		entries.push_back(left_entry);
		entries.push_back(right_entry);
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
		std::vector<int> variables;
		std::vector<IPulse*> entries;

		switch (pulse.type) 
		{
			case UnitPulse:
			{
				Graph::UnitPulse* unit_pulse = dynamic_cast<Graph::UnitPulse*>(&pulse);
				for (int i : this->variables)
				{
					if (i != unit_pulse->variable)
					{
						variables.push_back(i);
					}
				}

				if (variables.size() == 1)
				{
					return &pulse;
				}

				for (IPulse* entry : this->entries)
				{
					IPulse* negation = entry->negate();
					IPulse* intermidiate = *negation + pulse;
					
					if (intermidiate->isNegative())
					{
						entries.push_back(intermidiate->negate());
					}

					delete negation;
					delete intermidiate;
				}

				if (entries.empty())
				{
					return new Graph::UnitPulse(CONFLICT, 0);
				}

				if (entries.size() == 1)
				{
					if (negative)
					{
						return entries.front();
					}
					else
					{
						return entries.front()->negate();
					}
				}

				return new AggregatedPulse(negative, variables, entries);
			}
			case Pulse:
			{
				if (pulse != *this)
				{
					return pulse + *this;
				}

				std::vector<int> variables, left_padding, right_padding;

				for (int i : this->variables)
				{
					bool present = false;
					variables.push_back(i);
					for (IUnit v : pulse.getVariables())
					{
						if (v.variable == i)
						{
							present = true;
						}
					}
					if (!present)
					{
						right_padding.push_back(i);
					}
				}

				for (IUnit v : pulse.getVariables())
				{
					bool present = false;
					for (int i : this->variables)
					{
						if (v.variable == i)
						{
							present = true;
						}
					}
					if (!present)
					{
						variables.push_back(v.variable);
						left_padding.push_back(v.variable);
					}
				}

				for (IPulse* entry : this->entries)
				{

				}
			}
		}
	}


	std::vector <Graph::UnitPulse*> AggregatedPulse::getUnits() 
	{
		return std::vector<Graph::UnitPulse*>();
	}

	std::vector <Graph::IPulse*> AggregatedPulse::getPulses() 
	{
		return entries;
	}

	std::vector<IUnit> AggregatedPulse::getVariables()
	{
		std::vector<IUnit> vec;

		for (int i : variables)
		{
			vec.push_back(IUnit(ANY, i));
		}

		return vec;
	}

	IPulse* AggregatedPulse::open()
	{
		std::vector<Graph::UnitPulse*> definitiveSet;

		for (int i : variables)
		{
			definitiveSet.push_back(new Graph::UnitPulse(ANY, i));
		}

		return new Graph::Pulse(false, std::vector<IPulse*>(), definitiveSet);
	}

	std::string AggregatedPulse::print()
	{
		std::string result;
		
		return result;
	}

	bool AggregatedPulse::isNegative()
	{
		return true;
	}

	Graph::IPulse* Graph::AggregatedPulse::negate()
	{
		return new AggregatedPulse(!negative, variables, entries);
	}
}