#include "Alexandra.h"

int main(int argc, char* argv[])
{
		std::vector<std::string> arguments;
		for (int i = 1; i < argc; i++)
		{
			arguments.push_back(argv[i]);
		}

		if (arguments.empty())
		{
			std::cout << "please input number of threads (optional), file path, and assumption (optional)" << std::endl;
			return 0;
		}

		int number_of_threads = 0;
		std::string file_path;
		std::map<int, bool> assumption;

		if (arguments.size() == 1)
		{
			file_path = arguments.back();
		}
		else
		{
			try
			{
				number_of_threads = std::stoi(arguments[0]);
			}
			catch (std::invalid_argument* err)
			{
				std::cout << err << std::endl;
				return 0;
			}

			file_path = arguments[1];

			for (int i = 2; i < arguments.size(); i++)
			{
				int variable;
				try
				{
					variable = std::stoi(arguments[0]);
				}
				catch (std::invalid_argument* err)
				{
					std::cout << err << std::endl;
					return 0;
				}
				if (variable / 2 == 0)
				{
					assumption[variable] = true;
				}
				else
				{
					assumption[variable - 1] = false;
				}
			}
		}

		if (number_of_threads >= 0)
		{
			Utilities::ThreadPool::initialise(number_of_threads);
		}
		else
		{
			std::cout << "invalid input: number of threads cannot be negative";
			return 0;
		}

		try
		{
			Alexandra::Solver solver(file_path);
			if (assumption.empty())
			{
				std::cout << (std::string)solver.solve() << std::endl;
			}
			else
			{
				std::cout << (std::string)solver.solve(assumption) << std::endl;
			}
		}
		catch (std::exception& err)
		{
			std::cout << err.what() << std::endl;
		}
}
