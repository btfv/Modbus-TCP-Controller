#pragma once

#include <ctime>
#include "../DataItems/ModbusProtocol.h"
#include "../DataItems/ErrorCodes.h"

using namespace std;

class ModbusRequestHandler
{
private:
    SOCKET _clntSock;
    hid_device* _handle;
    unsigned char buf[MAX_STR];
public:
    ModbusRequestHandler(SOCKET clntSock, hid_device* handle)
    {
        _clntSock = clntSock;
        _handle = handle;
    }

    char* HandleRequest(ModBusProtocol modbus)
    {
        switch (modbus.GetFunctionCode())
        {
        case FunctionCode::Ping: HandlePingCommand(); break;
        case FunctionCode::SetMaxLEDBrightness : HandleSetMaxLEDBrightnessCommand(); break;
        case FunctionCode::SetMinLEDBrightness: HandleSetMinLEDBrightnessCommand(); break;
        case FunctionCode::DrawDisplay: HandleDrawDisplayCommand(); break;
        case FunctionCode::PrintVoltage: HandleGetVoltageCommand(); break;
        default: throw std::exception("Invalid function code");
        }
    }

private:
    void HandlePingCommand()
    {
        std::string message = "Server is working";

        ModBusProtocol modbus = ModBusProtocol(0, 0, message.length() + 1, 0, FunctionCode::Ping, (unsigned char*)message.c_str());
        unsigned char* response = modbus.GetCommand();
        send(_clntSock, (char*)response, modbus.GetTotalSize(), NULL);
    }

    void HandleSetMaxLEDBrightnessCommand()
    {
        int length = 7;
        buf[0] = Command::SetLEDColor;
        for (int i = 1; i < length; i++)
            buf[i] = 0xff;

        string errorCode;
        bool isErrorResponse = false;
        try
        {
            hid_send_feature_report(_handle, buf, length);
        }
        catch(exception ex)
        {
            Util::LogError(ex);
            errorCode = ErrorCode::HardwareErrorCode;
            isErrorResponse = true;
        }

        ModBusProtocol modbus = ModBusProtocol(0, 0, errorCode.length() + 1, 0, FunctionCode::SetMaxLEDBrightness, (unsigned char*)errorCode.c_str(), isErrorResponse);
        unsigned char* response = modbus.GetCommand();
        send(_clntSock, (char*)response, modbus.GetTotalSize(), NULL);
    }

    void HandleSetMinLEDBrightnessCommand()
    {
        int length = 7;
        buf[0] = Command::SetLEDColor;
        for (int i = 1; i < length; i++)
            buf[i] = 0x00;

        string errorCode;
        bool isErrorResponse = false;
        try
        {
            hid_send_feature_report(_handle, buf, length);
        }
        catch (exception ex)
        {
            Util::LogError(ex);
            errorCode = ErrorCode::HardwareErrorCode;
            isErrorResponse = true;
        }
 

        ModBusProtocol modbus = ModBusProtocol(0, 0, errorCode.length() + 1, 0, FunctionCode::SetMinLEDBrightness, (unsigned char*)errorCode.c_str(), isErrorResponse);
        unsigned char* response = modbus.GetCommand();
        send(_clntSock, (char*)response, modbus.GetTotalSize(), NULL);
    }

    void HandleDrawDisplayCommand()
    {
        const int displayHeight = 64;
        const int displayWidth = 128;

        static bool color = 0;
        color = !color;
        buf[0] = Command::DrawPixel;
        int lenght = 4;

        string errorCode;
        bool isErrorResponse = false;
        try
        {
            for (unsigned char i = 0; i < displayHeight; i++)
            {
                for (unsigned char j = 0; j < displayWidth; j++)
                {
                    buf[1] = j;
                    buf[2] = i;
                    buf[3] = (unsigned char)color;
                    hid_send_feature_report(_handle, buf, lenght);
                }
            }
        }
        catch (exception ex)
        {
            Util::LogError(ex);
            errorCode = ErrorCode::HardwareErrorCode;
            isErrorResponse = true;
        }

        ModBusProtocol modbus = ModBusProtocol(0, 0, errorCode.length() + 1, 0, FunctionCode::DrawDisplay, (unsigned char*)errorCode.c_str(), isErrorResponse);
        unsigned char* response = modbus.GetCommand();
        send(_clntSock, (char*)response, modbus.GetTotalSize(), NULL);
    }

    void HandleGetVoltageCommand()
    {
        string message;
        string errorCode;
        bool isErrorResponse = false;
        try
        {
            buf[0] = Command::GetVoltage;
            hid_get_feature_report(_handle, buf, 3);

            unsigned char voltage[2];
            memcpy(&voltage, &buf[1], 2);
            char16_t voltageValue = NotationConvertor::ConvertToChar16(voltage);
            message = Util::UIntToCharArray(voltageValue);

            int length = 7;
            for (int i = 0; i < length / 2; i++)
            {
                buf[1 + i * 2] = voltage[0];
                buf[2 + i * 2] = voltage[1];
            }
            buf[0] = Command::SetLEDColor;
            hid_send_feature_report(_handle, buf, length);

            Sleep(228);
        }
        catch (exception ex)
        {
            Util::LogError(ex);
            errorCode = ErrorCode::HardwareErrorCode;
            isErrorResponse = true;
        }

        ModBusProtocol modbus = ModBusProtocol(0, 0, message.length() + 1, 0, FunctionCode::PrintVoltage, (unsigned char*)message.c_str(), isErrorResponse);
        unsigned char* response = modbus.GetCommand();
        send(_clntSock, (char*)response, modbus.GetTotalSize(), NULL);
    }
};