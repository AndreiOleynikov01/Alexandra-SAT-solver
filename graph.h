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
		virtual IUnit& operator=(IUnit&);
	};

	struct IPulse
	{
		static enum PulseType {
			UnitPulse, Pulse, AggregatedPulse
		};

		const PulseType type;

		IPulse(PulseType);

		virtual IPulse* operator+(IPulse&);
		virtual bool operator==(IPulse&);
		virtual IPulse* open();
		virtual std::vector <Graph::IUnit*> getUnits();
		virtual std::vector <Graph::IPulse*> getPulses();
		virtual std::vector<IUnit> getVariables();
		virtual std::string print();
		virtual bool isNegative();
		virtual IPulse* negate();
	};

	class UnitPulse : public virtual IPulse, public virtual IUnit
	{
	public:
		UnitPulse(State, int);
		IPulse* operator+(IPulse&);
		IPulse* open();
		bool operator==(IPulse&);
		std::vector <Graph::IUnit*> getUnits();
		std::vector <Graph::IPulse*> getPulses();
		std::vector<IUnit> getVariables();
		std::string print();
		bool isNegative();
		IPulse* negate();
	};

	class Pulse : public virtual IPulse
	{
		std::vector<Graph::UnitPulse*> units;
		std::vector<IPulse*> pulses;
		bool const negative;
	public:
		Pulse(bool = true, std::vector<IPulse*> = std::vector<IPulse*>(), std::vector<Graph::UnitPulse*> = std::vector<Graph::UnitPulse*>());
		IPulse* open();
		IPulse* operator+(IPulse&);
		bool operator==(IPulse&);
		std::vector <Graph::IUnit*> getUnits();
		std::vector <Graph::IPulse*> getPulses();
		std::vector<IUnit> getVariables();
		std::string print();
		bool isNegative();
		IPulse* negate();
	};

	class AggregatedPulse : public virtual IPulse
	{
		bool negative;
		std::vector<int> variables;
		std::vector<IPulse*> entries;
		AggregatedPulse(bool, std::vector<int>, std::vector<IPulse*>);
	public:
		AggregatedPulse(bool, Graph::Pulse&, Graph::Pulse&);
		IPulse* open();
		IPulse* operator+(IPulse&);
		bool operator==(IPulse&);
		std::vector <Graph::IUnit*> getUnits();
		std::vector <Graph::IPulse*> getPulses();
		std::vector<IUnit> getVariables();
		std::string print();
		bool isNegative();
		IPulse* negate();
	};

	class Accumulator
	{
		static struct AccNode
		{
			std::mutex mutex;
			bool master;
			bool satisfied;
			bool negative;
			IPulse* value;
			std::vector<AccNode*> next_node;
			int child_nodes;
			int fold_count;

			AccNode(bool);
			void add_child();
			void set_master();
			void ripe(IPulse&);
			void add_pulse(UnitPulse*);
			bool add_node(AccNode*);
			void fold();
			IPulse* get_result();
		};

		Utilities::Stack::Entry* master_pointer;
		std::mutex mutex;
		std::map<int, AccNode*> accNodes;

		AccNode* get_node(Utilities::Stack::Entry*);
		void add_node(Utilities::Stack::Entry*, AccNode*);
	public:
		Accumulator();
		void accumulate(Utilities::Stack, int, bool);
		void accumulate(Utilities::Stack);
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

		int virtual getOccurances();

		void virtual addOccurance();

		Node(int);

		const int me;
	};

	class NVariable : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

		NVariable(int, Accumulator*);
	};

	class NAnd : public  Node
	{
		std::mutex mutex;
		bool visited;
		Accumulator* accumulator;
		int occurances;

		Node* left;
		Node* right;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

		NAnd(int, Node*, Node*,Accumulator*);
	};

	class NNot : public virtual Node
	{
		std::mutex mutex;
		bool visited;
		Accumulator* accumulator;

		Node* fanin;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

		NNot(int, Node*,Accumulator*);
	};

	class NLatch : public virtual Node
	{
		Node* fanin;
		const int next_state;
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

		NLatch(int, int, Node*, Accumulator*);
	};

	class NTrue : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

		NTrue(Accumulator*);
	};

	class NFalse : public virtual Node
	{
		Accumulator* accumulator;
	public:
		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

		NFalse(Accumulator*);
	};

	class NodeHandler : public virtual Node
	{
		int occurances;
	public:
		Node* node;

		void propagate(Utilities::Stack, Utilities::Stack);

		int getOccurances();

		void addOccurance();

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