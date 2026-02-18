#include "Alexandra.h"

namespace Alexandra
{

	Solver::Solver(std::string& file_path) :nodes(), accumulator()
	{
		if (file_path.substr(file_path.size() - 4) == ".aag")
		{
			std::ifstream file(file_path, std::ifstream::in);

			std::string header;

			char delimiter = ' ';

			std::string token;

			if (file >> header)
			{
				if (header.substr(0, 3) != "aag") throw "invalid format";


			}
		}
		else
		{
			throw "file is not accepted";
		}
	}
}