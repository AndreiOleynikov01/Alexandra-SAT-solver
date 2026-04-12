#pragma once
#include "graph.h"

namespace Alexandra
{
	class Solver
	{
		Graph::Accumulator accumulator;

		Graph::IStartPoint* start_point;

		std::map<int, Graph::NodeHandler&> nodes;

		std::ifstream file;

		const char delimiter;

		int number_of_inputs;

		int number_of_latches;

		int number_of_outputs;

		int number_of_ands;

		bool validate_header();

		void add_node(int);

		void inputs();

		void latches();

		void outputs();

		void ands();

	public:

		Solver(std::string&);

		Result& solve();

		Result& solve(std::map<int, bool>);
	};

	class Result 
	{
		Graph::IPulse* value;
	public:
		Result(Graph::IPulse*);

		operator std::string();
	};
}
