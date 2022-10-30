#pragma once
#include <string>
#include "CharProps.h"

namespace algebra
{
	void analyse(std::wstringstream& WS);

	//Get number of occurances of given character in string
	int countCharacter(const std::wstringstream& WS, const wchar_t character);
}