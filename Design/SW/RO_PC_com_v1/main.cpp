#define WIN32_LEAN_AND_MEAN

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"





#include "SerialPort.h"
#include "dataParser.h"
#include <stdio.h>
#include <string.h>
#include <thread>


using namespace std;

//Define COM port name
char* portName = "\\\\.\\COM3";

//Define buffer length
#define MAX_DATA_LENGTH 255
#define RcvBufferLength 512

//SerialPort object
SerialPort *COM_Port;

//Parser object
dataParser* Parser;

void transmitData(SerialPort* COM_Port,socketHandler* Socket) {
    printf("Transmitter thread successfully started\n");
    char rcvBuffer[RcvBufferLength];

    //Waiting for actual data
    bool waitForData = false;
    uint16_t dataLength = 0;

    while(1) {
        Socket->receiveSocket(rcvBuffer,RcvBufferLength);
        if (waitForData) {
            COM_Port->writeSerialPortEnhanced(rcvBuffer,dataLength);
            waitForData = false;
        }
        else {
            dataLength = (uint16_t) rcvBuffer[0];
            dataLength += (uint16_t) rcvBuffer[1] << 8;
            waitForData = true;
            //printf("Preamble (%u)\n",dataLength);
        }

    }
    return;
}

void receiveData(SerialPort* COM_Port,socketHandler* Socket) {
    printf("Receiver thread successfully started\n");

    // Create new data parser
    Parser = new dataParser(Socket);

    // Create data buffer
    unsigned char incomingData[MAX_DATA_LENGTH];

    // Read data
    while(1) {
        int readBytes = COM_Port->readSerialPortEnhanced(incomingData, MAX_DATA_LENGTH);
        if (readBytes) {
            for (int i = 0; i<readBytes;i++) {
                Parser->addData(incomingData[i]);
            }
        }
    }
    return;
}



int main()
{

    //Welcome msg
    printf("****************************************\n");
    printf("*             Reflow oven              *\n");
    printf("*                                      *\n");
    printf("*         Communication module         *\n");
    printf("*       Full duplex asynchronous       *\n");
    printf("*                                      *\n");
    printf("*       Created by: Tamas Polgar       *\n");
    printf("*                2021                  *\n");
    printf("****************************************\n\n");

    // Create socket
    socketHandler* Socket = new socketHandler();
    Socket->initSocket();

    // Open COM Port
    COM_Port = new SerialPort(portName);
    while (!COM_Port->isConnected());
    printf("COM port 3 connected\n");

    // Start receiver thread
    thread receiveThread(receiveData,COM_Port,Socket);

    // Start transmitter thread
    thread transmitThread(transmitData,COM_Port,Socket);

    receiveThread.join();
    //transmitThread.join();
    return 0;

}

