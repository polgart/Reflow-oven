/*
* Author: Manash Kumar Mandal
* Modified Library introduced in Arduino Playground which does not work
* This works perfectly
* LICENSE: MIT
*/

#ifndef RO_PC_COM_V1_SERIALPORT_H
#define RO_PC_COM_V1_SERIALPORT_H

#define MAX_DATA_LENGTH 255

#include <windows.h>
#include <iostream>

class SerialPort
{
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;
public:
    explicit SerialPort(const char *portName);
    ~SerialPort();

    int readSerialPort(const unsigned char *buffer, unsigned int buf_size);
    int readSerialPortEnhanced(const unsigned char *buffer, unsigned int buf_size);
    bool writeSerialPortEnhanced(const char *buffer, unsigned int buf_size);
    bool writeSerialPort(const char *buffer, unsigned int buf_size);
    bool isConnected();
    void closeSerial();
};

#endif //RO_PC_COM_V1_SERIALPORT_H
