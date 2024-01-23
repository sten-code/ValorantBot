#include "serialport.h"

#include <iostream>
#include <string>

#include <Windows.h>

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::Open(const std::string& portName, int baudRate)
{
    Close(); // Close the existing serial port if it's already connected
    m_PortName = portName;

    // Open the serial port
    m_SerialPort = CreateFileA(
        portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (m_SerialPort == INVALID_HANDLE_VALUE)
        return false;

    // Configure the serial port settings (baud rate, data bits, parity, stop bits)
    DCB serialParams = { 0 };
    serialParams.DCBlength = sizeof(serialParams);
    GetCommState(m_SerialPort, &serialParams);
    serialParams.BaudRate = baudRate;
    serialParams.ByteSize = 8;          // 8 data bits
    serialParams.Parity = NOPARITY;     // No parity
    serialParams.StopBits = ONESTOPBIT; // 1 stop bit
    SetCommState(m_SerialPort, &serialParams);
    PurgeComm(m_SerialPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
 
	return true;
}

void SerialPort::Close()
{
    // Only close the serial port if it's assigned a value
    if (m_SerialPort != NULL)
        CloseHandle(m_SerialPort);
}

bool SerialPort::Write(const std::string& message)
{
    // Write data to the serial port
    DWORD bytesWritten;
    return WriteFile(m_SerialPort, message.c_str(), message.length(), &bytesWritten, nullptr);
}

bool SerialPort::Write(const unsigned char* bytes, size_t size)
{
    // Write data to the serial port
    DWORD bytesWritten;
    return WriteFile(m_SerialPort, bytes, size, &bytesWritten, nullptr);
}

bool SerialPort::Write(const char* chars, size_t size)
{
    // Write data to the serial port
    DWORD bytesWritten;
    return WriteFile(m_SerialPort, chars, size, &bytesWritten, nullptr);
}

unsigned char* SerialPort::Read(size_t size)
{
    // Allocate an extra byte for the null terminator
    unsigned char* buffer = new unsigned char[size + 1];
    DWORD bytesRead;

    if (ReadFile(m_SerialPort, buffer, size, &bytesRead, nullptr)) 
    {
        buffer[bytesRead] = '\0'; // Null-terminate the string
        return buffer;
    }
    return nullptr;
}
