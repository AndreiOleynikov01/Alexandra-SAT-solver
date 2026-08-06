#include "Alexandra.h"

namespace Graph
{
	bool Pulse::operator==(IPulse& pulse)
	{
		std::vector<IUnit> target_variables = pulse.getVariables();
		std::vector<IUnit> variables = getVariables();
		for (IUnit unit : variables)
		{
			for (IUnit target_unit : target_variables)
			{
				if (unit.variable == target_unit.variable)
				{
					return true;
				}
			}
		}
		return false;
	}

	IPulse* Pulse::operator+(IPulse& pulse)
	{
		std::vector<Graph::UnitPulse*> unit_buffer;
		std::vector<Graph::IPulse*> pulse_buffer;
		std::vector<Graph::UnitPulse*> left_unit_buffer;
		std::vector<Graph::IPulse*> left_pulse_buffer;
		std::vector<Graph::UnitPulse*> right_unit_buffer;
		std::vector<Graph::IPulse*> right_pulse_buffer;

		bool satisfied;

		switch (pulse.type)
		{
		case PulseType::UnitPulse:
			right_unit_buffer.push_back(&dynamic_cast<Graph::UnitPulse&>(pulse));
			break;
		case PulseType::Pulse:
		{
			if (negative && !pulse.isNegative())
			{
				return pulse + *this;
			}

			if (negative == pulse.isNegative())
			{
				for (IUnit* u : pulse.getUnits())
				{
					right_unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(u));
				}
				right_pulse_buffer = pulse.getPulses();
			}
			else
			{
				right_pulse_buffer.push_back(&pulse);
			}
			break;
		}
		case AggregatedPulse:
		{
			if (!negative)
			{
				right_pulse_buffer.push_back(&pulse);
			}
			else
			{
				return pulse + *this;
			}
			break;
		}
		}

		for (Graph::UnitPulse* u : units)
		{
			left_unit_buffer.push_back(u);
		}
		left_pulse_buffer = getPulses();

		for (auto left_iterator = left_unit_buffer.begin(); left_iterator != left_unit_buffer.end(); left_iterator++)
		{
			bool present = false;
			bool found = false;

			for (auto right_iterator = right_unit_buffer.begin(); right_iterator != right_unit_buffer.end(); right_iterator++)
			{
				if ((*left_iterator)->value == ANY)
				{
					present = true;
				}
				if ((*left_iterator)->variable == (*right_iterator)->variable)
				{
					Graph::UnitPulse* intermidiate = dynamic_cast<Graph::UnitPulse*>(*(*left_iterator) + *(*right_iterator));

					if (intermidiate->value == CONFLICT)
					{
						if (negative)
						{
							if (pulse.type == UnitPulse)
							{
								satisfied = true;
								present = true;
							}
							else
							{
								return new Graph::AggregatedPulse(false, *this, dynamic_cast<Pulse&>(pulse));
							}
						}
						else
						{
							return intermidiate;
						}
					}
					else
					{
						if (negative)
						{
							if (!(*left_iterator)->value == ANY)
							{
								unit_buffer.push_back(intermidiate);
							}
							present = true;
						}
						else
						{
							unit_buffer.push_back(intermidiate);
						}
					}

					found = true;

					right_unit_buffer.erase(right_iterator);

					break;
				}
			}
			if (!found)
			{
				for (auto right_iterator = right_pulse_buffer.begin(); right_iterator != right_pulse_buffer.end(); right_iterator++)
				{
					if (**left_iterator == **right_iterator)
					{
						if ((*left_iterator)->value == ANY)
						{
							present = true;
						}

						found = true;

						IPulse* intermidiate = *(*left_iterator) + *(*right_iterator);

						switch (intermidiate->type)
						{
						case UnitPulse:
						{
							Graph::UnitPulse* unit = dynamic_cast<Graph::UnitPulse*>(intermidiate);
							if (unit->value == CONFLICT)
							{
								return intermidiate;
							}
							right_unit_buffer.push_back(unit);
							break;
						}
						case PulseType::Pulse:
						{
							Graph::Pulse* pulse = dynamic_cast<Graph::Pulse*>(intermidiate);
							if (!pulse->isNegative())
							{
								for (IUnit* u : pulse->getUnits())
								{
									right_unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(u));
								}
								for (IPulse* p : pulse->getPulses())
								{
									right_pulse_buffer.push_back(p);
								}
							}
							else
							{
								right_pulse_buffer.push_back(intermidiate);
							}
							break;
						}
						case AggregatedPulse:
						{
							right_pulse_buffer.push_back(intermidiate);
						}
						}
						right_pulse_buffer.erase(right_iterator);
						break;
					}
				}
			}

			if (!present)
			{
				unit_buffer.push_back(*left_iterator);
			}

		}

		for (auto left_iterator = left_pulse_buffer.begin(); left_iterator != left_pulse_buffer.end(); left_iterator++)
		{
			IPulse* intermidiate = *left_iterator;
			auto right_unit_iterator = right_unit_buffer.begin();
			while (right_unit_iterator != right_unit_buffer.end())
			{
				if (**left_iterator == **right_unit_iterator)
				{
					intermidiate = *intermidiate + **right_unit_iterator;
					right_unit_buffer.erase(right_unit_iterator);
				}
				else
				{
					right_unit_iterator++;
				}
			}
			auto right_pulse_iterator = right_pulse_buffer.begin();
			while (right_pulse_iterator != right_pulse_buffer.end())
			{
				if (**left_iterator == **right_pulse_iterator)
				{
					intermidiate = *intermidiate + **right_pulse_iterator;
					right_pulse_buffer.erase(right_pulse_iterator);
				}
				else
				{
					right_pulse_iterator++;
				}
			}
			if (intermidiate->type == UnitPulse)
			{
				Graph::UnitPulse* unit = dynamic_cast<Graph::UnitPulse*>(intermidiate);
				if (unit->value == CONFLICT)
				{
					return intermidiate;
				}
				else
				{
					right_unit_buffer.push_back(unit);
				}
			}
			else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
			{
				for (IUnit* u : intermidiate->getUnits())
				{
					right_unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(u));
				}
				for (IPulse* p : intermidiate->getPulses())
				{
					right_pulse_buffer.push_back(p);
				}
			}
			else
			{
				right_pulse_buffer.push_back(intermidiate);
			}
		}

		IPulse* return_value;

		if (right_unit_buffer.empty() && right_pulse_buffer.size() == 1)
		{
			return_value = right_pulse_buffer.front();
		}
		else if (right_unit_buffer.size() == 1 && right_pulse_buffer.empty())
		{
			return_value = right_unit_buffer.front();
		}
		else
		{
			return new Pulse(negative, right_pulse_buffer, right_unit_buffer);
		}

		if (negative)
		{
			return return_value->negate();
		}
		else
		{
			return return_value;
		}
	}

	Pulse::Pulse(bool negative,  std::vector<IPulse*> pulses, std::vector<Graph::UnitPulse*> units) : IPulse(PulseType::Pulse), negative(negative), units(units), pulses(pulses) {}

	IPulse* Pulse::open()
	{
		std::vector<IPulse*> result;
		for (IUnit u : getVariables())
		{
			result.push_back(new Graph::UnitPulse(ANY, u.variable));
		}
		return new Pulse(false, result);
	}


	std::vector <Graph::IUnit*> Pulse::getUnits() 
	{
		std::vector<IUnit*> unit_buffer;
		for (Graph::UnitPulse* unit : units)
		{
			unit_buffer.push_back(unit);
		}

		return unit_buffer;
	}

	std::vector <Graph::IPulse*> Pulse::getPulses() 
	{
		return pulses;
	}


	std::vector<IUnit> Pulse::getVariables()
	{
		std::vector<IUnit> vec;
		for (Graph::UnitPulse* unit : units)
		{
			vec.push_back(unit);
		}
		for (IPulse* pulse : pulses)
		{
			for (IUnit unit : pulse->getVariables())
			{
				vec.push_back(unit);
			}
		}
		return vec;
	}

	std::string Pulse::print()
	{
		std::string result;
		if (negative)
		{
			result = "Not ";
		}

		result += "(";

		for (int i = 0; i < units.size(); i++)
		{
			result += units[i]->print(); 
			if (i < units.size() - 1 || !pulses.empty())
			{
				result += ", ";
			}
		}

		for (int i = 0; i < pulses.size(); i++)
		{
			result += pulses[i]->print();

			if (i < pulses.size() - 1)
			{
				result += ", ";
			}
		}

		result += ")";
		

		return result;
	}

	bool Pulse::isNegative()
	{
		return negative;
	}

	Graph::IPulse* Pulse::negate()
	{
		return new Pulse(!negative, pulses, units);
	}
}