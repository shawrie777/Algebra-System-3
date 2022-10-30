#pragma once
#include <TOM.h>

struct charProps
{
	bool bold;
	bool underline;
	bool subscript;
	bool superscript;

	charProps() : bold(false), underline(false), subscript(false), superscript(false) {};
	charProps(winrt::Windows::UI::Text::ITextCharacterFormat format)
	{
		bold = format.Bold()== winrt::Windows::UI::Text::FormatEffect::On;
		underline = format.Underline() == winrt::Windows::UI::Text::UnderlineType::Single;
		subscript = format.Subscript() == winrt::Windows::UI::Text::FormatEffect::On;
		superscript = format.Superscript() == winrt::Windows::UI::Text::FormatEffect::On;
	}

	charProps operator+(const charProps& rhs);
	void operator+=(const charProps& rhs);
};

bool operator==(const charProps& lhs, const charProps& rhs);
bool operator!=(const charProps& lhs, const charProps& rhs);
