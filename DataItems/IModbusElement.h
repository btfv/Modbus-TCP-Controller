#pragma once

class IModBusElement
{
public:
	virtual unsigned char* GetByteArray() = 0;
	virtual int GetSize() = 0;
};