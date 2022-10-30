#include <pch.h>
#include "Operation.h"
#include <numeric>
#include <set>

namespace algebra
{
	std::pair<int, int> operator+(std::pair<int, int> lhs, std::pair<int, int> rhs);
	std::pair<int, int> operator*(std::pair<int, int> lhs, std::pair<int, int> rhs);

	bool operator<(const operation& lhs, const operation& rhs)
	{
		return lhs.priority < rhs.priority;
	}

	bool operator>(const operation& lhs, const operation& rhs)
	{
		return lhs.priority > rhs.priority;
	}

	bool operator==(const operation& lhs, const operation& rhs)
	{
		return lhs.priority == rhs.priority;
	}

	bool operator!=(const operation& lhs, const operation& rhs)
	{
		return lhs.priority != rhs.priority;
	}

	bool operation::isPolynomial()
	{
		if (Type != opType::sum)
			return false;
		
		wchar_t symbol = L' ';
		auto it = operands.begin();
		while (it != operands.end())
		{
			switch (it->Type)
			{
			case opType::product:
			{
				bool found = false;
				for (auto& k : it->operands)
				{
					switch (k.Type)
					{
					case opType::power:
						if (found)
							return false;
						else
							found = true;
						if (k.operands[0].Type != opType::variable)
							return false;
						if (symbol == L' ')
							symbol = k.operands[0].Symbol;
						else if (k.operands[0].Symbol != symbol)
							return false;
						if (k.operands[1].Type != opType::integer)
							return false;
						break;
					case opType::variable:
						if (found)
							return false;
						else
							found = true;
						if (symbol == L' ')
							symbol = k.Symbol;
						else if (k.Symbol != symbol)
							return false;
						break;
					case opType::integer:
					case opType::rational:
					case opType::constant:
						break;
					default:
						return false;
						break;
					}
				}
			}
				break;
			case opType::power:
			{
				if (it->operands[0].Type != opType::variable)
					return false;
				if (symbol == L' ')
					symbol = it->operands[0].Symbol;
				else if (it->operands[0].Symbol != symbol)
					return false;
				if (it->operands[0].Type != opType::integer)
					return false;
			}
				break;
			case opType::integer:
			case opType::rational:
			case opType::constant:
				break;
			default:
				return false;
				break;
			}
			it++;
		}

		return true;
	}

	//only for use in sort functions
	bool in_lessThan(operation& lhs, operation& rhs)
	{
		if (lhs.priority < rhs.priority)
			return true;
		else if (lhs.priority > rhs.priority)
			return false;
		else if (lhs.operands.size() < rhs.operands.size())
			return true;
		else if (lhs.operands.size() > rhs.operands.size())
			return false;
		else if (lhs.isNumber() && rhs.isNumber())
		{
			double lval = 0, rval = 0;
			if (std::holds_alternative<int>(lhs.value))
				lval = std::get<0>(lhs.value);
			else if (std::holds_alternative<std::pair<int, int>>(lhs.value))
			{
				double a = std::get<1>(lhs.value).first;
				double b = std::get<1>(lhs.value).second;
				lval = a / b;
			}
			else
				lval = std::get<2>(lhs.value);

			if (std::holds_alternative<int>(rhs.value))
				rval = std::get<0>(rhs.value);
			else if (std::holds_alternative<std::pair<int, int>>(rhs.value))
			{
				double a = std::get<1>(rhs.value).first;
				double b = std::get<1>(rhs.value).second;
				rval = a / b;
			}
			else
				rval = std::get<2>(rhs.value);

			return lval < rval;
		}

		std::vector<operation> lcopy = lhs.operands;
		std::vector<operation> rcopy = rhs.operands;

		std::sort(lcopy.begin(), lcopy.end(), in_lessThan);
		std::sort(rcopy.begin(), rcopy.end(), in_lessThan);

		for (int i = 0; i < lcopy.size(); i++)
			if (in_lessThan(lcopy[i], rcopy[i]))
				return true;
			else if (in_lessThan(rcopy[i], lcopy[i]))
				return false;

		return false;
	}

	bool in_equal(operation& lhs, operation& rhs)
	{
		if (lhs.priority != rhs.priority)
			return false;
		if (lhs.operands.size() != rhs.operands.size())
			return false;
		if (lhs.Type != rhs.Type)
			return false;

		if (lhs.Type == opType::integer && std::get<0>(lhs.value) != std::get<0>(rhs.value))
			return false;
		if (lhs.Type == opType::rational && std::get<1>(lhs.value) != std::get<1>(rhs.value))
			return false;
		if (lhs.Type == opType::constant && std::get<2>(lhs.value) != std::get<2>(rhs.value))
			return false;
		if (lhs.Type == opType::variable && rhs.Type == opType::variable)
			return lhs.Symbol == rhs.Symbol;

		std::vector<operation> lcopy = lhs.operands;
		std::vector<operation> rcopy = rhs.operands;

		std::set<opType> sortable{ opType::sum,opType::product };
		if (sortable.find(lhs.Type) != sortable.end())
		{
			std::sort(lcopy.begin(), lcopy.end(), in_lessThan);
			std::sort(rcopy.begin(), rcopy.end(), in_lessThan);
		}

		for (int i = 0; i < lcopy.size(); i++)
		{
			if (!in_equal(lcopy[i], rcopy[i]))
				return false;
		}
		return true;
	}

	operation::operation(opType type) : Type(type), Symbol(L' ')
	{
		switch (type)
		{
		case opType::integer:
			priority = 100;
			break;
		case opType::rational:
			priority = 100;
			break;
		case opType::sum:
			operandCount = -1;
			priority = 50;
			OpPosition = opPosition::both;
			break;
		case opType::product:
			operandCount = -1;
			priority = 75;
			OpPosition = opPosition::both;
			break;
		case opType::power:
			operandCount = 2;
			priority = 90;
			OpPosition = opPosition::both;
			break;
		default:
			break;
		}
	}

	operation::operation(int intVal) : Type(opType::integer), priority(100), value(intVal), Symbol(L' ')
	{
	}

	operation::operation(int numerator, int denominator) : Type(opType::rational), priority(100), Symbol(L' ')
	{
		if (denominator == 0)
			throw("Denominator can't be 0");
		if (denominator == 1)
		{
			Type = opType::integer;
			value = numerator;
		}
		else
		{
			int G = std::gcd(numerator, denominator);
			value = std::pair(numerator / G, denominator / G);
		}
	}

	operation::operation(wchar_t symbol, double val) : Symbol(symbol), Type(opType::constant), value(val), priority(100)
	{

	}

	operation::operation(wchar_t symbol) : Symbol(symbol), Type(opType::variable), priority(100)
	{
	}

	const opType operation::type()
	{
		return Type;
	}

	const opPosition operation::getPosition()
	{
		return OpPosition;
	}

	const int operation::getPriority()
	{
		return priority;
	}

	void operation::addOperand(operation op)
	{
		if (operandCount == -1 || operands.size() < operandCount)
			operands.push_back(op);
	}

	void operation::setBracketCount(int depth)
	{
		priority += 100 * depth;
	}

	bool operation::isNumber() const
	{
		return Type == opType::integer || Type == opType::rational || Type == opType::constant;
	}

	void operation::unNest()
	{
		switch (Type)
		{
		case opType::sum:
		case opType::product:
		{
			auto it = operands.begin();
			while (it != operands.end())
			{
				if (it->type() == Type)
				{
					int pos = it - operands.begin();
					operands.insert(operands.end(), it->operands.begin(), it->operands.end());
					it = operands.begin() + pos;
					it = operands.erase(it);
				}
				else
					it++;
			}
		}
		break;
		default:
			break;
		}
	}

	void operation::negate()
	{
		if (std::holds_alternative<int>(value))
			value = std::get<0>(value) * -1;
		else if (std::holds_alternative<std::pair<int, int>>(value))
		{
			std::pair<int, int> val = std::get<1>(value);
			value = std::pair<int, int>(-1 * val.first, val.second);
		}
		else
		{
			operation op(opType::product);
			operation minus(-1);
			op.addOperand(minus);
			op.addOperand(*this);
			*this = op;
		}
	}

	void operation::invert()
	{
		if (std::holds_alternative<int>(value))
			value = std::pair<int, int>(1, std::get<0>(value));
		else if (std::holds_alternative <std::pair<int, int>>(value))
		{
			std::pair<int, int> val = std::get<1>(value);
			value = std::pair<int, int>(val.second, val.first);
		}
		else if (Type == opType::power)
		{
			operands[1].negate();
		}
		else
		{
			operation op(opType::power);
			operation minus(-1);
			op.addOperand(*this);
			op.addOperand(minus);
			*this = op;
		}
	}

	std::variant<int, std::pair<int, int>, double> operation::getValue()
	{
		if (isNumber())
			return value;
		else
			throw("This operation has no value");
	}

	operation operation::simplify()
	{
		//check ordering on these
		unNest();
		for (auto it = operands.begin(); it != operands.end(); it++)
			it->simplify();

		switch (Type)
		{
		case opType::sum:
		{
			gatherTerms();			
			return *this;
		}
		break;
		case opType::product:
			gatherTerms();
			return *this;
			break;
		case opType::power:
			//needs filling
			//break;
		default:
			return *this;
			break;
		}
	}

	std::vector<std::pair<wchar_t, charProps>> operation::write()
	{
		std::vector<std::pair<wchar_t, charProps>> output;
		switch (Type)
		{
		case opType::constant:
			output.push_back({ Symbol,charProps{} });
			break;
		case opType::integer:
		{
			auto str = std::to_wstring(std::get<int>(value));
			for (auto it = str.begin(); it != str.end(); it++)
				output.push_back({ *it, charProps() });
		}
		break;
		case opType::rational:
		{
			std::pair<int, int> vals = std::get<1>(value);
			std::wstring str = std::to_wstring(vals.first) + L"/" + std::to_wstring(vals.second);
			for (auto it = str.begin(); it != str.end(); it++)
				output.push_back({ *it,charProps() });
		}
		break;
		case opType::variable:
			output.push_back({ Symbol,charProps{} });
			break;
		case opType::sum:
			for (auto& k : operands)
			{
				auto temp = k.write();
				output.insert(output.end(), temp.begin(), temp.end());
				output.push_back({ L'+',charProps() });
			}
			output.pop_back();
			break;
		case opType::product:
		{
			bool symbol = false;
			for (auto& k : operands)
			{
				if (symbol && (k.Type == opType::integer || k.Type == opType::rational))
					output.push_back({ L'*',charProps() });

				auto temp = k.write();
				output.insert(output.end(), temp.begin(), temp.end());
				
				symbol = k.Type == opType::integer || k.Type == opType::rational;
			}
		}
			break;
		case opType::power:
		{
			auto temp = operands[0].write();
			output.insert(output.end(), temp.begin(), temp.end());

			temp = operands[1].write();
			charProps powProp;
			powProp.superscript = true;

			for (auto& k : temp)
			{
				k.second += powProp;
				output.push_back(k);
			}
		}
			break;
		default:
			break;
		}

		for (int i = 0; i < output.size() - 1; i++)
		{
			if (output[i].first == L'+' && output[i + 1].first == L'-')
				output.erase(output.begin() + i);
		}
		return output;
	}

	std::pair<int, int> operator+(std::pair<int, int> lhs, std::pair<int, int> rhs)
	{
		int top = lhs.first * rhs.second + rhs.first * lhs.second;
		int bottom = lhs.second * rhs.second;
		int GCD = std::gcd(top, bottom);
		top /= GCD;
		bottom /= GCD;
		return std::make_pair(top, bottom);
	}

	std::pair<int, int> operator*(std::pair<int, int> lhs, std::pair<int, int> rhs)
	{
		int top = lhs.first * rhs.first;
		int bottom = lhs.second * rhs.second;
		int GCD = std::gcd(top, bottom);
		top /= GCD;
		bottom /= GCD;
		return std::make_pair(top, bottom);
	}

	void operation::gatherTerms()
	{
		if (Type == opType::sum)
		{
			//coef, then term
			std::vector<std::pair<std::pair<int, int>, operation>> terms;
			std::pair<int, int> fracTerm(0,1);
			auto termIt = terms.begin();
			auto it = operands.begin();
			while (it != operands.end())
			{
				if (it->Type == opType::integer)
				{
					fracTerm = fracTerm + std::pair<int, int>(std::get<0>(it->value), 1);
					it++;
					continue;
				}
				if (it->Type == opType::rational)
				{
					fracTerm = fracTerm + std::get<1>(it->value);
					it++;
					continue;
				}


				std::pair<int, int> coef(1,1);
				operation op(opType::empty);

				if (it->Type == opType::product)
				{					
					auto subIt = it->operands.begin();
					while (subIt != it->operands.end())
					{
						if (subIt->Type == opType::integer)
						{
							coef = std::pair<int, int>(std::get<0>(subIt->value), 1) * coef;
							subIt = it->operands.erase(subIt);
						}
						else if (subIt->Type == opType::rational)
						{
							coef = std::get<1>(subIt->value) * coef;
							subIt = it->operands.erase(subIt);
						}
						else
							subIt++;
					}

					if (it->operands.size() == 1)
						op = it->operands[0];
					else op = *it;
				}
				else				
					op = *it;
				
				termIt = terms.begin();
				while (termIt != terms.end())
				{
					if (in_equal(termIt->second, op))
					{
						//add coefs
						termIt->first = termIt->first + coef;
						break;
					}
					termIt++;
				}
				if (termIt == terms.end())
					terms.push_back({ coef, op });

				it++;
			}

			termIt = terms.begin();
			operation result(opType::sum);
			while (termIt != terms.end())
			{
				//coef is 1
				if (termIt->first.first == 1 && termIt->first.second == 1)
					result.addOperand(termIt->second);

				//coef is int
				else if (termIt->first.second == 1)
				{
					operation I(termIt->first.first);
					operation P(opType::product);
					P.addOperand(I);
					P.addOperand(termIt->second);
					result.addOperand(P);
				}

				//coef is fraction
				else
				{
					operation R(termIt->first.first, termIt->first.second);
					operation P(opType::product);
					P.addOperand(R);
					P.addOperand(termIt->second);
					result.addOperand(P);
				}

				termIt++;
			}
			
			//result is only fracTerm
			if (result.operands.size() == 0)
			{
				if (fracTerm.second == 1)
					*this = operation(fracTerm.first);
				else
					*this = operation(fracTerm.first, fracTerm.second);
			}
			else if (result.operands.size() == 1 && fracTerm.first == 0)
				*this = result.operands[0];			
			else
			{
				if (fracTerm.first != 0 && fracTerm.second == 1)
				{
					operation I(fracTerm.first);
					result.addOperand(I);
				}
				else if (fracTerm.first != 0)
				{
					operation F(fracTerm.first, fracTerm.second);
					result.addOperand(F);
				}
				*this = result;
			}
		}
		else if (Type == opType::product)
		{
			//term, then power
			std::vector<std::pair<operation, operation>> terms;
			std::pair<int, int> fracTerm(1, 1);
			auto termIt = terms.begin();
			auto it = operands.begin();
			while (it != operands.end())
			{
				if (it->Type == opType::integer)
				{
					fracTerm = fracTerm * std::pair<int, int>(std::get<0>(it->value), 1);
					it++;
					continue;
				}
				if (it->Type == opType::rational)
				{
					fracTerm = fracTerm * std::get<1>(it->value);
					it++;
					continue;
				}

				operation pow(1);
				operation base(opType::empty);
				if (it->Type == opType::power)
				{
					base = it->operands[0].simplify();
					pow = it->operands[1].simplify();
				}
				else
					base = *it;

				termIt = terms.begin();
				while (termIt != terms.end())
				{
					if (in_equal(termIt->first, base))
					{
						operation S(opType::sum);
						S.addOperand(termIt->second);
						S.addOperand(pow);
						termIt->second = S.simplify();
						break;
					}
					termIt++;
				}
				if (termIt == terms.end())
					terms.push_back({ base, pow });

				it++;
			}

			termIt = terms.begin();
			operation result(opType::product);
			if (fracTerm.first == 0)
			{
				*this = operation(0);
			}
			else if (fracTerm.second == 1 && fracTerm.first != 1)
			{
				operation I(fracTerm.first);
				result.addOperand(I);
			}
			else if (fracTerm.first != 1)
			{
				operation R(fracTerm.first, fracTerm.second);
				result.addOperand(R);
			}
			while (termIt != terms.end())
			{
				if (termIt->second.Type == opType::integer && std::get<0>(termIt->second.value) == 1)
					result.addOperand(termIt->first);
				else
				{
					operation P(opType::power);
					P.addOperand(termIt->first);
					P.addOperand(termIt->second);
					result.addOperand(P);
				}
				termIt++;
			}

			if (result.operands.size() == 1)
				*this = result.operands[0];
			else
				*this = result;
		}
	}

	polynomial::polynomial(std::map<int, std::pair<int, int>> Terms) : terms(Terms)
	{
	}

	polynomial::polynomial(operation op)
	{
		try
		{
			if (op.type() != opType::sum)
				throw("Not a sum");
			int LCM = 1;
			auto it = op.operands.begin();
			while (it != op.operands.end())
			{
				int power = 0;
				std::pair<int,int> coef{1,1};
				switch (it->Type)
				{
				case opType::product:
					for (auto& k : it->operands)
					{
						switch (k.Type)
						{
						case opType::power:
							if (k.operands[0].Type != opType::variable)
								throw("Base isn't variable");
							if (symbol == L' ')
								symbol = k.operands[0].Symbol;
							else if (k.operands[0].Symbol != symbol)
								throw("Change in variable");
							if (k.operands[1].Type != opType::integer)
								throw("Invalid power");
							else
								power = std::get<int>(k.operands[1].value);
							break;
						case opType::variable:
							if (symbol == L' ')
								symbol = k.Symbol;
							else if (k.Symbol != symbol)
								throw("Change in variable");
							power = 1;
							break;
						case opType::integer:
							coef = coef * std::pair<int,int>(std::get<int>(k.value),1);
							break;
						case opType::rational:
							coef = coef * std::get<1>(k.value);
							break;
						case opType::constant:
							throw("Irrational coefficient");
							break;
						default:
							throw("Invalid product");
							break;
						}
					}
					break;
				case opType::power:
					if (it->operands[0].Type != opType::variable)
						throw("Base isn't variable");
					if (symbol == L' ')
						symbol = it->operands[0].Symbol;
					else if (it->operands[0].Symbol != symbol)
						throw("Change in variable");
					if (it->operands[1].Type != opType::integer)
						throw("Invalid power");
					else
						power = std::get<int>(it->operands[1].value);
					break;
				case opType::variable:
					if (symbol == L' ')
						symbol = it->Symbol;
					else if (it->Symbol != symbol)
						throw("Change in variable");
					power = 1;
					break;
				case opType::integer:
					coef = coef * std::pair<int, int>(std::get<int>(it->value), 1);
					break;
				case opType::rational:
					coef = coef * std::get<1>(it->value);
					break;
				case opType::constant:
					throw("Irrational coefficient");
					break;
				default:
					throw("Invalid type.");
					break;
				}
				terms.insert({ power, coef });

				LCM = std::lcm(LCM, coef.second);
				order = order > power ? order : power;
				it++;
			}

			for (auto it = terms.begin(); it != terms.end(); it++)
				it->second = std::pair(LCM, 1) * it->second;

			for (int k=0; k<order;k++)
				if (!terms.contains(k))
					terms.insert({ k,std::make_pair(0,0) });
		}
		catch(std::string err)
		{
			throw;
		}
	}

	//val must be a rational number
	bool polynomial::testValue(operation val)
	{
		std::pair<int, int> testVal;
		if (val.Type == opType::integer)
			testVal = std::pair<int, int>(std::get<0>(val.value), 1);
		else if (val.Type == opType::rational)
			testVal = std::get<1>(val.value);
		else
			throw("Not a rational value");

		std::pair total(0,1);
		for (int i = 0; i < terms.size(); i++)
		{			
			std::pair<int,int> term(pow(testVal.first, terms[i].first), pow(testVal.second, terms[i].first));
			term = term * terms[i];
			total = total + term;
		}

		return abs(static_cast<double>(total.first) / total.second) < 0.000001;
	}

	std::vector<operation> polynomial::ratRoots()
	{
		std::vector<operation> roots;
		int highC = (--terms.end())->second.first;
		int lowC = terms.begin()->second.first;
		
		auto highFacs = factors(highC);
		auto lowFacs = factors(lowC);

		for (int i=0;i<highFacs.size();i++)
			for (int j = 0; j < lowFacs.size(); j++)
			{
				operation R(lowFacs[j], highFacs[i]);
				if (testValue(R))
					roots.push_back(R);
				R.negate();
				if (testValue(R))
					roots.push_back(R);
			}

		return roots;
	}

	operation polynomial::ratFactor()
	{
		operation result(opType::product);
		auto roots = ratRoots();
		std::map<int, std::pair<int, int>> newTerms = terms;
		operation var(symbol);

		for (auto it = roots.begin(); it != roots.end(); it++)
		{
			
			operation root = *it;
			std::pair<int, int> rootPair;
			if (root.Type == opType::integer)
				rootPair = std::make_pair(std::get<0>(root.value), 1);
			else
				rootPair = std::get<1>(root.value);
			root.negate();
			operation factor(opType::sum);
			factor.addOperand(var);
			factor.addOperand(root);

			result.addOperand(factor);

			auto rootInv = std::make_pair(rootPair.second, rootPair.first);
			std::pair neg(-1, 1);

			newTerms[0] = newTerms[0] * rootInv * neg;

			for (int i = 1; i < newTerms.size(); i++)
				newTerms[i] = (newTerms[i - 1] + (neg * newTerms[i])) * rootInv;

			newTerms.erase(newTerms.size() - 1);
		}

		polynomial poly(newTerms);
		result.addOperand(poly.ToOperation());
		return result;
	}

	operation polynomial::ToOperation()
	{
		operation S(opType::sum);
		operation var(symbol);

		auto it = terms.rbegin();
		while (it != terms.rend())
		{
			if (it->second.first == 0)
				continue;
			else if (it->first == 0)
			{
				if (it->second.second == 1)
				{
					operation I(it->second.first);
					S.addOperand(I);
				}
				else
				{
					operation R(it->second.first, it->second.second);
					S.addOperand(R);
				}
			}
			else if (it->second.first == 1 && it->second.second == 1)
			{
				if (it->first == 1)					
					S.addOperand(var);				
				else
				{
					operation P(opType::power);
					P.addOperand(var);

					operation K(it->first);
					P.addOperand(K);
					S.addOperand(P);
				}
			}
			else
			{
				operation prod(opType::product);
				if (it->second.second == 1)
				{
					operation I(it->second.first);
					prod.addOperand(I);
				}
				else
				{
					operation I(it->second.first, it->second.second);
					prod.addOperand(I);
				}
				if (it->first == 1)
					prod.addOperand(var);
				else
				{
					operation pow(opType::power);
					pow.addOperand(var);
					operation I(it->first);
					pow.addOperand(I);
					prod.addOperand(pow);
				}
				S.addOperand(prod);
			}

			it++;
		}

		return S;
	}

	/*
	surd::surd(rational base, rational power) : m_base(base), m_power(power)
	{
		OpPosition = opPosition::both;
		priority = 85;
		value = pow(m_base.getValue(), m_power.getValue());
	}

	SPop surd::simplify()
	{
		simpOperands();
		if (m_power.isInteger())
		{
			if (m_base.isInteger())
				return std::make_shared<integer>(pow(m_base.getNumerator(), m_power.getNumerator()));
			else
			{
				int T = pow(m_base.getNumerator(), m_power.getNumerator());
				int B = pow(m_base.getDenominator(), m_power.getNumerator());
				return std::make_shared<rational>(T, B);
			}
		}


		auto tPrimes = primeFactors(m_base.getNumerator());
		auto bPrimes = primeFactors(m_base.getDenominator());
		int tCoef = 1;
		int bCoef = 1;
		int n = m_power.getDenominator();
		auto it = tPrimes.begin();
		while (it != tPrimes.end())
		{
			int x = *it;
			if (std::count(tPrimes.begin(), tPrimes.end(), x) >= n)
			{
				tCoef *= x;
				it = tPrimes.erase(it, it + n);
			}
			else
				it++;
		}
		it = bPrimes.begin();
		while (it != bPrimes.end())
		{
			int x = *it;
			if (std::count(bPrimes.begin(), bPrimes.end(), x) >= n)
			{
				bCoef *= x;
				it = bPrimes.erase(it, it + n);
			}
			else
				it++;
		}
		int top = 1;
		int bottom = 1;
		for (int x : tPrimes)
			top *= x;
		for (int x : bPrimes)
			bottom *= x;

		rational coef(tCoef, bCoef);
		rational base(top, bottom);
		//NEEDS FIXING
		if (coef.isInteger() && base == integer(1))
			return std::make_shared<integer>(tCoef);
		else if (base == integer(1))
			return std::make_shared<rational>(coef);
		else if (coef == integer(1))
			return std::make_shared<surd>(base, rational(1, n));
		else if (coef.isInteger())
		{
			auto I = std::make_shared<integer>(tCoef);
			auto R = std::make_shared<surd>(base, rational(1, n));
			auto P = std::make_shared<product>();
			P->addOperand(I);
			P->addOperand(R);
			return P;
		}
		else
		{
			auto C = std::make_shared<rational>(coef);
			auto R = std::make_shared<surd>(base, rational(1, n));
			auto P = std::make_shared<product>();
			P->addOperand(C);
			P->addOperand(R);
			return P;
		}
	}
	std::vector<std::pair<wchar_t, charProps>> surd::write()
	{
		std::vector<std::pair<wchar_t, charProps>> result;
		charProps props;
		if (m_base == integer(1))
		{
			result.push_back({ L'1',props });
			return result;
		}
		if (m_power == integer(1))
			return m_base.write();
		if (!m_power.isInteger())
		{
			switch (m_power.getDenominator())
			{
			case 2:
				result.push_back({ L'√',props });
				break;
			case 3:
				result.push_back({ L'∛',props });
				break;
			case 4:
				result.push_back({ L'∜',props });
				break;
			default:
				props.superscript = true;
				auto x = std::to_wstring(m_power.getDenominator());
				for (auto k : x)
					result.push_back({ k,props });
				props.superscript = false;
				result.push_back({ L'√',props });
				break;
			}
		}
		if (m_base.isInteger())
		{
			auto x = std::to_wstring(m_base.getNumerator());
			for (auto k : x)
				result.push_back({ k,props });
			if (m_power.getNumerator() != 1)
			{
				props.superscript = true;
				x = std::to_wstring(m_power.getNumerator());
				for (auto k : x)
					result.push_back({ k,props });
				props.superscript = false;
			}
		}
		else
		{
			bool condition = m_base.getNumerator() == 1;
			if (!condition)
				result.push_back({ L'(',props });
			auto R = m_base.write();
			result.insert(result.end(), R.begin(), R.end());
			if (!condition)
				result.push_back({ L')',props });

			if (m_power.getNumerator() != 1)
			{
				props.superscript = true;
				auto x = std::to_wstring(m_power.getNumerator());
				for (auto k : x)
					result.push_back({ k,props });
				props.superscript = false;
			}
		}
		return result;
	}
	*/
}