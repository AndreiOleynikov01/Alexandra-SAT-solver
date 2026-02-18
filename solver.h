#pragma once
#include "graphTemplates.h"

namespace Alexandra
{
	class Solver
	{
		Graph::Accumulator accumulator;

		Graph::IStartPoint* start_point;

		std::map<int, Graph::Node&> nodes;

		bool validate_header();

		void add_node();

	public:

		Solver(std::string&);

		Result& solve();

		Result& solve(std::map<int, bool>);
	};

	class Result 
	{
		std::string print();
	};
}
