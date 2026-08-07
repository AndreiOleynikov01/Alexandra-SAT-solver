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

		Graph::IPulse* left_entry;
		if (!left_padding.empty())
		{
			Graph::Pulse left_padding_pulse(false, std::vector<IPulse*>(), left_padding);
			left_entry = *left.negate()+left_padding_pulse;
		}
		else
		{
			left_entry = left.negate();
		}

		Graph::IPulse* right_entry;
		if (!right_padding.empty())
		{
			Graph::Pulse right_padding_pulse(false, std::vector<IPulse*>(), right_padding);
			right_entry = *right.negate() + right_padding_pulse;
		}
		else
		{
			right_entry = right.negate();
		}



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
				bool found = false;
				Graph::UnitPulse* unit_pulse = dynamic_cast<Graph::UnitPulse*>(&pulse);
				if (unit_pulse->value == CONFLICT)
				{
					return unit_pulse;
				}

				for (int i : this->variables)
				{
					if (i != unit_pulse->variable)
					{
						variables.push_back(i);
					}
					else
					{
						found = true;
					}
				}

				if (!found)
				{
					std::vector<Graph::UnitPulse*>unit;
					unit.push_back(dynamic_cast<Graph::UnitPulse*>(&pulse));
					std::vector<Graph::IPulse*> pulse;
					pulse.push_back(this);
					return new Graph::Pulse(false, pulse, unit);
				}

				for (IPulse* entry : this->entries)
				{
					IPulse* negation = entry->negate();
					IPulse* intermidiate = *negation + pulse;
					IPulse* control_value = *entry + pulse;

					if (control_value->type == UnitPulse && dynamic_cast<Graph::UnitPulse*>(control_value)->value == CONFLICT)
					{
						entries.push_back(intermidiate->negate());
					}

					//delete negation;
					//delete intermidiate;
				}

				if (entries.empty())
				{
					return new Graph::UnitPulse(CONFLICT, 0);
				}

				if (entries.size() == 1)
				{
					if (negative)
					{
						return entries.front()->negate();
					}
					else
					{
						return entries.front();
					}
				}
				for (IPulse* entry : entries)
				{
				}
				return new AggregatedPulse(negative, variables, entries);
			}
			case Pulse:
			{
				if (pulse != *this || !pulse.isNegative())
				{
					return pulse + *this;
				}

				std::vector<Graph::UnitPulse*> left_padding, right_padding;

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
						right_padding.push_back(new Graph::UnitPulse(ANY, i));
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
						left_padding.push_back(new Graph::UnitPulse(ANY, v.variable));
					}
				}

				IPulse* positive_pulse = NULL;
				IPulse* intermidiate;

				for (IPulse* entry : this->entries)
				{
					if (negative)
					{
						intermidiate = *entry + pulse;
					}
					else
					{
						positive_pulse = pulse.negate();
						intermidiate = *entry + *positive_pulse;
					}

					if (intermidiate->type != UnitPulse)
					{
						entries.push_back(intermidiate);
					}
					/*else
					{
						delete intermidiate;
					}*/

					/*if (positive_pulse != NULL)
					{
						delete positive_pulse;
					}*/
				}

				if (entries.empty())
				{
					if (negative)
					{
						return new Graph::UnitPulse(CONFLICT, 0);
					}
					else
					{
						Graph::Pulse right_padding_pulse(false, std::vector<IPulse*>(), right_padding), left_padding_pulse(false, std::vector<IPulse*>(), left_padding);

						for (IPulse* p : this->entries)
						{
							if (!left_padding.empty())
							{
								entries.push_back(*p + left_padding_pulse);
							}
							else
							{
								entries.push_back(p);
							}
						}
						if (!right_padding.empty())
						{
							entries.push_back(*pulse.negate() + right_padding_pulse);
						}
						else
						{
							entries.push_back(pulse.negate());
						}

					}
				}
				else if (entries.size() == 1) 
				{
					if (negative)
					{
						return entries.front()->negate();
					}
					else
					{
						return entries.front();
					}
				}

				if (getWeight() >= std::pow(2, getVariables().size()))
				{
					if (negative)
					{
						return open();
					}
					else
					{
						return new Graph::UnitPulse(CONFLICT, 0);
					}
				}

				return new AggregatedPulse(negative, variables, entries);
			}
			case PulseType::AggregatedPulse:
			{
				if (pulse != *this)
				{
					std::vector<IPulse*> vec;
					vec.push_back(this);
					vec.push_back(&pulse);
					return new Graph::Pulse(false, vec);
				}

				std::vector<Graph::UnitPulse*> left_padding, right_padding;

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
						right_padding.push_back(new Graph::UnitPulse(ANY, i));
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
						left_padding.push_back(new Graph::UnitPulse(ANY, v.variable));
					}
				}

				std::vector<Graph::IPulse*> left_entries, right_entries;

				if (negative == pulse.isNegative())
				{
					for (IPulse* entry : this->entries)
					{
						left_entries.push_back(entry);
					}

					for (IPulse* entry : pulse.getPulses())
					{
						right_entries.push_back(entry);
					}

					for (IPulse* lp : left_entries)
					{
						IPulse* intermidiate;
						for (IPulse* rp : right_entries)
						{
							intermidiate = *lp + *rp;

							if (intermidiate->type == Pulse)
							{
								entries.push_back(intermidiate);
							}
							/*else
							{
								delete intermidiate;

							}*/
						}
					}
				}
				else
				{
					for (IPulse* entry : this->entries)
					{
						if (negative)
						{
							left_entries.push_back(entry);
						}
						else
						{
							right_entries.push_back(entry->negate());
						}
					}

					for (IPulse* entry : pulse.getPulses())
					{
						if (pulse.isNegative())
						{
							left_entries.push_back(entry);
						}
						else
						{
							right_entries.push_back(entry->negate());
						}
					}

					for (IPulse* lp : left_entries)
					{
						bool acceptable = false;
						IPulse* intermidiate;
						for (IPulse* rp : right_entries)
						{
							intermidiate = *lp + *rp;

							if (intermidiate->type != UnitPulse)
							{
								acceptable = true;
							}
							//delete intermidiate;
						}

						if (acceptable)
						{
							entries.push_back(lp);
						}
					}
				}

				if (entries.empty())
				{
					if (negative == pulse.isNegative())
					{
						if (negative)
						{
							return new Graph::UnitPulse(CONFLICT, 0);
						}
						else
						{
							Graph::Pulse right_padding_pulse(false, std::vector<IPulse*>(), right_padding), left_padding_pulse(false, std::vector<IPulse*>(), left_padding);

							for (IPulse* lp : left_entries)
							{
								if (!left_padding.empty())
								{
									entries.push_back(*lp + left_padding_pulse);
								}
								else
								{
									entries.push_back(lp);
								}
							}

							for (IPulse* rp : right_entries)
							{
								if (!left_padding.empty())
								{
									entries.push_back(*rp + right_padding_pulse);
								}
								else
								{
									entries.push_back(rp);
								}
							}
						}
					}
					else
					{
						return new Graph::UnitPulse(CONFLICT, 0);
					}
				}
				else if (entries.size() == 1)
				{
					if (negative && pulse.isNegative())
					{
						return entries.front()->negate();
					}
					else
					{
						return entries.front();
					}
				}


				if (getWeight() >= std::pow(2, getVariables().size()))
				{
					if (negative)
					{
						return open();
					}
					else
					{
						return new Graph::UnitPulse(CONFLICT, 0);
					}
				}

				return new AggregatedPulse(!(pulse.isNegative() && negative), variables, entries);
			}
		}
	}


	std::vector <Graph::IUnit*> AggregatedPulse::getUnits() 
	{
		return std::vector<Graph::IUnit*>();
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
		
		if (negative)
		{
			result += "Either [";
		}
		else
		{
			result += "Neither [";
		}

		for (int i = 0; i < entries.size(); i++)
		{
			result += entries[i]->print();
			if (i < entries.size() - 1)
			{
				result += ", ";
			}
		}
		result += "]";

		return result;
	}

	bool AggregatedPulse::isNegative()
	{
		return negative;
	}

	Graph::IPulse* Graph::AggregatedPulse::negate()
	{
		return new AggregatedPulse(!negative, variables, entries);
	}

	int AggregatedPulse::getWeight() 
	{ 
		int result = 0;
		for (IPulse* entry : entries)
		{
			result += entry->getWeight();
		}
		return (negative) ? (result) : (std::pow(2, getVariables().size()) - result) ;
	}
}