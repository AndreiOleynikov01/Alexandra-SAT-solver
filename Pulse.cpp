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
							for (Graph::IUnit* u : intermidiate->getUnits())
							{
								unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(u));
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

				if (unit_buffer.size() == 1 && pulse_buffer.size() == 0)
				{
					return unit_buffer[0]->negate();
				}

				if (unit_buffer.size() == 0 && pulse_buffer.size() == 1)
				{
					return pulse_buffer[0]->negate();
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
					for (IUnit* u : pulse.getUnits())
					{
						left_unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(u));
					}
					left_pulse_buffer = pulse.getPulses();
				}
				else if(!negative)
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
							if (left_unit_buffer[j] != NULL && *dynamic_cast<IPulse*>(right_unit_buffer[i]) == *left_unit_buffer[j])
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
										return new Graph::AggregatedPulse(false, *dynamic_cast<Pulse*>(this), dynamic_cast<Pulse&>(pulse));
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
								if (*dynamic_cast<IPulse*>(unit_buffer[j]) == *right_unit_buffer[i])
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
											return new Graph::AggregatedPulse(false, *dynamic_cast<Pulse*>(this), dynamic_cast<Pulse&>(pulse));
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
									if (*pulse_buffer[j] == *right_unit_buffer[i])
									{
										Graph::IPulse* delete_register = pulse_buffer[j];
										Graph::IPulse* intermidiate = *right_unit_buffer[i] + *pulse_buffer[j];

										if (intermidiate->type == AggregatedPulse || intermidiate->isNegative())
										{
											pulse_buffer[j] = intermidiate;
										}
										else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
										{
											for (Graph::IUnit* unit : intermidiate->getUnits())
											{
												unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(unit));
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

										//delete delete_register;
										present = true;
										if (right_unit_buffer[i]->value != ANY)
										{
											unit_buffer.push_back(right_unit_buffer[i]);
										}
									}
								}

								if (!present)
								{
									for (int j = 0; j < left_pulse_buffer.size(); j++)
									{
										if (left_pulse_buffer[j] != NULL && *right_unit_buffer[i] == *left_pulse_buffer[j])
										{
											Graph::IPulse* intermidiate = *right_unit_buffer[i] + *left_pulse_buffer[j];

											if (intermidiate->type == AggregatedPulse || intermidiate->isNegative())
											{
												pulse_buffer.push_back(intermidiate);
											}
											else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
											{
												for (Graph::IUnit* unit : intermidiate->getUnits())
												{
													unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(unit));
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
											if (right_unit_buffer[i]->value != ANY)
											{
												unit_buffer.push_back(right_unit_buffer[i]);
											}
											present = true;
										}
									}

									if (!present)
									{
										unit_buffer.push_back(right_unit_buffer[i]);
									}
								}
							}
						}

						right_unit_buffer[i] = NULL;
					}
					
					for (int i = 0; i < right_pulse_buffer.size(); i++)
					{
						bool present = false;
						Graph::IPulse* intermidiate = right_pulse_buffer[i];
						Graph::IPulse* delete_buffer = NULL;

						if (right_pulse_buffer[i] != NULL)
						{
							for (int j = 0; j < left_unit_buffer.size(); j++)
							{
								if (left_unit_buffer[j] != NULL && *intermidiate == *left_unit_buffer[j])
								{
									intermidiate = *intermidiate + *left_unit_buffer[j];
									if (delete_buffer != NULL)
									{
										//delete intermidiate;
									}
									delete_buffer = intermidiate;
									present = true;
									left_unit_buffer[j] = NULL;
								}
							}

							for (int j = 0; j < unit_buffer.size(); j++)
							{
								if (*unit_buffer[j] == *intermidiate)
								{
									intermidiate = *intermidiate + *unit_buffer[j];
									/*if (delete_buffer != NULL)
									{
										delete delete_buffer;
									}*/
									delete_buffer = intermidiate;
									present = true;
									if (unit_buffer[j]->value == ANY)
									{
										unit_buffer.erase(std::next(unit_buffer.begin(), j));
									}
								}
							}

							for (int j = 0; j < pulse_buffer.size(); j++)
							{
								if (*pulse_buffer[j] == *intermidiate)
								{
									intermidiate = *intermidiate + *pulse_buffer[j];
									/*if (delete_buffer != NULL)
									{
										delete delete_buffer;
									}*/

									pulse_buffer.erase(std::next(pulse_buffer.begin(), j));
									delete_buffer = intermidiate;
									present = true;
								}
							}

							for (int j = 0; j < left_pulse_buffer.size(); j++)
							{
								if (left_pulse_buffer[j] != NULL && *left_pulse_buffer[j] == *right_pulse_buffer[i])
								{
									intermidiate = *intermidiate + *left_pulse_buffer[j];
									/*if (delete_buffer != NULL)
									{
										delete delete_buffer;
									}*/

									left_pulse_buffer[j] = NULL;
									delete_buffer = intermidiate;
									present = true;
								}
							}

							if (intermidiate->type == PulseType::UnitPulse)
							{
								if (dynamic_cast<Graph::UnitPulse*>(intermidiate)->value == CONFLICT)
								{
									if (negative && pulse.isNegative())
									{
										return new Graph::AggregatedPulse(false, *dynamic_cast<Pulse*>(this), dynamic_cast<Pulse&>(pulse));
									}
									else
									{
										return intermidiate;
									}
								}
								else
								{
									unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(intermidiate));
								}
							}
							else if (intermidiate->type == PulseType::Pulse && !intermidiate->isNegative())
							{
								for (Graph::IUnit* u : intermidiate->getUnits())
								{
									unit_buffer.push_back(dynamic_cast<Graph::UnitPulse*>(u));
								}
								
								for (Graph::IPulse* p : intermidiate->getPulses())
								{
									pulse_buffer.push_back(p);
								}
							}
							else
							{
								pulse_buffer.push_back(intermidiate);
							}
						}
					}

					right_unit_buffer[i] = NULL;
				}

				for (int i = 0; i < left_unit_buffer.size(); i++)
				{
					if (left_unit_buffer[i] != NULL)
					{
						unit_buffer.push_back(left_unit_buffer[i]);
					}
				}

				for (int i = 0; i < left_pulse_buffer.size(); i++)
				{
					if (left_pulse_buffer[i] != NULL)
					{
						pulse_buffer.push_back(left_pulse_buffer[i]);
					}
				}

				if (unit_buffer.size() == 1 && pulse_buffer.size() == 0)
				{
					return unit_buffer[0]->negate();
				}

				if (unit_buffer.size() == 0 && pulse_buffer.size() == 1)
				{
					return pulse_buffer[0]->negate();
				}

				return new Graph::Pulse(negative, pulse_buffer, unit_buffer);
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


	std::vector <Graph::IUnit*> Pulse::getUnits() 
	{
		std::vector<IUnit*> units;

		for (Graph::UnitPulse* unit : this->units)
		{
			units.push_back(unit);
		}

		return units;
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
			result = "Not (";
		}

		for (int i = 0; i < pulses.size(); i++)
		{
			result += pulses[i]->print();

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
		return new Pulse(!negative, pulses, units);
	}
}