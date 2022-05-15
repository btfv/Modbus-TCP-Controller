#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <hidapi.h>
#include <stdint.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <iostream>

#include "../DataItems/Commands.h"
#include "../DataItems/Pins.h"
#include "../DataItems/NotationConvertor.h"
#include "../DataItems/Constants.h"
#include "../DataItems/ModbusProtocol.h"
#include "ModbusRequestHandler.h"

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void PrintControllerInfo(hid_device* handle);

int main(int argc, char* argv[])
{
	unsigned char data[MAXBYTE];

	cout << "Hidapi test/example tool. Compiled with hidapi version " << HID_API_VERSION_STR << ", runtime version " << hid_version_str() << endl;

	if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH)
	{
		cout << "Compile-time version matches runtime version of hidapi." << endl;
	}
	else
	{
		cout << "Compile-time version is different than runtime version of hidapi." << endl;
	}

	if (hid_init()) return -1;

	unsigned short vendorId = 0x1234;
	unsigned short productId = 0x0001;

	hid_device* handle = hid_open(vendorId, productId, NULL);
	if (!handle)
	{
		cout << "Unable to open device" << endl;
		return 1;
	}
	PrintControllerInfo(handle);

	memset(data, 0x00, sizeof(data));
	data[0] = Command::SetLEDColor;
	data[1] = 0xff;
	data[2] = 0xff;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0xff;
	data[6] = 0xff;

	int result = hid_send_feature_report(handle, data, 7);

	if (result == -1)
	{
		cout << "Hid_write error." << endl;
		return -1;
	}

	data[0] = Command::CheckActivePins;
	result = hid_get_feature_report(handle, data, 2);
	if (result < 0)
	{
		cout << "Unable to get a feature report." << endl;
		cout << hid_error(handle) << endl;
	}
	else
	{
		cout << "Feature Report" << endl;
		for (int i = 0; i < result; i++)
			cout << static_cast<int>(data[i]);
		cout << endl;
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockAddr.sin_port = htons(502);

	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	listen(servSock, 20);

	int nSize;
	SOCKET clntSock;

	while (true)
	{
		try
		{
			SOCKADDR* clntAddr = nullptr;

			nSize = sizeof(SOCKADDR);
			clntSock = accept(servSock, clntAddr, &nSize);
			recv(clntSock, (char*)data, MAXBYTE, NULL);

			ModBusProtocol modbus = ModBusProtocol(data);
			ModbusRequestHandler handler = ModbusRequestHandler(clntSock, handle);
			handler.HandleRequest(modbus);
		}
		catch (std::exception ex)
		{
			Util::LogError(ex);
		}

		closesocket(clntSock);
	}

	WSACleanup();
	return 0;
}

void PrintControllerInfo(hid_device* handle)
{
	wchar_t wstr[MAX_STR * 2];
	wstring string;

	wstr[0] = 0x0000;
	int res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	res < 0 ?
		cout << "Unable to read manufacturer string" << endl :
		cout << "Manufacturer String: " << Util::GetString(wstr) << endl;

	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	res < 0 ?
		cout << "Unable to read product string" << endl :
		cout << "Product String: " << Util::GetString(wstr) << endl;

	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	res < 0 ?
		cout << "Unable to read serial number string" << endl :
		cout << "Serial Number String: " << Util::GetString(wstr) << endl;

	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	res < 0 ?
		cout << "Unable to read indexed string 1" << endl :
		cout << "Indexed String 1: " << Util::GetString(wstr) << endl;
}