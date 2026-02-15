#pragma once
#include "dependancies.h"

namespace Graph
{
	enum State
	{
		TRUE, FALSE, CONFLICT
	};

	class Accumulator
	{
	public:
		void accumulate(std::stack<int>*, State);
	};

	class Node
	{
	public:
		void virtual propogate(std::stack<int>*);

		Node(int);
		
		const int me;
	};

	class NVariable : virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(std::stack<int>*);

		NVariable(int, Accumulator*);
	};

	class NAnd : virtual Node
	{
		Node* left;
		Node* right;
	public:
		void propagate(std::stack<int>*);

		NAnd(int, Node*, Node*);
	};

	class NNot : virtual Node
	{
		Node* fanin;
	public:
		void propagate(std::stack<int>*);

		NNot(int, Node*);
	};

	class NLatch : virtual Node
	{
		Node* fanin;
		const int next_state;
		Accumulator* accumulator;
	public:
		void propagate(std::stack<int>*);

		NLatch(int, int, Node*, Accumulator*);
	};

	class NodeHandler : virtual Node
	{
		Node* node;
	public:
		void propagate(std::stack<int>*);

		NodeHandler();

		NodeHandler(Node*);

		void set_node(Node*);
	};

	/*template<class N>
	class Concurrent :public N
	{
	private:
		std::mutex mutex;
	public:
		Concurrent(int me) : N(me) {}

		void propagate(std::stack<int>*)
		{
			std::lock
			N::propagate(std::stack<int>*);
		}
	};*/
}