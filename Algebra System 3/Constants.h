#pragma once
#include "Operation.h"
#include <numbers>

namespace constants
{
	using namespace algebra;
	const operation E(L'e', std::numbers::e);
	const operation pi(L'π', std::numbers::pi);
	const operation phi(L'ϕ', std::numbers::phi);
}
