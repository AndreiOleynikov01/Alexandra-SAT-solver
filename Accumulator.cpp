#include "Alexandra.h"

namespace Graph
{
	Accumulator::AccNode::AccNode(bool is_master) : master(is_master), signals(), nodes(), mutex() {}

	void Accumulator::AccNode::add_pulse(UnitPulse* signal) 
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		if (signal->value == CONFLICT)
		{
			signals.erase(signals.begin(), signals.end());
			signals.push_back(signal);
		}
		else if (signal->variable == 0 && signal->value == TRUE)
		{
			if (master)
			{
				signals.erase(signals.begin(), signals.end());
				signals.push_back(new UnitPulse(CONFLICT, 0));
			}
		}
		else
		{
			for (IPulse* s : signals)
			{
				if ((s->toUnitPulse()->variable != signal->variable && s->toUnitPulse()->value != signal->value) && s->toUnitPulse()->value != CONFLICT)
				{
					signals.push_back(signal);
				}
				else
				{
					signals.erase(signals.begin(), signals.end());
					signals.push_back(new UnitPulse(CONFLICT, 0));
				}
			}
		}
	}

	void Accumulator::AccNode::add_node(AccNode* node)
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		nodes.push_back(node);
	}

	IPulse* Accumulator::AccNode::fold()
	{
		if (nodes.empty())
		{
			return new Pulse(master, signals);
		}
		else
		{
			IPulse* result;
			Add* new_adder = new Add(signals.size() + 1, NULL, result);
			for (AccNode* n : nodes)
			{
				Utilities::ThreadPool::make_thread(Fold(new_adder, n));
			}
			new_adder->add_operand(new Pulse(master, signals));
			Utilities::ThreadPool::make_thread(*new_adder);
			Utilities::ThreadPool::wait_until_done();
			return result;
		}
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
		AccNode* prev_node = NULL;
		while (!sat_trace.empty())
		{
			AccNode* node;
			Utilities::Stack::Entry* current_not = sat_trace.pop();
			if (prev_node == NULL)
			{
				node = get_node(current_not);
				if (node == NULL)
				{
					node = new AccNode(current_not->value == 0);
					add_node(current_not, node);
				}

				
				node->add_pulse(new UnitPulse((state)?(TRUE):(FALSE), variable));
			}
			else
			{
				node = get_node(current_not);
				if (node == NULL)
				{
					node = new AccNode(current_not->value == 0);
					add_node(current_not, node);
					node->add_node(prev_node);
				}
			}
			prev_node = node;
			if (master_pointer == NULL && current_not->value == 0)
			{
				master_pointer = current_not;
			}
		}
	}

	IPulse* Accumulator::solve()
	{
		return get_node(master_pointer)->fold();
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

	Accumulator::Add::Add(int size, Add* prev, IPulse* result) : mutex(), cv(), number_of_operands(size), adder(prev), result_buffer(&result), operands() {}

	void Accumulator::Add::add_operand(IPulse* pulse)
	{
		std::lock_guard<std::mutex> synchronise(mutex);

		operands.push_back(pulse);
	}

	void Accumulator::Add::operator()()
	{
		std::unique_lock<std::mutex> lock(mutex);

		if (operands.size() < number_of_operands)
		{
			if (Utilities::ThreadPool::thread_count > 0)
			{
				lock.unlock();
				Utilities::ThreadPool::make_thread(*this);
				return;
			}
			else
			{
				struct Wait
				{
					const int size;
					std::vector<IPulse*>& vec;
					Wait(int size, std::vector<IPulse*>& vec) : size(size), vec(vec){}
					bool operator()()
					{
						return size == vec.size();
					}
				};
				
				cv.wait(lock, Wait(number_of_operands, operands));
			}
		}
			IPulse* garbage = NULL;
			IPulse* result = NULL;
			for (IPulse* p : operands)
			{
				if (result == NULL)
				{
					result = p;
				}
				else
				{
					IPulse* intermideate = *p + *result;
					delete garbage;
					result = intermideate;
					garbage = intermideate;
				}
			}
			lock.unlock();
			*result_buffer = result;
	}

	Accumulator::Fold::Fold(Add* adder, AccNode* node) : adder(adder), node(node){}

	void Accumulator::Fold::operator()()
	{
		if (node->nodes.empty())
		{
			adder->add_operand(new Pulse(node->master, node->signals));
		}
		else
		{
			Add* new_adder = new Add(node->signals.size() + 1, adder);
			for (AccNode* n : node->nodes)
			{
				Utilities::ThreadPool::make_thread(Fold(new_adder, n));
			}
			new_adder->add_operand(new Pulse(node->master, node->signals));
			Utilities::ThreadPool::make_thread(*new_adder);
		}
	}
}