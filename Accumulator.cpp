#include "Alexandra.h"

namespace Graph
{
	Accumulator::AccNode::AccNode() : master(false), signals(), next_node(NULL), child_nodes(0), fold_count(0), intermidiate_value(){}

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

			intermidiate_value = *intermidiate_value + pulse;
			fold_count++;
			is_ripe = fold_count == child_nodes;
		}
		
		if (is_ripe)
		{
			Utilities::ThreadPool::make_thread(Fold(next_node));
		}
	}

	void Accumulator::AccNode::add_pulse(UnitPulse* signal) 
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		
	}

	void Accumulator::AccNode::add_node(AccNode* node)
	{
		next_node = node;
	}

	void Accumulator::AccNode::fold()
	{
		std::cout << "starting folding at master: " << master<< std::endl;
		std::lock_guard<std::mutex> synchronise(mutex);

		
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

		accNodes[id] = node;
	}

	void Accumulator::accumulate(Utilities::Stack sat_trace, int variable, bool state) 
	{
		
	}

	IPulse* Accumulator::solve()
	{
		
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