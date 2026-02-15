#pragma once
#include "graph.h"

namespace Graph
{
	template<int N>
	class GeneralStartPoint
	{
		Node* fanins[N];

	public:
		GeneralStartPoint(Node* nodes[N]):fanins(nodes) {}

		void start()
		{
			//implement with later: requires solver thread
		}
	};

	class StartPoint : public GeneralStartPoint<1>
	{
	public:
		StartPoint(Node* [1]);

		void start();
	};
}