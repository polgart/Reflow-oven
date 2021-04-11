/*
* Author: Manash Kumar Mandal
* Modified Library introduced in Arduino Playground which does not work
* This works perfectly
* LICENSE: MIT
*/

#include "SerialPort.h"

SerialPort::SerialPort(const char *portName)
{
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);
    if (this->handler == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            std::cerr << "ERROR: Handle was not attached.Reason : " << portName << " not available\n";
        }
        else
        {
            std::cerr << "ERROR!!!\n";
        }
    }
    else
    {
        DCB dcbSerialParameters = {0};

        if (!GetCommState(this->handler, &dcbSerialParameters))
        {
            std::cerr << "Failed to get current serial parameters\n";
        }
        else
        {
            dcbSerialParameters.BaudRate = CBR_19200;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
                std::cout << "ALERT: could not set serial port parameters\n";
            }
            else
            {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
            }
        }
    }
}

SerialPort::~SerialPort()
{
    if (this->connected)
    {
        this->connected = false;
        CloseHandle(this->handler);
    }
}

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int SerialPort::readSerialPort(const unsigned char *buffer, unsigned int buf_size)
{
    DWORD bytesRead{};
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0)
    {
        if (this->status.cbInQue > buf_size)
        {
            toRead = buf_size;
        }
        else
        {
            toRead = this->status.cbInQue;
        }
    }

    memset((void*) buffer, 0, buf_size);

    if (ReadFile(this->handler, (void*) buffer, toRead, &bytesRead, NULL))
    {
        return bytesRead;
    }

    return 0;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool SerialPort::writeSerialPort(const char *buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*) buffer, buf_size, &bytesSend, 0))
    {
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    //printf("Bytes sent: %lu",bytesSend);
    return true;
}

// Checking if serial port is connected
bool SerialPort::isConnected()
{
    if (!ClearCommError(this->handler, &this->errors, &this->status))
    {
        this->connected = false;
    }

    return this->connected;
}

void SerialPort::closeSerial()
{
    CloseHandle(this->handler);
}

int SerialPort::readSerialPortEnhanced(const unsigned char *buffer, unsigned int buf_size) {
    memset((void*) buffer, 0, buf_size);

    DWORD dwRead;
    BOOL fWaitingOnRead = FALSE;
    OVERLAPPED osReader = {0};

    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osReader.hEvent == NULL) {
        // Error creating overlapped event; abort.
        printf("Error creating overlapped event; abort.\n");
    }

    if (!fWaitingOnRead) {
        // Issue read operation.
        if (!ReadFile(this->handler, (void *) buffer, 1, &dwRead, &osReader)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                printf("COM Port reading error\n");
                return 0;
            } else {
                fWaitingOnRead = TRUE;
                //printf("Waiting on read\n");
            }
        } else {
            // read completed immediately
            //printf("Read completed immediately. (%d)\n",dwRead);
            return dwRead;
        }
    }

    DWORD dwRes;

    if (fWaitingOnRead) {
        dwRes = WaitForSingleObject(osReader.hEvent, INFINITE);
        switch(dwRes)
        {
            // Read completed.
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(this->handler, &osReader, &dwRead, FALSE)) {
                    printf("COM Port reading error\n");
                    return 0;
                }
                else {
                    //printf("Read completed. (%d)\n",dwRead);
                    fWaitingOnRead = FALSE;
                    return dwRead;
                }
                break;

            case WAIT_TIMEOUT:
                // Operation isn't complete yet. fWaitingOnRead flag isn't
                // changed since I'll loop back around, and I don't want
                // to issue another read until the first one finishes.
                //
                // This is a good time to do some background work.
                break;

            default:
                // Error in the WaitForSingleObject; abort.
                // This indicates a problem with the OVERLAPPED structure's
                // event handle.
                break;
        }
    }

    return 0;
}

bool SerialPort::writeSerialPortEnhanced(const char *buffer, unsigned int buf_size) {
    OVERLAPPED osWrite = {0};
    DWORD dwWritten;
    DWORD dwRes;
    BOOL fRes;

    // Create this write operation's OVERLAPPED structure's hEvent.
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osWrite.hEvent == NULL) {
        printf("Error creating overlapped event handle\n");
        return FALSE;
    }

    // Issue write.
    if (!WriteFile(this->handler, (void*)buffer, buf_size, &dwWritten, &osWrite)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            printf("WriteFile failed, but isn't delayed. Report error and abort.\n");
            fRes = FALSE;
        }
        else {
            //printf("Write is pending.\n");
            dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);

            switch(dwRes)
            {
                // OVERLAPPED structure's event has been signaled.
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(this->handler, &osWrite, &dwWritten, FALSE))
                        fRes = FALSE;
                    else
                        //printf("Write operation completed successfully.");
                        fRes = TRUE;
                    break;

                default:
                    printf("An error has occurred in WaitForSingleObject.\n");
                    printf("This usually indicates a problem with the\n");
                    printf("OVERLAPPED structure's event handle.\n");
                    fRes = FALSE;
                    break;
             }
        }
    }
    else {
        //printf("WriteFile completed immediately.\n");
        fRes = TRUE;
    }

    CloseHandle(osWrite.hEvent);
    return fRes;
}
