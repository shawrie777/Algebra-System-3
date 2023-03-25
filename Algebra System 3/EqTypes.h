#pragma once

namespace algebra
{
	enum class eqType
	{
		none = 0,
		polynomial = 1 << 0,
		trig = 1 << 1,
		exponential = 1 << 2,
		linear = 1 << 3
	};

	eqType operator|(eqType lhs, eqType rhs);
	eqType operator&(eqType lhs, eqType rhs);

}
