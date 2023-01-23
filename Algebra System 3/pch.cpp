#include "pch.h"

std::vector<int> factors(int x)
{
	std::vector<int> result;

	x = abs(x);
	int root = floor(sqrt(x));

	if (x == 0 || x == 1)
		result.push_back(x);
	else
	{
		for (int i = 2; i <= root; i++)
		{
			if (x % i == 0)
			{
				result.push_back(i);
				result.push_back(x / i);
			}
		}
		if (root * root == x)
			result.push_back(root);
		result.push_back(1);
		result.push_back(x);
	}

	std::sort (result.begin(), result.end());
	result.erase(std::unique(result.begin(), result.end()), result.end());

	return result;
}

std::vector<int> primeFactors(int x)
{
	std::vector<int> result;
	x = abs(x);
	int root = floor(sqrt(x)) + 1;

	if (x < 3)
		result.push_back(x);
	else
	{
		int factor = 2;
		while (factor < root)
		{
			if (x % factor == 0)
			{
				result.push_back(factor);
				x /= factor;
			}
			else
				factor++;
		}
	}
	return result;
}
