#include "Alexandra.h"

namespace Graph
{
	Accumulator::AccNode::AccNode() : master(false), satisfied(false),  next_node(NULL), child_nodes(0), fold_count(0), value(NULL) {}

	void Accumulator::AccNode::add_child()
	{
		child_nodes++;
	}
	void Accumulator::AccNode::set_master()
	{
		master = true;
	}

	void Accumulator::AccNode::ripe(IPulse& pulse)
	{
		bool is_ripe;
		{
			std::lock_guard<std::mutex> synchronise(mutex);

			std::cout << pulse.print() << " is added";
			if (value != NULL)
				std::cout << " to " << value->print();
			std::cout << std::endl;

			if (value == NULL)
			{
				std::vector<Graph::IPulse*> pulses;
				pulses.push_back(&pulse);
				value = new Graph::Pulse(false, pulses);
			}
			else
			{
				IPulse* delete_buffer = value;

				if (satisfied)
				{
					IPulse* open_pulse = pulse.open();

					value = *value + *open_pulse;

					//delete open_pulse;
				}
				else
				{
					value = *value + pulse;

					if (value->type == IPulse::PulseType::UnitPulse)
					{
						satisfied = true;

						//delete value;

						IPulse* open_me = delete_buffer->open();
						IPulse* open_pulse = pulse.open();

						value = *open_me + *open_pulse;

						//delete open_me;
						//delete open_pulse;
					}
				}

				//delete delete_buffer;
			}

			fold_count++;
			is_ripe = fold_count == child_nodes;
		}

		std::cout << "new value is " << value->print() << std::endl;

		if (is_ripe)
		{
			Utilities::ThreadPool::make_thread(Fold(this));
		}
	}

	void Accumulator::AccNode::add_pulse(UnitPulse* signal)
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		std::cout << signal->print() << " is reported";

		if (value != NULL)
			std::cout << " to " << value->print();
		std::cout << std::endl;

		if (value == NULL)
		{
			value = signal;
		}
		else
		{
			IPulse* delete_buffer = value;

			if (satisfied)
			{
				value = *value + *signal->open();
			}
			else
			{
				value = *value + *signal;

				if (value->type == IPulse::PulseType::UnitPulse)
				{
					if (dynamic_cast<Graph::UnitPulse*>(value)->value == CONFLICT && !master)
					{
						satisfied = true;
						//delete value;

						IPulse* open_me = delete_buffer->open();
						IPulse* open_pulse = signal->open();

						value = *open_me + *open_pulse;

						//delete open_me;
						//delete open_pulse;
					}
				}
			}

			/*if (delete_buffer->type != IPulse::PulseType::UnitPulse)
			{
				delete delete_buffer;
			}*/
		}

		std::cout << "new value is " << value->print() <<std::endl;
	}

	void Accumulator::AccNode::add_node(AccNode* node)
	{
		next_node = node;
	}

	void Accumulator::AccNode::fold()
	{
		{
			std::lock_guard<std::mutex> synchronise(mutex);

			IPulse* intermidiate_value;

			if (!master)
			{
				intermidiate_value = value->negate();
			}
			else
			{
				intermidiate_value = value;
			}

			if (!master)
			{
				next_node->ripe(*intermidiate_value);
			}
		}
	}

	IPulse* Accumulator::AccNode::get_result()
	{
		return value;
	}

	Accumulator::Accumulator() : mutex(), accNodes(), master_pointer(NULL) {}

	Accumulator::AccNode* Accumulator::get_node(Utilities::Stack::Entry* id)
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		if (accNodes.contains(id))
		{
			return accNodes[id];
		}
		return NULL;
	}

	void Accumulator::add_node(Utilities::Stack::Entry* id, Accumulator::AccNode* node)
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		if (id->value == 0)
		{
			node->set_master();
			master_pointer = id;
		}

		accNodes[id] = node;
	}

	void Accumulator::accumulate(Utilities::Stack sat_trace, int variable, bool state)
	{
		UnitPulse* signal = new UnitPulse((state) ? (TRUE) : (FALSE), variable);

		AccNode* current_node;
		AccNode* prev_node = NULL;
		AccNode* leaf_node = NULL;

		Utilities::Stack::Entry* iterator = sat_trace.top_entry();

		bool complete = false;

		while (!complete && iterator != NULL)
		{
			current_node = get_node(iterator);
			if (current_node == NULL)
			{
				current_node = new AccNode();
				add_node(iterator, current_node);

			}

			if (leaf_node == NULL)
			{
				leaf_node = current_node;
				leaf_node->add_pulse(signal);
			}

			if (prev_node != NULL)
			{
				current_node->add_child();
				prev_node->add_node(current_node);
			}

			if (current_node->next_node == NULL)
			{
				prev_node = current_node;
			}
			else
			{
				complete = true;
			}

			iterator = iterator->prev;
		}
	}

	IPulse* Accumulator::solve()
	{
		for (auto p = accNodes.begin(); p != accNodes.end(); p++)
		{
			if (p->second->child_nodes == 0)
			{
				Utilities::ThreadPool::make_thread(Fold(p->second));
			}
		}
		Utilities::ThreadPool::wait_until_done();
		return accNodes[master_pointer]->get_result();
	}

	IPulse* Accumulator::solve(std::map<int, bool>& assumption)
	{
		std::vector<IPulse*> buffer;
		for (auto iter = assumption.begin(); iter != assumption.end(); iter++)
		{
			buffer.push_back(new UnitPulse((iter->second) ? (TRUE) : (FALSE), iter->first));
		}
		return (*solve() + *(new Pulse(false, buffer)));
	}


	Accumulator::Fold::Fold(AccNode* node) : node(node) {}

	void Accumulator::Fold::operator()()
	{
		node->fold();
	}
}