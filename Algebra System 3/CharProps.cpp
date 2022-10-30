#include <pch.h>
#include "CharProps.h"

bool operator==(const charProps& lhs, const charProps& rhs)
{
	return lhs.bold == rhs.bold && lhs.underline == rhs.underline && lhs.subscript == rhs.subscript && lhs.superscript == rhs.superscript;
}

bool operator!=(const charProps& lhs, const charProps& rhs)
{
	return !(lhs == rhs);
}

charProps charProps::operator+(const charProps& rhs)
{
	charProps result;
	result.bold = bold || rhs.bold;
	result.underline = underline || rhs.underline;
	result.superscript = superscript || rhs.superscript;
	result.subscript = subscript || rhs.subscript;
	if (result.superscript && result.subscript)
	{
		result.superscript = false;
		result.subscript = false;
	}
	return result;
}

void charProps::operator+=(const charProps& rhs)
{
	*this = *this + rhs;
}
