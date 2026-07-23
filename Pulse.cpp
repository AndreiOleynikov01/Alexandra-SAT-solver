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

		switch (pulse.type)
		{
			case PulseType::UnitPulse:
			{
				Graph::UnitPulse& unit_pulse = dynamic_cast<Graph::UnitPulse&>(pulse);

				if (unit_pulse.value == CONFLICT)
				{
					return &pulse;
				}

				bool satisfied = false;
				bool present = false;

				for (Graph::UnitPulse* u : units)
				{
					if (u->variable != unit_pulse.variable)
					{
						unit_buffer.push_back(u);
					}
					else
					{
						present = true;

						if (u->value == unit_pulse.value || unit_pulse.value ==ANY)
						{
							if (!negative)
							{
								unit_buffer.push_back(u);
							}
						}
						else if (u->value == ANY)
						{
							unit_buffer.push_back(&unit_pulse);
						}
						else
						{
							if (!negative)
							{
								return new Graph::UnitPulse(CONFLICT, 0);
							}
							else
							{
								satisfied = true;
							}
						}
					}
				}

				if (unit_buffer.empty() && pulses.empty())
				{
					return new Graph::UnitPulse(CONFLICT, 0);
				}

				for (IPulse* p : pulses)
				{
					if (!p->operator==(pulse))
					{
						pulse_buffer.push_back(p);
					}
					else
					{
						if (unit_pulse.value == ANY)
						{
							present = true;
						}

						IPulse* intermidiate = p->operator+(unit_pulse);
						if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
						{
							for (Graph::UnitPulse* u : intermidiate->getUnits())
							{
								unit_buffer.push_back(u);
							}
							for (Graph::IPulse* p : intermidiate->getPulses())
							{
								pulse_buffer.push_back(p);
							}
							delete intermidiate;
						}
						else if (intermidiate->type == PulseType::UnitPulse)
						{
							Graph::UnitPulse* unit = dynamic_cast<Graph::UnitPulse*>(intermidiate);
							if (unit->value == CONFLICT)
							{
								return intermidiate;
							}

							unit_buffer.push_back(unit);
						}
						else
						{
							pulse_buffer.push_back(intermidiate);
						}
					}
				}

				if (!present)
				{
					unit_buffer.push_back(&unit_pulse);
				}

				if (satisfied)
				{
					Pulse result = Pulse(negative, pulse_buffer, unit_buffer);
					return result.open();
				}

				return new Pulse(negative, pulse_buffer, unit_buffer);
			}
			case PulseType::Pulse:
			{
				Graph::Pulse& plain_pulse = dynamic_cast<Graph::Pulse&>(pulse);

				std::vector<Graph::UnitPulse*> left_unit_buffer;
				std::vector<Graph::IPulse*> left_pulse_buffer;

				if (plain_pulse.isNegative() == negative)
				{
					left_unit_buffer = plain_pulse.getUnits();
					left_pulse_buffer = plain_pulse.getPulses();
				}
				else if(plain_pulse.isNegative())
				{
					left_pulse_buffer.push_back(&plain_pulse);
				}
				else
				{
					return plain_pulse + *this;
				}



				break;
			}
			case PulseType::AggregatedPulse:
			{
				Graph::AggregatedPulse& aggregate_pulse = dynamic_cast<Graph::AggregatedPulse&>(pulse);

			}
		}
	}

	Pulse::Pulse(bool negative) : IPulse(PulseType::Pulse), negative(negative), units(), pulses() {}

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


	std::vector <Graph::UnitPulse*> Pulse::getUnits() 
	{
		return units;
	}

	std::vector <Graph::IPulse*> Pulse::getPulses() 
	{
		return pulses;
	}


	std::vector<IUnit> Pulse::getVariables()
	{
		std::vector<IUnit> vec;
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
			result = "Not (";
		}

		for (int i = 0; i < pulses.size(); i++)
		{
			result += pulses.at(i)->print();

			if (i < pulses.size() - 1)
			{
				result += ", ";
			}
		}

		if (negative)
		{
			result += ")";
		}

		return result;
	}

	bool Pulse::isNegative()
	{
		return negative;
	}

	Graph::IPulse* Pulse::negate()
	{
		return new Pulse(!isNegative, pulses, units);
	}
}