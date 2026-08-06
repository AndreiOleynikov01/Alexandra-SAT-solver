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
		std::cout << "pulse added: " << pulse.print() << std::endl;
		std::vector<Graph::UnitPulse*> left_unit_buffer;
		std::vector<Graph::IPulse*> left_pulse_buffer;

		std::queue<Graph::UnitPulse*> right_unit_buffer;
		std::queue<Graph::IPulse*> right_pulse_buffer;

		bool satisfied = false;

		switch (pulse.type)
		{
		case PulseType::UnitPulse:
			left_unit_buffer.push_back(&dynamic_cast<Graph::UnitPulse&>(pulse));
			for (Graph::UnitPulse* u : units)
			{
				right_unit_buffer.push(u);
			}
			for (Graph::IPulse* p : pulses)
			{
				right_pulse_buffer.push(p);
			}
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
					right_unit_buffer.push(dynamic_cast<Graph::UnitPulse*>(u));
				}
				for (IPulse* p : pulse.getPulses())
				{
					right_pulse_buffer.push(p);
				}
			}
			else
			{
				right_pulse_buffer.push(&pulse);
			}

			for (Graph::UnitPulse* u : units)
			{
				left_unit_buffer.push_back(u);
			}
			left_pulse_buffer = getPulses();

			break;
		}
		case AggregatedPulse:
		{
			if (!negative)
			{
				right_pulse_buffer.push(&pulse);

				for (Graph::UnitPulse* u : units)
				{
					left_unit_buffer.push_back(u);
				}
				left_pulse_buffer = getPulses();
			}
			else
			{
				return pulse + *this;
			}
			break;
		}
		}

		for (auto left_iterator = left_unit_buffer.begin(); left_iterator != left_unit_buffer.end(); left_iterator++)
		{
			bool present = false;
			bool found = false;

			int size = right_unit_buffer.size();
			int count = 0;

			while (count < size)
			{
				if ((*left_iterator)->value == ANY)
				{
					present = true;
				}
				if ((*left_iterator)->variable == right_unit_buffer.front()->variable)
				{
					Graph::UnitPulse* intermidiate = dynamic_cast<Graph::UnitPulse*>(*(*left_iterator) + *right_unit_buffer.front());

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
								right_unit_buffer.push(intermidiate);
							}
							present = true;
						}
						else
						{
							right_unit_buffer.push(intermidiate);
						}
					}

					found = true;

					right_unit_buffer.pop();

					break;
				}
				else
				{
					right_unit_buffer.push(right_unit_buffer.front());
					right_unit_buffer.pop();
				}
				count++;
			}
			if (!found)
			{
				size = right_pulse_buffer.size();
				count = 0;
				while (count < size)
				{
					if (**left_iterator == *right_pulse_buffer.front())
					{
						if ((*left_iterator)->value == ANY)
						{
							present = true;
						}

						found = true;

						IPulse* intermidiate = *(*left_iterator) + *(right_pulse_buffer.front());

						switch (intermidiate->type)
						{
						case UnitPulse:
						{
							Graph::UnitPulse* unit = dynamic_cast<Graph::UnitPulse*>(intermidiate);
							if (negative && pulse.type == PulseType::Pulse)
							{
								if (negative)
								{
									return new Graph::AggregatedPulse(false, *this, dynamic_cast<Pulse&>(pulse));
								}
								return intermidiate;
							}
							right_unit_buffer.push(unit);
							break;
						}
						case PulseType::Pulse:
						{
							Graph::Pulse* pulse = dynamic_cast<Graph::Pulse*>(intermidiate);
							if (!pulse->isNegative())
							{
								for (IUnit* u : pulse->getUnits())
								{
									right_unit_buffer.push(dynamic_cast<Graph::UnitPulse*>(u));
								}
								for (IPulse* p : pulse->getPulses())
								{
									right_pulse_buffer.push(p);
								}
							}
							else
							{
								right_pulse_buffer.push(intermidiate);
							}
							break;
						}
						case AggregatedPulse:
						{
							right_pulse_buffer.push(intermidiate);
						}
						}
						right_pulse_buffer.pop();
						break;
					}
					else
					{
						right_pulse_buffer.push(right_pulse_buffer.front());
						right_pulse_buffer.pop();
					}
					count++;
				}
			}

			if (!found && pulse.type == UnitPulse && negative)
			{
				left_pulse_buffer.push_back(this);
				
				return new Pulse(false, left_pulse_buffer, left_unit_buffer);
			}

			if (!present)
			{
				right_unit_buffer.push(*left_iterator);
			}
		}

		for (auto left_iterator = left_pulse_buffer.begin(); left_iterator != left_pulse_buffer.end(); left_iterator++)
		{
			IPulse* intermidiate = *left_iterator;

			int size = right_unit_buffer.size();
			int count = 0;

			while (count < size)
			{
				if (**left_iterator == *right_unit_buffer.front())
				{
					intermidiate = *intermidiate + *right_unit_buffer.front();
					right_unit_buffer.pop();
				}
				count++;
			}

			size = right_pulse_buffer.size();
			count = 0;

			while (count < size)
			{
				if (**left_iterator == *right_pulse_buffer.front())
				{
					intermidiate = *intermidiate + *right_pulse_buffer.front();
					right_pulse_buffer.pop();
				}
				count++;
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
					right_unit_buffer.push(unit);
				}
			}
			else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
			{
				for (IUnit* u : intermidiate->getUnits())
				{
					right_unit_buffer.push(dynamic_cast<Graph::UnitPulse*>(u));
				}
				for (IPulse* p : intermidiate->getPulses())
				{
					right_pulse_buffer.push(p);
				}
			}
			else
			{
				right_pulse_buffer.push(intermidiate);
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
			std::vector<Graph::UnitPulse*> unit_vec;
			std::vector<IPulse*> pulse_vec;

			while (!right_unit_buffer.empty())
			{
				unit_vec.push_back(right_unit_buffer.front());
				right_unit_buffer.pop();
			}
			while (!right_pulse_buffer.empty())
			{
				pulse_vec.push_back(right_pulse_buffer.front());
				right_pulse_buffer.pop();
			}

			
			if (satisfied)
			{
				IPulse return_value = Pulse(negative, pulse_vec, unit_vec);
				return return_value.open();
			}
			else
			{
				return new Pulse(negative, pulse_vec, unit_vec);
			}
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