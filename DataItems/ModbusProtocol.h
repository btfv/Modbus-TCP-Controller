#pragma once
#include <string>
#include "NotationConvertor.h"
#include "ModbusHeader.h"
#include "ModbusPDU.h"
#include "Utils.h"

class ModBusProtocol
{
private:
	ModBusHeader* _header;
	ModBusPDU* _PDU;

public:
	ModBusProtocol(
		char16_t transactionId,
		char16_t protocolId,
		char16_t length,
		unsigned char unitId,
		FunctionCode functionCode,
		unsigned char* data,
		bool isError = false
	)
	{
		_header = new ModBusHeader(
			transactionId,
			protocolId,
			length,
			unitId);

		_PDU = new ModBusPDU(functionCode, data, length, isError);
	}

	ModBusProtocol(unsigned char* command)
	{
		char16_t transactionId = NotationConvertor::ConvertToChar16(Util::GetPartOfArray(command, 0, 1));
		char16_t protocolId = NotationConvertor::ConvertToChar16(Util::GetPartOfArray(command, 2, 3));
		char16_t length = NotationConvertor::ConvertToChar16(Util::GetPartOfArray(command, 4, 5)) - 2;
		unsigned char unitId = command[6];
		FunctionCode functionCode = static_cast<FunctionCode>(command[7]);
		unsigned char* data = Util::GetPartOfArray(command, 8, length + 8);

 		_header = new ModBusHeader(
			transactionId,
			protocolId,
			length,
			unitId);

		_PDU = new ModBusPDU(functionCode, data, length, false);
	}

	int GetTotalSize() {
		return _header->GetSize() + _PDU->GetSize();
	}

	unsigned char GetFunctionCode()
	{
		return _PDU->GetFunctionCode();
	}

	unsigned char* GetData()
	{
		return _PDU->GetData();
	}

	int GetDataSize()
	{
		return _PDU->GetDataSize();
	}

	unsigned char* GetCommand() {
		unsigned char* command = new unsigned char[_header->GetSize() + _PDU->GetSize()];

		unsigned char* headerByteArray = _header->GetByteArray();
		for (int i = 0; i < _header->GetSize(); i++)
			command[i] = headerByteArray[i];

		unsigned char* PDUByteArray = _PDU->GetByteArray();
		for (int i = _header->GetSize(), j = 0; j < _PDU->GetSize(); i++, j++)
			command[i] = PDUByteArray[j];

		return command;
	}
};