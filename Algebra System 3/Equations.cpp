#include <pch.h>
#include "Operation.h"
#include "EqTypes.h"

namespace algebra
{
	void operation::solveEquation()
	{
		operation LHS = operands[0].simplify();
		operation RHS = operands[1].simplify();

		std::set<wchar_t> unknowns = getVariables();
		int count = unknowns.size();
		
		auto eq_Type = getEqType();
		
		if (count == 1)
		{
			if (eq_Type == eqType::linear)
			{
				std::vector<operation>::iterator it = LHS.operands.begin();
				while (it != LHS.operands.end())
				{
					if (LHS.type()==opType::sum && !it->contains(opType::variable))
					{
						operation O = *it;
						O.negate();
						operation L_Sum(opType::sum);
						L_Sum.addOperand(LHS);
						L_Sum.addOperand(O);
						L_Sum.simplify();
						LHS = L_Sum;

						operation R_Sum(opType::sum);
						R_Sum.addOperand(RHS);
						R_Sum.addOperand(O);
						R_Sum.simplify();
						RHS = R_Sum;

						it = LHS.operands.begin();
					}
					else
						it++;
				}

				it = RHS.operands.begin();
				while (it != RHS.operands.end())
				{
					if (RHS.type() == opType::sum && it->contains(opType::variable))
					{
						operation O = *it;
						O.negate();
						operation L_Sum(opType::sum);
						L_Sum.addOperand(LHS);
						L_Sum.addOperand(O);
						L_Sum.simplify();
						LHS = L_Sum;

						operation R_Sum(opType::sum);
						R_Sum.addOperand(RHS);
						R_Sum.addOperand(O);
						R_Sum.simplify();
						RHS = R_Sum;

						it = RHS.operands.begin();
					}
					else
						it++;
				}

				auto zero = operation(0);
				if (in_equal(LHS, zero))
				{
					*this = operation(opType::equation);
					return;
				}

				operation V(*unknowns.begin());
				V.invert();
				operation P(opType::product);
				P.addOperand(LHS);
				P.addOperand(V);
				P.simplify();
				P.invert();
				P.simplify();
				operation ans(opType::product);
				ans.addOperand(RHS);
				ans.addOperand(P);
				ans.simplify();

				operation L(*unknowns.begin());
				operation E(opType::equation);
				E.addOperand(L);
				E.addOperand(ans);
				*this = E;
			}
		}
	}

	eqType algebra::operation::getEqType()
	{
		eqType result = eqType::none;

		for (auto& k : operands)
		{
			if (k.type() == opType::power && k.operands[0].contains(opType::variable) && !k.operands[1].contains(opType::variable))
				result = result | eqType::polynomial;
			if (k.type() == opType::power && k.operands[1].contains(opType::variable))
				result = result | eqType::exponential;
			if (k.type() == opType::variable)
				result = result | eqType::linear;
			if (k.type() == opType::product && std::any_of(k.operands.begin(), k.operands.end(), [](operation& O) {return O.type() == opType::variable; }))
				result = result | eqType::linear;

			result = result | k.getEqType();
		}
		return result;
	}

	std::set<wchar_t> algebra::operation::getVariables()
	{
		std::set<wchar_t> result;
		for (auto& k : operands)
		{
			if (k.type() == opType::variable)
				result.insert(k.Symbol);
			auto temp = k.getVariables();
			result.insert(temp.begin(), temp.end());
		}
		return result;
	}

	eqType algebra::operator|(eqType lhs, eqType rhs)
	{
		return static_cast<eqType>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}

	eqType algebra::operator&(eqType lhs, eqType rhs)
	{
		return static_cast<eqType>(static_cast<int>(lhs) & static_cast<int>(rhs));
	}
}