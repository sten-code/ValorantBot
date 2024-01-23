#pragma once

#include <string>
#include <Windows.h>

class SerialPort
{
public:
	~SerialPort();

	bool Open(const std::string& portName, int baudRate = 9600);
	void Close();

	bool Write(const std::string& message);
	bool Write(const unsigned char* bytes, size_t size);
	bool Write(const char* chars, size_t size);

	unsigned char* Read(size_t size);
	HANDLE GetSerialPortHandle() const { return m_SerialPort; }
private:
	HANDLE m_SerialPort = NULL;
	std::string m_PortName;
};