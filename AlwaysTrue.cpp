#include "utilities.h"

namespace Utilities
{
	AlwaysTrue::AlwaysTrue() : exception() {}

	const char* AlwaysTrue::what()
	{
		return "Problem always evaluates to 1";
	}
}