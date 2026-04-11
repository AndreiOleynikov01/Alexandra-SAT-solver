#include "Alexandra.h"

namespace Graph
{
	Accumulator::Accumulator() : mutex(), accNodes() {}

	void Accumulator::get_node(Utilities::Stack::Entry*) {}
	void Accumulator::add_node(Utilities::Stack::Entry*, Accumulator::AccNode*) {}
}