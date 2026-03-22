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
		bool virtual operator==(IPulse&);
		std::vector <IPulse&> virtual negate();
		std::vector<IPulse&> virtual getValues();
		std::string virtual print();
	private:
		bool negatitve;
	};

	class UnitPulse : public virtual IPulse
	{
		int value;
		bool isNegative;
	public:
		UnitPulse(bool, int);
		bool operator==(IPulse&);
		std::vector <IPulse&> negate();
		std::vector<IPulse&> getValues();
		std::string print();
	};

	class Pulse : public virtual IPulse
	{
		std::vector<IPulse&> pulses;
	public:
		Pulse();
		Pulse(UnitPulse&);
		bool operator==(IPulse&);
		std::vector <IPulse&> negate();
		std::vector<IPulse&> getValues();
		std::string print();
	};

	class AggregatePulse : public virtual IPulse
	{
		std::set<std::vector<State>> exclusionSet;
		std::vector<int> definitiveSet;
		std::set<std::vector<State>> definitiveConfigurations;
		std::vector<IPulse&> dependantSet;
	public:
		AggregatePulse();
		AggregatePulse(UnitPulse&);
		AggregatePulse(Pulse&);
		bool operator==(IPulse&);
		std::vector <IPulse&> negate();
		std::vector<IPulse&> getValues();
		std::string print();
	};

	Pulse& operator+(Pulse&, Pulse&);
	AggregatePulse& operator+(AggregatePulse&, AggregatePulse&);

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