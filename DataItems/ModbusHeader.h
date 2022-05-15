#pragma once
#include "IModbusElement.h"

class ModBusHeader : public IModBusElement
{
private:
	char16_t _transactionId;
	char16_t _protocolId;
	char16_t _length;
	unsigned char _unitId;

	const int TotalHeaderSize = 7;
	const int SizeOFUnit = 1;
	const int SizeOfFunctionCode = 1;

public:
	ModBusHeader()
	{
		_transactionId = 0;
		_protocolId = 0;
		_length = 0;
		_unitId = 0;
	}

	ModBusHeader(
		char16_t transactionId,
		char16_t protocolId,
		char16_t length,
		unsigned char unitId
	)
	{
		if (length > 253)
			throw std::exception("Length size can not be more that 253 bytes");

		length += SizeOFUnit + SizeOfFunctionCode;

		_transactionId = transactionId;
		_protocolId = protocolId;
		_length = length;
		_unitId = unitId;
	}

	int GetSize() {
		return TotalHeaderSize;
	}

	unsigned char* GetByteArray()
	{
		unsigned char* header = new unsigned char[7];

		unsigned char* transactionId = NotationConvertor::ToByteArray(_transactionId);
		header[0] = transactionId[0];
		header[1] = transactionId[1];

		unsigned char* protocolId = NotationConvertor::ToByteArray(_protocolId);
		header[2] = protocolId[0];
		header[3] = protocolId[1];

		unsigned char* length = NotationConvertor::ToByteArray(_length);
		header[4] = length[0];
		header[5] = length[1];

		header[6] = _unitId;

		return header;
	}
};