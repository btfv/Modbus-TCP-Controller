#pragma once
#include <exception>
#include <string>
#include <time.h>
#include <iostream>

using namespace std;

static class Util 
{
public:
	static unsigned char* GetPartOfArray(unsigned char* array, int start, int end)
	{
		if (end < start) throw std::exception("End point can not be before than start point");

		int newLength = end - start + 1;
		unsigned char* newArray = new unsigned char[newLength];

		for (int i = start, j = 0; j < newLength; i++, j++)
		{
			newArray[j] = array[i];
		}

		return newArray;
	}

	static string GetString(wchar_t* wstr)
	{
		wstring ws(wstr);
		string str(ws.begin(), ws.end());
		return str;
	}

	static void LogError(exception ex)
	{
		time_t now = time(0);
		cout << ctime(&now) << " Error: " << ex.what() << endl;
	}

	static char* IntToCharArray(int value)
	{
		int length = 0;
		int degree = 1;
		while (value / degree)
		{
			degree *= 10;
			length++;
		}

		char* result = new char[length + 1];
		for (int i = 0; i < length; i++)
		{
			result[length - 1 - i] = value % 10 + '0';
			value /= 10;
		}
		result[length] = '\0';
		return result;
	}
};