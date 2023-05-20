//
// Created by Максим on 16.05.2023.
//
#include "LN.h"
#include "return_codes.h"

#include <deque>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Wrong number of arguments expected 2\n");
		return ERROR_PARAMETER_INVALID;
	}
	try
	{
		ifstream in(argv[1], std::ifstream::binary);
		if (!in)
		{
			fprintf(stderr, "Cannot open input file\n");
			return ERROR_CANNOT_OPEN_FILE;
		}
		string str = string("");
		deque< LN > sta = deque< LN >();
		LN ans = LN(1);
		while (getline(in, str))
		{
			if (str.at(str.length() - 1) == '\r')
			{
				str.pop_back();
			}
			if (str == string("+") || str == "+=")
			{
				ans = sta.at(sta.size() - 1) + sta.at(sta.size() - 2);
			}
			else if (str == "-" || str == "-=")
			{
				ans = sta.at(sta.size() - 1) - sta.at(sta.size() - 2);
			}
			else if (str == "*" || str == "*=")
			{
				ans = sta.at(sta.size() - 1) * sta.at(sta.size() - 2);
			}
			else if (str == "/" || str == "/=")
			{
				ans = sta.at(sta.size() - 1) / sta.at(sta.size() - 2);
			}
			else if (str == "%" || str == "%=")
			{
				ans = sta.at(sta.size() - 1) % sta.at(sta.size() - 2);
			}
			else if (str == "==")
			{
				ans = LN(sta.at(sta.size() - 1) == sta.at(sta.size() - 2));
			}
			else if (str == ">=")
			{
				ans = LN(sta.at(sta.size() - 1) >= sta.at(sta.size() - 2));
			}
			else if (str == "<=")
			{
				ans = LN(sta.at(sta.size() - 1) <= sta.at(sta.size() - 2));
			}
			else if (str == ">")
			{
				ans = LN(sta.at(sta.size() - 1) > sta.at(sta.size() - 2));
			}
			else if (str == "<")
			{
				ans = LN(sta.at(sta.size() - 1) < sta.at(sta.size() - 2));
			}
			else if (str == "!=")
			{
				ans = LN(sta.at(sta.size() - 1) != sta.at(sta.size() - 2));
			}
			else if (str == "~")
			{
				ans = ~sta.at(sta.size() - 1);
			}
			else if (str == "_")
			{
				ans = -sta.at(sta.size() - 1);
			}
			else
			{
				const char* sti = str.c_str();
				sta.emplace_back(sti);
				continue;
			}
			if (str == "+" || str == "-" || str == "*" || str == "/" || str == "==" || str == ">=" || str == "<=" || str == "<" ||
				str == ">" || str == "!=" || str == "%" || str == "+=" || str == "-=" || str == "*=" || str == "/=" || str == "%=")
			{
				sta.pop_back();
				sta.pop_back();
				sta.push_back(ans);
			}
			else
			{
				sta.pop_back();
				sta.push_back(ans);
			}
		}
		in.close();
		FILE* f = ::fopen(argv[2], "w");
		if (!f)
		{
			fprintf(stderr, "Cannot open output file\n");
			return ERROR_CANNOT_OPEN_FILE;
		}
		for (size_t i = 0; i < sta.size(); i++)
		{
			::fprintf(f, "%s\n", sta.at(sta.size() - i - 1).toString());
		}
		fclose(f);
	} catch (int x)
	{
		if (x == 2)
		{
			fprintf(stderr, "Not enough memory\n");
			return ERROR_OUT_OF_MEMORY;
		}
	}
	return SUCCESS;
}
