#pragma once
#include "IModbusElement.h"
#include "FunctionCodes.h"
#include <exception>

class ModBusPDU : public IModBusElement
{
private:
	unsigned char _functionCode;
	unsigned char* _data;
	int _dataLength;
public:
	ModBusPDU(FunctionCode code, unsigned char* data, int length, bool isError)
	{
		if (length > 253) throw std::exception("Payload size can not be more than 253 bytes");

		_functionCode = isError ? code + 128 : code;
		_data = data;
		_dataLength = length;
	}

	int GetSize()
	{
		return _dataLength + 1;
	}

	int GetDataSize()
	{
		return _dataLength;
	}

	unsigned char GetFunctionCode()
	{
		return _functionCode;
	}

	unsigned char* GetData()
	{
		return _data;
	}

	unsigned char* GetByteArray()
	{
		unsigned char* PDU = new unsigned char[_dataLength + 1];
		PDU[0] = _functionCode;

		for (int i = 0; i < _dataLength; i++)
		{
			PDU[i + 1] = _data[i];
		}

		return PDU;
	}
};