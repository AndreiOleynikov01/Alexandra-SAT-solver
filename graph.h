#pragma once
#include "utilities.h"

namespace Graph
{
	enum State
	{
		TRUE, FALSE, CONFLICT
	};

	class Accumulator
	{
		std::map<Utilities::Stack::Entry*, IPulse&> pulses;
	public:
		Accumulator();
		void accumulate(Utilities::Stack, int);
		IPulse& solve();
		IPulse& solve(std::map<int, bool>&);
	};

	struct IPulse
	{
		IPulse();
		bool operator==(IPulse&);
		IPulse& operator+(IPulse&);
		IPulse& operator=(IPulse&);
		IPulse& operator+=(IPulse&);
	};

	
	class UnitPulse : public virtual IPulse
	{
		std::map<int, State> values;
	public:
		UnitPulse();
	};

	class Pulse : public virtual IPulse
	{
		const bool is_negative;
		std::vector<IPulse&> pulses;
	public:
		Pulse();
		Pulse(bool);
	};

	class AggregatePulse : public virtual IPulse
	{
		std::vector<int> variables;
		std::map<std::vector<State>, Pulse> pulses;
	public:
		AggregatePulse();
	};

	class Node
	{
	public:
		void virtual propogate(Utilities::Stack, int);

		Node(int);
		
		const int me;
	};

	class NVariable : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, int);

		NVariable(int, Accumulator*);
	};

	class NAnd : public virtual Node
	{
		Node* left;
		Node* right;
	public:
		void propagate(Utilities::Stack, int);

		NAnd(int, Node*, Node*);
	};

	class NNot : public virtual Node
	{
		Node* fanin;
	public:
		void propagate(Utilities::Stack, int);

		NNot(int, Node*);
	};

	class NLatch : public virtual Node
	{
		Node* fanin;
		const int next_state;
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, int);

		NLatch(int, int, Node*, Accumulator*);
	};

	class NTrue : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, int);

		NTrue(Accumulator*);
	};

	class NFalse : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, int);

		NFalse(Accumulator*);
	};

	class NodeHandler : public virtual Node
	{
		Node* node;
	public:
		void propagate(Utilities::Stack, int);

		NodeHandler();

		NodeHandler(Node*);

		void set_node(Node*);
	};

	class Propagate
	{
		Utilities::Stack sat_trace;
		int last_node;
		Node* target;

	public:
		Propagate(Utilities::Stack, int, Node*);

		void operator()();
	};

	class IStartPoint
	{
	public:
		void virtual start();
	};

	class StartPoint : public IStartPoint
	{
		Node** fanins;
		const int size;
	public:
		StartPoint(Node** nodes, int size);
		StartPoint(Node**);

		void start();
	};
}