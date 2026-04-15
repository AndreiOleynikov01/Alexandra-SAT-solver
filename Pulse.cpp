#include "Alexandra.h"

namespace Graph
{
	bool Pulse::operator==(IPulse& pulse) 
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

	IPulse* Pulse::operator+(IPulse& pulse)
	{
		switch (pulse.type)
		{
		case PulseType::AggregatedPulse:
			return pulse + * this;
		case PulseType::UnitPulse:
		{
			std::vector<IPulse*> old_buffer, new_buffer;
			bool satisfied = false;
			for (IPulse* p : pulses)
			{
				if (*p == pulse)
				{
					IPulse* intermidiate = *p + pulse;
					if (*intermidiate == Graph::UnitPulse(CONFLICT, 0))
					{
						if (negative)
						{
							new_buffer.push_back(&pulse);
							satisfied = true;
						}
						else
						{
							return intermidiate;
						}
					}
					else
					{
						new_buffer.push_back(intermidiate);
					}
				}
				else
				{
					old_buffer.push_back(p);
				}
			}
			if (negative)
			{
				if (satisfied)
				{
					for (IPulse* p : old_buffer)
					{
						new_buffer.push_back(p->open());
					}
				}
				else
				{
					if (new_buffer.size() == 0)
					{
						new_buffer.push_back(&pulse);
					}
					new_buffer.push_back(new Pulse(true, old_buffer));
				}
				return new Pulse(false, new_buffer);
			}
			else
			{
				if (new_buffer.size() == 0)
				{
					new_buffer.push_back(&pulse);
				}
				for (IPulse* p : new_buffer)
				{
					if (p->type == PulseType::UnitPulse)
					{
						old_buffer.push_back(p);
					}
					else if (p->type == PulseType::Pulse && !p->isNegative())
					{
						std::vector<IPulse*> values = p->getvalues();
						for (IPulse* value : values)
						{
							old_buffer.push_back(value);
						}
					}
					else
					{
						old_buffer.push_back(p);
					}
				}
				return new Pulse(false, old_buffer);
			}
		}
		case PulseType::Pulse:
		{
			Pulse* ppulse = dynamic_cast<Graph::Pulse*>(&pulse);
			if (ppulse->negative == true && negative == true)
			{
				std::vector<IPulse*> common_buffer, left_buffer, right_buffer, new_buffer;
				for (IPulse* p : pulses)
				{
					if (*p == pulse)
					{
						common_buffer.push_back(p);
					}
					else
					{
						left_buffer.push_back(p);
					}
				}
				for (IPulse* rp : ppulse->pulses)
				{
					bool present = false;
					for (IPulse* lp : common_buffer)
					{
						if (*lp == *rp)
						{
							present = true;
							IPulse* intermidiate = (*lp + *rp);
							if (*intermidiate == Graph::UnitPulse(CONFLICT, 0))
							{
								delete intermidiate;
								return new Graph::AggregatedPulse(*this, *ppulse);
							}
							else
							{
								new_buffer.push_back(intermidiate);
								break;
							}
						}
					}
					if (!present)
					{
						right_buffer.push_back(rp);
					}
				}
				for (IPulse* p : left_buffer)
				{
					new_buffer.push_back(p);
				}
				for (IPulse* p : right_buffer)
				{
					new_buffer.push_back(p);
				}
				return new Pulse(true, new_buffer);
			}
			else if (ppulse->negative == false && negative == false)
			{
				std::vector<IPulse*> left_buffer, right_buffer, unit_buffer, pulse_buffer;
				std::vector<IPulse*> first_pointer_buffer, second_pointer_buffer;
				for (IPulse* lp : pulses)
				{
					bool equivalent = false;
					for (IPulse* rp : ppulse->pulses)
					{
						if (*lp == *rp)
						{
							equivalent = true;
							IPulse* intermidiate = *lp + *rp;
							if (*intermidiate == Graph::UnitPulse(CONFLICT, 0))
							{
								return intermidiate;
							}
							else
							{
								if (intermidiate->type == PulseType::UnitPulse)
								{
									unit_buffer.push_back(intermidiate);
								}
								else
								{
									first_pointer_buffer.push_back(intermidiate);
									second_pointer_buffer.push_back(intermidiate);
								}
							}
						}
					}
					if (!equivalent)
					{
						left_buffer.push_back(lp);
					}
				}
				for (IPulse* p : ppulse->pulses)
				{
					if (*p != *this)
					{
						right_buffer.push_back(p);
					}
				}
				for (int i = 0; i < first_pointer_buffer.size(); i++)
				{
					pulse_buffer.push_back(first_pointer_buffer[i]);
					for (int j = i + 1; j < first_pointer_buffer.size(); j++)
					{
						if (second_pointer_buffer[j] != NULL)
						{
							IPulse* temp = pulse_buffer.back();
							if (*second_pointer_buffer[j] == *temp)
							{
								pulse_buffer.pop_back();
								pulse_buffer.push_back(*second_pointer_buffer[j] + *temp);
								delete temp;
								second_pointer_buffer[j] = NULL;
							}
						}
					}
					for (IPulse* p : second_pointer_buffer)
					{
						if (p != NULL)
						{
							pulse_buffer.push_back(p);
						}
					}
				}
				std::vector<IPulse*> new_buffer;
				for (IPulse* u : unit_buffer)
				{
					new_buffer.push_back(u);
				}
				for (IPulse* p : pulse_buffer)
				{
					if (p->type == PulseType::UnitPulse)
					{
						return p;
					}
					else if (p->type == PulseType::AggregatedPulse)
					{
						new_buffer.push_back(p);
					}
					else
					{
						if (p->isNegative())
						{
							new_buffer.push_back(p);
						}
						else
						{
							for (IPulse* value : dynamic_cast<Graph::Pulse*>(p)->getvalues())
							{
								new_buffer.push_back(value);
							}
						}
					}
				}
				return new Pulse(false, new_buffer);
			}
			else if (ppulse->negative == true && negative == false)
			{
				if (*this != pulse)
				{
					std::vector<IPulse*> new_buffer;
					for (IPulse* p : pulses)
					{
						new_buffer.push_back(p);
					}
					new_buffer.push_back(&pulse);
					return new Pulse(false, new_buffer);
				}
				
				bool satisfied = false;
				std::vector<IPulse*> intermediate_buffer, new_buffer;
				for (IPulse* lp : pulses)
				{
					std::vector<IPulse*> equivalence_buffer;
					for (IPulse* rp : ppulse->pulses)
					{
						if (*lp == *rp)
						{
							equivalence_buffer.push_back(rp);
						}
					}
					IPulse* equivalent_pulse;
					if (equivalence_buffer.size() > 0)
					{
						if (equivalence_buffer.size() > 1)
						{
							equivalent_pulse = new Pulse(true, equivalence_buffer);
						}
						else
						{
							equivalent_pulse = equivalence_buffer.back();
						}
						IPulse* intermidiate_pulse = *lp + *equivalent_pulse;
						if (*intermidiate_pulse == Graph::UnitPulse(CONFLICT, 0))
						{
							intermediate_buffer.push_back(lp);
							satisfied = true;
							delete intermidiate_pulse;
						}
						else
						{
							intermediate_buffer.push_back(intermidiate_pulse);
						}
						delete equivalent_pulse;
					}
					else
					{
						new_buffer.push_back(lp);
					}
				}
				bool equivalent = true;
				std::vector<IPulse*> remaining_buffer;
				for (IPulse* p : ppulse->pulses)
				{
					if (*p != *this)
					{
						equivalent = false;
						remaining_buffer.push_back(p);
					}
				}
				if ((new_buffer.empty() || equivalent == true) && satisfied == false)
				{
					return new Graph::UnitPulse(CONFLICT, 0);
				}
				else
				{
					if (!remaining_buffer.empty())
					{
						IPulse* remaining = new Pulse(true, remaining_buffer);
						if (satisfied)
						{
							intermediate_buffer.push_back(remaining->open());
							delete remaining;
						}
						else
						{
							intermediate_buffer.push_back(remaining);
						}
					}
					for (IPulse* p : intermediate_buffer)
					{
						if (p->type == PulseType::Pulse && !p->isNegative())
						{
							for (IPulse* value : p->getvalues())
							{
								new_buffer.push_back(value);
							}
						}
						else
						{
							new_buffer.push_back(p);
						}
					}
					return new Pulse(false, new_buffer);
				}

			}
			else
			{
				return pulse + *this;
			}
		}
		}
	}

	Pulse::Pulse(bool negative) : IPulse(PulseType::Pulse), negative(negative), pulses() {}

	Pulse::Pulse(bool negative, std::vector<IPulse*> pulses) : IPulse(PulseType::Pulse), negative(negative), pulses(pulses){}

	IPulse* Pulse::open()
	{
		std::vector<IPulse*> result;
		for (IUnit u : getVariables())
		{
			result.push_back(new Graph::UnitPulse(ANY, u.variable));
		}
		return new Pulse(false, result);
	}
	
	std::vector <IPulse*> Pulse::getvalues() 
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

		for (int i = 0; i< pulses.size(); i++)
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
}