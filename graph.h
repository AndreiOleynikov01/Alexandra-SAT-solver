#pragma once
#include "dependancies.h"

namespace Graph
{
	struct IValidatable 
	{
		void virtual validate(Node*, Node::State);

	};

	struct Validator 
	{
		Validator(Node*, Node::State, IValidatable);

		void operator()();

	private:
		Node* me;
		Node::State state;
		IValidatable target;
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