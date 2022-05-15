#pragma once

static class NotationConvertor 
{
public:
	static unsigned char* ToByteArray(char16_t char16t)
	{
		return ConvertToByteArray(char16t, sizeof(char16_t));
	}

	static char16_t ConvertToChar16(unsigned char* bytes)
	{
		char16_t value = 0;
		for (int i = 2; i >= 0; i--)
		{
			value = value << 8;
			value |= bytes[i];
		}

		return value;
	}

	static char32_t ConvertToChar32(unsigned char* bytes)
	{
		char32_t value = 0;
		for (int i = 4; i > 0; i--)
		{
			value = value << 8;
			value |= bytes[i];
		}

		return value;
	}

private:
	static unsigned char* ConvertToByteArray(unsigned long value, int length)
	{
		unsigned char* bytes = new unsigned char[length];
		for (int i = 0; i < length; i++)
			bytes[i] = static_cast<int>(value >> 8 * i & 0xff);;

		return bytes;
	}
};