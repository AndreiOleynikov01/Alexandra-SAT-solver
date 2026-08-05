#include "Alexandra.h"

namespace Alexandra
{

	Solver::Solver(std::string& file_path) :file(), nodes(), delimiter(' '), accumulator()
	{
		if (file_path.substr(file_path.size() - 4) == ".aag")
		{
			file.open(file_path, std::ifstream::in);

			if (Solver::validate_header())
			{
				inputs();

				latches();

				outputs();

				ands();

				file.close();

			}
			else throw std::invalid_argument("invalid file format - header");
		}
		else
		{
			throw std::invalid_argument("file is not accepted");
		}
	}

	bool Solver::validate_header()
	{
		std::string header;

		std::getline(file,header);

		std::stringstream stream(header);

		std::string token;

		std::vector<std::string> tokens;

		while(std::getline(stream, token, delimiter))
		{
			tokens.push_back(token);
		}

		if (tokens.size() != 6)
		{
			return false;
		}
		else
		{
			if (tokens.at(0) != "aag") return false;

			try
			{
				number_of_inputs = std::stoi(tokens.at(2));

				number_of_latches = std::stoi(tokens.at(3));

				number_of_outputs = std::stoi(tokens.at(4));

				number_of_ands = std::stoi(tokens.at(5));
			}
			catch (std::invalid_argument)
			{
				return false;
			}
		}

		return true;
	}

	void Solver::add_node(int node_index)
	{

		Graph::NodeHandler* vertice = new Graph::NodeHandler();
		if (node_index == 0)
		{
			Graph::NFalse* node_0 = new Graph::NFalse(&accumulator);
			vertice->set_node(node_0);
			nodes[node_index] =  vertice;
		}
		else if (node_index == 1)
		{
			Graph::NTrue* node_1 = new Graph::NTrue(&accumulator);
			vertice->set_node(node_1);
			nodes[node_index] = vertice;
		}
		else
		{
			if (node_index % 2)
			{
				if (!nodes.contains(node_index-1))
				{
					add_node(node_index - 1);
				}


				Graph::NNot* not_node = new Graph::NNot(node_index, nodes[node_index-1], &accumulator);
				vertice->set_node(not_node);
				nodes[node_index-1]->addOccurance();

				nodes[node_index] = vertice;

			}
			else
			{

				nodes[node_index] =  vertice;
			}
		}
	}

	void Solver::inputs()
	{
		for (int i = 1; i <= number_of_inputs; i++)
		{
			std::string line;
			std::getline(file, line);
			int node_index;

			try
			{
				node_index = std::stoi(line);
			}
			catch (std::invalid_argument)
			{
				throw new std::invalid_argument("invalid file format");
			}

			add_node(node_index);

			nodes[node_index]->set_node(new Graph::NVariable(node_index, &accumulator));
		}
	}

	void Solver::latches()
	{
		for (int i = 1; i <= number_of_latches; i++)
		{
			std::string line;
			std::getline(file, line);
			std::stringstream stream(line);

			int node_index;
			int next_state;

			try
			{
				std::string token_latch;
				std::getline(stream, token_latch, delimiter);
				node_index = std::stoi(token_latch);

				std::string token_next;
				std::getline(stream, token_next, delimiter);
				next_state = std::stoi(token_next);
			}
			catch (std::invalid_argument)
			{
				throw  std::invalid_argument("invalid file format");
			}

			add_node(node_index);

			Graph::Node* node_next_state;

			if (!nodes.contains(next_state))
			{
				add_node(next_state);
			}

			node_next_state = nodes[next_state];

			node_next_state->addOccurance();

			Graph::NLatch* latch = new Graph::NLatch(node_index, next_state, node_next_state, &accumulator);

			nodes[node_index]->set_node(latch);

			//debug 
			std::cout << next_state << " occurs " << node_next_state->getOccurances() << " times" << std::endl;
			std::cout << node_index << " occurs " << nodes[node_index]->getOccurances() << " times" << std::endl;
		}
	}

	void Solver::outputs()
	{
		if (number_of_outputs > 1)
		{
			Graph::Node** outputs = new Graph::Node* [number_of_outputs];

			for (int i = 1; i <= number_of_outputs; i++)
			{
				std::string line;
				std::getline(file, line);
				int index;

				try 
				{
					index = std::stoi(line);
				}
				catch (std::invalid_argument)
				{
					throw std::invalid_argument("invalid file format");
				}

				if (!nodes.contains(index))
				{
					add_node(index);
				}

				*outputs = nodes[index];
				outputs++;
			}

			start_point = new Graph::StartPoint(outputs, number_of_outputs);
		}
		else if (number_of_outputs == 1)
		{
			Graph::Node** outputs = new Graph::Node * [number_of_outputs];

			std::string line;
			std::getline(file, line);
			int index;

			try
			{
				index = std::stoi(line);
			}
			catch (std::invalid_argument)
			{
				throw std::invalid_argument("invalid file format");
			}

			if (index == 1)
			{
				throw Utilities::AlwaysTrue();
			}
			else if (index == 0)
			{
				throw Utilities::AlwaysFalse();
			}
			else
			{
				if (!nodes.contains(index))
				{
					add_node(index);
				}

				*outputs = nodes[index];
			}

			start_point = new Graph::StartPoint(outputs, number_of_outputs);
		}
		else
		{
			throw Utilities::Unsolvable();
		}
	}

	void Solver::ands()
	{
		for (int i = 1; i <= number_of_ands; i++)
		{
			std::string line;
			std::getline(file, line);
			std::stringstream stream(line);

			std::string node_token;
			std::string left_token;
			std::string right_token;

			int node_index;
			int right_fanin;
			int left_fanin;

			try
			{
				std::getline(stream, node_token, delimiter);
				std::getline(stream, left_token, delimiter);
				std::getline(stream, right_token, delimiter);

				node_index = std::stoi(node_token);
				right_fanin = std::stoi(right_token);
				left_fanin = std::stoi(left_token);
			}
			catch (std::invalid_argument)
			{
				throw std::invalid_argument("invalid file format");
			}

			if (nodes[node_index] == NULL)
			{
				add_node(node_index);
			}

			if (nodes[right_fanin] == NULL)
			{
				add_node(right_fanin);
			}

			if (nodes[left_fanin] == NULL)
			{
				add_node(left_fanin);
			}

			nodes[left_fanin]->addOccurance();
			nodes[right_fanin]->addOccurance();

			Graph::NAnd* node = new Graph::NAnd(node_index, nodes[left_fanin], nodes[right_fanin],&accumulator);

			nodes[node_index]->set_node(node);

			//debug 
			std::cout<<left_fanin<<" occurs " << nodes[left_fanin]->getOccurances()<<" times" << std::endl;
			std::cout << right_fanin << " occurs " << nodes[right_fanin]->getOccurances() << " times" << std::endl;
		}
	}

	Result& Solver::solve()
	{
		start_point->start();
		return *new Result(accumulator.solve());
	}

	Result& Solver::solve(std::map<int, bool> assumption)
	{
		start_point->start();
		return *new Result(accumulator.solve(assumption));
	}
}

