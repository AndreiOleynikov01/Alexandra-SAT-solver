#include "utilities.h"

namespace Utilities
{
	AlwaysFalse::AlwaysFalse() :exception() {}

	const char* AlwaysFalse::what()
	{
		return "Problem always evaluates to 0";
	}
}