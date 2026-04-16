#pragma once
#include "utilities.h"

namespace Graph
{
	enum State
	{
		ANY, TRUE, FALSE, CONFLICT
	};

	struct IUnit
	{
		const State value;
		const int variable;
		IUnit(State, int);
		IUnit(IUnit*);
		IUnit& operator=(IUnit&);
	};

	struct IPulse
	{
		static enum PulseType {
			UnitPulse, Pulse, AggregatedPulse
		};

		const PulseType type;

		IPulse(PulseType);

		virtual IPulse* operator+(IPulse&) ;
		virtual bool operator==(IPulse&) ;
		virtual IPulse* open();
		virtual std::vector <IPulse*> getvalues();
		virtual std::vector<IUnit> getVariables();
		virtual std::string print();
		virtual bool isNegative();
	};

	class UnitPulse : public virtual IPulse, public virtual IUnit
	{
	public:
		UnitPulse(State, int);
		IPulse* operator+(IPulse&) ;
		IPulse* open();
		bool operator==(IPulse&) ;
		std::vector <IPulse*> getvalues();
		std::vector<IUnit> getVariables();
		std::string print();
		bool  isNegative();
	};

	class Pulse : public virtual IPulse
	{
		std::vector<IPulse*> pulses;
		bool const negative;
	public:
		Pulse(bool = true);
		Pulse(bool = true, std::vector<IPulse*> = std::vector<IPulse*>());
		IPulse* open();
		IPulse* operator+(IPulse&);
		bool operator==(IPulse&);
		std::vector <IPulse*> getvalues();
		std::vector<IUnit> getVariables();
		std::string print();
		bool isNegative();
	};

	class AggregatedPulse : public virtual IPulse
	{
		const bool negative;
		int size;
		std::vector<std::vector<IUnit>> exclusionSet;
		std::vector<IPulse*> definitiveSet;
	public:
		AggregatedPulse(bool = false);
		AggregatedPulse(Graph::Pulse&, Graph::Pulse&);
		IPulse* open();
		IPulse* operator+(IPulse&) ;
		bool operator==(IPulse&);
		std::vector <IPulse*> getvalues();
		std::vector<IUnit> getVariables();
		std::string print();
		bool isNegative();
	};

	class Accumulator
	{
		static struct AccNode
		{
			std::mutex mutex;
			bool master;
			std::vector<IPulse*> signals;
			std::vector<AccNode*> nodes;

			AccNode(bool);
			void add_pulse(UnitPulse*);
			void add_node(AccNode*);
			IPulse* fold();
		};

		Utilities::Stack::Entry* master_pointer;
		std::mutex mutex;
		std::map<Utilities::Stack::Entry*, AccNode*> accNodes;

		AccNode* get_node(Utilities::Stack::Entry*);
		void add_node(Utilities::Stack::Entry*, AccNode*);
	public:
		Accumulator();
		void accumulate(Utilities::Stack, int, bool);
		IPulse* solve();
		IPulse* solve(std::map<int, bool>&);

		class Add
		{
			std::mutex mutex;
			std::condition_variable cv;
			const int number_of_operands;
			std::vector<IPulse*> operands;
			IPulse** result_buffer;
			Add* adder;

		public:
			Add(int, Add* = NULL, IPulse* = NULL);

			void add_operand(IPulse*);

			void operator()();
		};

		static  class Fold
		{
			Add* adder;
			AccNode* node;
		public:
			Fold(Add*, AccNode*);

			void operator()();
		};
	};

	class Node
	{
	public:
		virtual void propogate(Utilities::Stack, int);

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

	class StartPoint : public virtual IStartPoint
	{
		Node** fanins;
		const int size;
	public:
		StartPoint(Node** nodes, int size);
		StartPoint(Node**);

		void start();
	};

}