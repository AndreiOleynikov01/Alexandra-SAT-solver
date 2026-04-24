#include "Alexandra.h"

namespace Graph
{
	Accumulator::AccNode::AccNode() : master(false), signals(), next_node(NULL), child_nodes(0), fold_count(0), intermidiate_value() {}

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

			if (intermidiate_value == NULL)
			{
				intermidiate_value = new Pulse(!master, signals);
			}

			IPulse* new_value = *intermidiate_value + pulse;
			//delete intermidiate_value;
			intermidiate_value = new_value;
			fold_count++;
			is_ripe = fold_count == child_nodes;
		}

		std::cout << "intermidiate value at parent: " << intermidiate_value->print() << std::endl;

		if (is_ripe)
		{
			Utilities::ThreadPool::make_thread(Fold(this));
		}
	}

	void Accumulator::AccNode::add_pulse(UnitPulse* signal)
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		bool ignore = false;
		if (signal->variable != 0 && signal->variable != 1)
		{
			for (IPulse* s : signals)
			{
				UnitPulse* up = dynamic_cast<UnitPulse*>(s);
				if ((up->value == CONFLICT) || (up ->variable == signal->variable && up->value == signal->value))
				{
					ignore = true;
				} 
				else if (up->variable == signal->variable && up->value != signal->value)
				{
					signals.erase(signals.begin());
					signals.push_back(new UnitPulse(CONFLICT, 0));
					ignore = true;
				}
			}
		}
		else 
		{
			if (!master)
			{
				ignore = true;
			}
			else
			{
				if ((signal->variable == 0 && signal->value == TRUE))
				{
					signals.erase(signals.begin());
					signals.push_back(new UnitPulse(CONFLICT, 0));
				}
				ignore = true;
			}
		}

		if (ignore)
		{
			delete signal;
		}
		else
		{
			signals.push_back(signal);
		}
	}

	void Accumulator::AccNode::add_node(AccNode* node)
	{
		next_node = node;
	}

	void Accumulator::AccNode::fold()
	{
		{
			std::cout << "starting folding at master: " << master << std::endl;
			std::lock_guard<std::mutex> synchronise(mutex);
			
			if (intermidiate_value == NULL)
			{
				if (signals.size() == 1)
				{
					intermidiate_value = signals.front();
				}
				else
				{
					intermidiate_value = new Pulse(!master, signals);
				}
			}

			std::cout << "intermidiate value: " << intermidiate_value->print() << std::endl;

			if (!master)
			{
				next_node->ripe(*intermidiate_value);
			}
		}
	}

	IPulse* Accumulator::AccNode::get_result()
	{
		return intermidiate_value;
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


	Accumulator::Fold::Fold(AccNode* node) : node(node){}

	void Accumulator::Fold::operator()()
	{
		node->fold();
	}
}