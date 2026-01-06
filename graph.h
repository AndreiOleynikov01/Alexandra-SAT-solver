#pragma once
#include "dependancies.h"

namespace Graph
{
	class IValidatable 
	{
		void virtual validate(Node*, Node::State);
	};

	class Node
	{
	public:
		void virtual set_true();
		void virtual set_false();
		void virtual set_undecided();

		void add_funout(IValidatable*);

		Node(int);

		static enum State
		{
			TRUE, FALSE, UNDECIDED, UNVISITED
		};

		std::vector<IValidatable*> funouts;
		
		const int me ;
	};

	class NVariable : public Node
	{
	public:
		void set_true();
		void set_false();
		void set_undecided();

		NVariable(int me);
	private:
		State state;
	};

}