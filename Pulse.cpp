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
			default:
			{
				std::vector<Graph::UnitPulse*> left_unit_buffer;
				std::vector<Graph::UnitPulse*> right_unit_buffer;
				std::vector<Graph::IPulse*> left_pulse_buffer; 
				std::vector<Graph::IPulse*> right_pulse_buffer;

				if (pulse.type==PulseType::Pulse && pulse.isNegative() == negative)
				{
					left_unit_buffer = pulse.getUnits();
					left_pulse_buffer = pulse.getPulses();
				}
				else if(pulse.isNegative() || pulse.type == PulseType::AggregatedPulse)
				{
					left_pulse_buffer.push_back(&pulse);
				}
				else
				{
					return pulse + *this;
				}

				for (Graph::UnitPulse* u : units)
				{
					right_unit_buffer.push_back(u);
				}

				for (Graph::IPulse* p : pulses)
				{
					right_pulse_buffer.push_back(p);
				}

				for (int i = 0; i < right_unit_buffer.size(); i++)
				{
					bool present = false;
					if (right_unit_buffer[i] != NULL)
					{
						for (int j = 0; j < left_unit_buffer.size(); j++)
						{
							if (left_unit_buffer[j] != NULL && right_unit_buffer[i] == left_unit_buffer[j])
							{
								Graph::UnitPulse* intermidiate = dynamic_cast<Graph::UnitPulse*>(*right_unit_buffer[i] + *left_unit_buffer[j]);
								if (intermidiate->value == CONFLICT)
								{
									if (!negative && !pulse.isNegative())
									{
										return intermidiate;
									}
									else
									{
										//constructor for aggregates
									}
								}
								else
								{
									unit_buffer.push_back(intermidiate);
								}
								left_unit_buffer[j] = NULL;
								present = true;
							}
						}

						if (!present)
						{
							for (int j = 0; j < unit_buffer.size(); j++)
							{
								if (unit_buffer[j] == right_unit_buffer[i])
								{
									Graph::UnitPulse* intermidiate = dynamic_cast<Graph::UnitPulse*>(*right_unit_buffer[i] + *unit_buffer[j]);
									if (intermidiate->value == CONFLICT)
									{
										if (!negative && !pulse.isNegative())
										{
											return intermidiate;
										}
										else
										{
											//constructor for aggregates
										}
									}
									else
									{
										unit_buffer[j] = intermidiate;
									}
									present = true;
								}
							}

							if (!present)
							{
								for (int j = 0; j < pulse_buffer.size(); j++)
								{
									if (pulse_buffer[j] == left_unit_buffer[i])
									{
										Graph::IPulse* delete_register = pulse_buffer[j];
										Graph::IPulse* intermidiate = *right_unit_buffer[i] + *pulse_buffer[j];

										if (intermidiate->type == AggregatedPulse || intermidiate->isNegative())
										{
											pulse_buffer[j] = intermidiate;
										}
										else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
										{
											for (Graph::UnitPulse* unit : intermidiate->getUnits())
											{
												unit_buffer.push_back(unit);
											}
											for (Graph::IPulse* plain_pulse : intermidiate->getPulses())
											{
												pulse_buffer.push_back(plain_pulse);
											}
										}
										else
										{
											if (dynamic_cast<Graph::UnitPulse*>(intermidiate)->value == CONFLICT)
											{
												return intermidiate;
											}
											else
											{
												unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(intermidiate));
											}
										}

										delete delete_register;
										present = true;
										unit_buffer.push_back(right_unit_buffer[i]);
									}
								}

								if (!present)
								{
									for (int j = 0; j < left_pulse_buffer.size(); j++)
									{
										if (left_pulse_buffer[j] != NULL && right_unit_buffer[i] == left_pulse_buffer[j])
										{
											Graph::IPulse* intermidiate = *right_unit_buffer[i] + *left_pulse_buffer[j];

											if (intermidiate->type == AggregatedPulse || intermidiate->isNegative())
											{
												pulse_buffer.push_back(intermidiate);
											}
											else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
											{
												for (Graph::UnitPulse* unit : intermidiate->getUnits())
												{
													unit_buffer.push_back(unit);
												}
												for (Graph::IPulse* plain_pulse : intermidiate->getPulses())
												{
													pulse_buffer.push_back(plain_pulse);
												}
											}
											else
											{
												if (dynamic_cast<Graph::UnitPulse*>(intermidiate)->value == CONFLICT)
												{
													return intermidiate;
												}
												else
												{
													unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(intermidiate));
												}
											}

											left_pulse_buffer[j] = NULL;
											unit_buffer.push_back(right_unit_buffer[i]);
										}
									}
								}
							}
						}

						right_unit_buffer[i] = NULL;
					}
					//continue here: right pulse buffer

				}
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