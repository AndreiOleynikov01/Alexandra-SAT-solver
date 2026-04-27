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
		int size;
		std::vector<std::vector<IUnit>> exclusionSet;
		std::vector<IPulse*> definitiveSet;
	public:
		AggregatedPulse();
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
			IPulse* intermidiate_value;
			AccNode* next_node;
			int child_nodes;
			int fold_count;

			AccNode();
			void add_child();
			void set_master();
			void ripe(IPulse&);
			void add_pulse(UnitPulse*);
			void add_node(AccNode*);
			void fold();
			IPulse* get_result();
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

		static  class Fold
		{
			AccNode* node;
		public:
			Fold(AccNode*);

			void operator()();
		};
	};

	class Node
	{
	public:
		void virtual propagate(Utilities::Stack, Utilities::Stack);

		Node(int);
		
		const int me;
	};

	class NVariable : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		NVariable(int, Accumulator*);
	};

	class NAnd : public  Node
	{
		Node* left;
		Node* right;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		NAnd(int, Node*, Node*);
	};

	class NNot : public virtual Node
	{
		Node* fanin;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		NNot(int, Node*);
	};

	class NLatch : public virtual Node
	{
		Node* fanin;
		const int next_state;
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		NLatch(int, int, Node*, Accumulator*);
	};

	class NTrue : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		NTrue(Accumulator*);
	};

	class NFalse : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		NFalse(Accumulator*);
	};

	class NodeHandler : public virtual Node
	{
	public:
		Node* node;
	
		void propagate(Utilities::Stack, Utilities::Stack);

		NodeHandler();

		NodeHandler(Node*);

		void set_node(Node*);
	};

	class Propagate
	{
		Utilities::Stack sat_trace;
		Utilities::Stack latch_trace;
		Node* target;

	public:
		Propagate(Utilities::Stack, Utilities::Stack, Node*);

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