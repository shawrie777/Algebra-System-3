#pragma once
#include <vector>
#include <memory>
#include <variant>
#include "CharProps.h"
#include <set>
#include <bitset>
#include "EqTypes.h"

namespace algebra
{
	enum class opType
	{
		empty,
		set,
		isIn,
		constant,
		integer,
		rational,
		complex,
		variable,
		sum,	
		product,
		power,
		GCD,
		equation
	};

	enum class opPosition
	{
		none,
		before,
		after,
		both
	};

	class operation
	{
		friend class polynomial;
		friend bool in_lessThan(operation& lhs, operation& rhs);
		friend bool in_equal(operation& lhs, operation& rhs);
		friend bool operator<(const operation& lhs, const operation& rhs);
		friend bool operator>(const operation& lhs, const operation& rhs);
		friend bool operator==(const operation& lhs, const operation& rhs);
		friend bool operator!=(const operation& lhs, const operation& rhs);
	private:
		int operandCount = 0;
		std::vector<operation> operands;
		int priority = 101;
		opType Type;
		opPosition OpPosition = opPosition::none;
		std::variant<int, std::pair<int, int>, double> value;
		wchar_t Symbol = L' ';		

		void gatherTerms();
		void distribute();
		void factorise();
		void splitFrac();
		void simpGCD();
		void solveEquation();
		eqType getEqType();
		std::set<wchar_t> getVariables();

	public:
		operation(opType type);
		operation(int intVal);
		operation(int numerator, int denominator);
		//for creating constants
		operation(wchar_t symbol, double val);
		//for creating variables
		operation(wchar_t symbol);

		const opType type();
		const opPosition getPosition();		
		const int getPriority();
		void addOperand(operation op);
		void setBracketCount(int depth);
		bool isNumber() const;
		void unNest();
		void negate();
		void invert();

		bool hasOperands();
		bool isPolynomial();

		//Checks if any operand in the structure is of the given type
		bool contains(opType O);
		
		std::variant<int, std::pair<int, int>, double> getValue();
		
		operation simplify();
		std::vector<std::pair<wchar_t, charProps>> write();
	};

	class polynomial
	{
	private:
		std::map<int, std::pair<int, int>> terms;
		std::map<int, std::pair<int, int>> derivative;
		int order = 0;
		wchar_t symbol = L' ';

		polynomial(std::map<int, std::pair<int, int>> Terms);
	public:
		polynomial(operation op);
		bool testValue(operation val);
		std::vector<operation> ratRoots();
		operation ratFactor();
		const wchar_t getSymbol() const { return symbol; };

		operation ToOperation();
	};
}
