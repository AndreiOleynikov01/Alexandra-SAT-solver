#include "Alexandra.h"

namespace Alexandra
{
	Result::Result(Graph::IPulse* value) : value(value) {}

	Result::operator std::string()
	{
		return value->print();
	}
}