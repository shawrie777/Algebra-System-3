﻿#include <pch.h>
#include <regex>
#include "MainPage.h"
#include <set>

using winrt::Algebra_System_3::implementation::main_page;

namespace algebra
{
	void analyse(std::wstringstream& WS)
	{
		std::vector<std::pair<wchar_t, charProps>> output;
		
		int openCount = countCharacter(WS, L'(');
		int closeCount = countCharacter(WS, L')');
		if (openCount - closeCount > 1)
			throw("Unmatched brackets");

		std::vector <std::pair<operation, bool>> operations;
		int bracketDepth = 0;
		int maxDepth = 0;

		std::wstring str = WS.str();
		bool neg = false;
		std::vector<int> inv;
		//tokenizing
		//set all tokens to search for here
		std::wregex tokens(L"∈|i|\\{|\\}|=|\\^|\\(|\\)|GCD|,|e|π|φ|\\*|-|/|\\+|([0-9]+\\.?[0-9]*)|[a-zA-ZΑ-Ωα-ω]");
		auto begin = std::wsregex_iterator(str.begin(), str.end(), tokens);
		auto end = std::wsregex_iterator();

		for (std::wsregex_iterator i = begin; i != end; ++i)
		{
			std::wsmatch match = *i;
			std::wstring token = match.str();

			operation op(opType::empty);
			//process each token here
			if (iswdigit(*token.begin()))
			{
				auto found = std::find(token.begin(), token.end(), L'.');
				if (found == token.end())
				{
					int I = std::stoi(token);
					op = operation(I);
				}
				else
				{
					int decimals = token.end() - found;
					double d = std::stod(token);
					int top = d * pow(10.0, decimals);
					int bottom = pow(10.0, decimals);
					op = operation(top, bottom);
				}
			}
			else if (token == L"i")
				op = operation(opType::complex);
			else if (token == L"+")
				op = operation(opType::sum);
			else if (token == L"-")
			{
				neg = true;
				if (!operations.empty() && (operations.back().first.getPosition() == opPosition::before ||
					operations.back().first.getPosition() == opPosition::none || operations.back().first.hasOperands()))
					op = operation(opType::sum);
				else
					continue;
			}
			else if (token == L",")
				op = operation(opType::set);
			else if (token == L"GCD")
				op = operation(opType::GCD);
			else if (token == L"=")
				op = operation(opType::equation);
			else if (token == L"e")
				op = constants::E;
			else if (token == L"φ")
				op = constants::phi;
			else if (token == L"π")
				op = constants::pi;
			else if (token == L"*")
				op = operation(opType::product);
			else if (token == L"/")
			{
				op = operation(opType::product);
				inv.push_back(bracketDepth);
			}
			else if (token == L"(" || token == L"{")
			{
				if (neg)
				{
					operation O(-1);
					O.setBracketCount(bracketDepth);
					operations.push_back({ O,false });
					operation P(opType::product);
					P.setBracketCount(bracketDepth);
					operations.push_back({ P,false });
				}
				bracketDepth++;
				maxDepth = bracketDepth > maxDepth ? bracketDepth : maxDepth;
				continue;
			}
			else if (token == L")" || token == L"}")
			{
				bracketDepth--;
			}
			else if (token == L"^")
				op = operation(opType::power);
			else if (token == L"∈")
				op = operation(opType::isIn);
			else
				op = operation(*token.begin());

			
			if (token != L"-" && neg)
			{
				op.negate();
				neg = false;
			}
			if (token != L"/" && !inv.empty() && inv.back() == bracketDepth)
			{
				//op.invert();
				op.setBracketCount(bracketDepth);
				operations.push_back({ op,false });
				operation P(opType::power);
				P.setBracketCount(bracketDepth);
				operations.push_back({ P,false });
				operation o(-1);
				o.setBracketCount(bracketDepth);
				operations.push_back({ o,false });
				inv.pop_back();
				continue;
			}

			op.setBracketCount(bracketDepth);
			operations.push_back({ op,false });
		}

		if (!inv.empty() && inv.back() == bracketDepth)
		{
			operation P(opType::power);
			P.setBracketCount(bracketDepth);
			operations.push_back({ P,false });
			operation o(-1);
			o.setBracketCount(bracketDepth);
			operations.push_back({ o,false });
			inv.pop_back();
		}

		//stack operations into a single operation based on priority
		while (operations.size() > 1)
		{
			auto found = operations.end();
			for (auto it = operations.begin(); it != operations.end(); it++)
			{
				if (it->second == false && (found == operations.end() || it->first > found->first))
					found = it;
			}

			bool endCheck = found == operations.end();

			if (!endCheck && found->first.type() == opType::empty)
			{
				operations.erase(found);
				continue;
			}

			if (endCheck || found->first.getPriority() < 75 + 100 * maxDepth)
			{
				auto it = operations.begin() + 1;
				while (it != operations.end())
				{
					if (endCheck || (it->second && (it - 1)->second && (ceil(it->first.getPriority()/100.0f) == ceil((it-1)->first.getPriority()/100.0f)
						/*&& ceil(it->first.getPriority() / 100.0f) < maxDepth + 2*/)))
					{
						operation P(opType::product);
						P.setBracketCount(maxDepth);
						it = operations.insert(it, { P,false });
						it += 2;
						continue;
					}
					else
						it++;
				}
				if (!endCheck)
					maxDepth--;				
				continue;
			}
			
			found->second = true;
			if (!found->first.hasOperands())
			{
				switch (found->first.getPosition())
				{
				case opPosition::both:
					found->first.addOperand((found - 1)->first);
					found->first.addOperand((found + 1)->first);
					operations.erase(found + 1);
					operations.erase(found - 1);
					break;
				case opPosition::after:
					found->first.addOperand((found + 1)->first);
					operations.erase(found + 1);
					break;
				default:
					break;
				}
			}
		}

		output = operations[0].first.simplify().write();

		if (output[0].first == L'/')
			output.insert(output.begin(), { L'1',charProps() });

		main_page->WriteOutput(output);
	}		

	//Get number of occurances of given character in string
	int countCharacter(const std::wstringstream& WS, const wchar_t character)
	{
		std::wstring str = WS.str();
		return std::count(str.begin(), str.end(), character);
	}

}