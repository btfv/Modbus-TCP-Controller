#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <iostream>

#include "../DataItems/ModbusProtocol.h"

#pragma comment(lib, "WS2_32")

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    try
    {
        bool run = true;
        while (run)
        {
            SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            sockaddr_in sockAddr;
            memset(&sockAddr, 0, sizeof(sockAddr));
            sockAddr.sin_family = PF_INET;
            sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            sockAddr.sin_port = htons(502);
            connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

            system("cls");
            std::cout << "Main menu: " << std::endl;
            std::cout << "1. PING" << std::endl;
            std::cout << "2. Set Max LED Brightness" << std::endl;
            std::cout << "3. Set Min LED Brightness" << std::endl;
            std::cout << "4. Get Voltage" << std::endl;
            std::cout << "5. Draw Display" << std::endl;
            std::cout << "6. Exit" << std::endl;

            char choice = 0;
            std::cin >> choice;

            ModBusProtocol* modbus = nullptr;
            switch (choice)
            {
            case '1':
            {
                modbus = new ModBusProtocol(0, 0, 0, 0, FunctionCode::Ping, NULL);
                break;
            }
            case '2':
            {
                modbus = new ModBusProtocol(0, 0, 0, 0, FunctionCode::SetMaxLEDBrightness, NULL);
                break;
            }
            case '3':
            {
                modbus = new ModBusProtocol(0, 0, 0, 0, FunctionCode::SetMinLEDBrightness, NULL);
                break;
            }
            case '4':
            {
                modbus = new ModBusProtocol(0, 0, 0, 0, FunctionCode::PrintVoltage, NULL);
                break;
            }
            case '5':
            {
                modbus = new ModBusProtocol(0, 0, 0, 0, FunctionCode::DrawDisplay, NULL);
                break;
            }
            case '6':
                run = false;
                continue;
            default:
                continue;
            }

            system("cls");

            if (modbus != nullptr)
            {
                unsigned char* command = modbus->GetCommand();
                send(sock, (char*)command, modbus->GetTotalSize(), NULL);

                char data[MAXBYTE];
                recv(sock, data, MAXBYTE, NULL);
                ModBusProtocol response = ModBusProtocol((unsigned char*)data);

                if (response.GetFunctionCode() == modbus->GetFunctionCode() + 128)
                {
                    std::cout << "Server responsed error: " << response.GetData() << std::endl;
                }
                else
                {
                    strcmp((char*)response.GetData(), "") ?
                        std::cout << "Server response: " << response.GetData() << std::endl:
                        std::cout << "Server response: success!" << std::endl;
                }

                std::cin >> choice;

                delete modbus;
            }

            closesocket(sock);
        }
    }
    catch (std::exception ex)
    {
        std::cout << "Connection is broken. " << ex.what() << std::endl;
    }

    WSACleanup();
    system("pause");
    return 0;
}