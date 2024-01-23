#pragma once

#include <string>
#include <Windows.h>
#include <memory>

class SerialPort
{
public:
	~SerialPort();

	bool Open(const std::string& portName, int baudRate = 9600);
	void Close();

	bool Write(const std::string& message);
	bool Write(const char* data, size_t size);

	std::unique_ptr<char[]> Read(size_t size);
	HANDLE GetSerialPortHandle() const { return m_SerialPort; }
private:
	HANDLE m_SerialPort = NULL;
	std::string m_PortName;
};