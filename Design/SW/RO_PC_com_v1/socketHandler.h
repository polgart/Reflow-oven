//
// Created by EDU_OBOS_5057 on 2021. 04. 03..
//


#ifndef RO_PC_COM_V1_SOCKETHANDLER_H
#define RO_PC_COM_V1_SOCKETHANDLER_H


#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "socketHandler.h"


// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

enum socketType {
    Receive,
    Send
};

class socketHandler {
public:

    socketHandler();
    int initSocket();
    int sendSocket(const char *sendBuffer);
    int receiveSocket(char* rcvBuffer, unsigned int length);
    int closeSocket();

private:
    int initSocketInternal(SOCKET &socket_param,WSAData &wsa_param,sockaddr_in &addr_param,int &i_result_param,unsigned int &port,enum socketType sType);
    SOCKET SendSocket;
    WSAData SendWsaData;
    sockaddr_in SendAddr;
    int Send_iResult;
    unsigned int send_port;

    SOCKET RcvSocket;
    WSAData RcvWsaData;
    sockaddr_in RcvAddr;
    int Rcv_iResult;
    unsigned int rcv_port;
};


#endif //RO_PC_COM_V1_SOCKETHANDLER_H
