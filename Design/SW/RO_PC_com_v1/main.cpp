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
    while(1) {
        Socket->receiveSocket(rcvBuffer,RcvBufferLength);
        //printf("%s (%d)\n",rcvBuffer,strlen(rcvBuffer));
        COM_Port->writeSerialPort(rcvBuffer,strlen(rcvBuffer));
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
        int readBytes = COM_Port->readSerialPort(incomingData, MAX_DATA_LENGTH);
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
    // Create socket
    socketHandler* Socket = new socketHandler();
    Socket->initSocket();

    // Open COM Port
    COM_Port = new SerialPort(portName);
    while (!COM_Port->isConnected());
    printf("COM port connected\n");

    // Start receiver thread
    thread receiveThread(receiveData,COM_Port,Socket);

    // Start transmitter thread
    thread transmitThread(transmitData,COM_Port,Socket);


    receiveThread.join();
    transmitThread.join();
    return 0;

}

