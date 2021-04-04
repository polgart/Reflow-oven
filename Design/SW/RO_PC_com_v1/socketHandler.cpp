#include "socketHandler.h"

int socketHandler::initSocket() {

    // Init receive socket
    if (this->initSocketInternal(this->RcvSocket,this->RcvWsaData,this->RcvAddr,this->Rcv_iResult,this->rcv_port,Receive)) {
        perror("Receive socket initialization failed.");
        return 1;
    }

    // Init send socket
    if (this->initSocketInternal(this->SendSocket,this->SendWsaData,this->SendAddr,this->Send_iResult,this->send_port,Send)) {
        perror("Send socket initialization failed.");
        return 1;
    }

    printf("Sockets initialized successfully.\n");

    return 0;
}

int socketHandler::closeSocket() {

    //---------------------------------------------
    // When the application is finished sending, close the socket.
    printf("Closing socket.\n");
    Rcv_iResult = closesocket(RcvSocket);
    Send_iResult = closesocket(SendSocket);

    if (Send_iResult == SOCKET_ERROR || Rcv_iResult == SOCKET_ERROR) {
        printf("closesocket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //---------------------------------------------
    // Clean up and quit.
    printf("Exiting.\n");
    WSACleanup();
    return 0;
}

int socketHandler::sendSocket(const char *sendBuffer) {
    //---------------------------------------------
    // Send a datagram to the receiver

    //printf("Sending a datagram to the receiver %s (%d)\n",sendBuffer,strlen(sendBuffer));
    Send_iResult = sendto(SendSocket,
                          sendBuffer, strlen(sendBuffer), 0, (SOCKADDR *) & SendAddr, sizeof (SendAddr));
    if (Send_iResult == SOCKET_ERROR) {
        wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
        closesocket(SendSocket);
        WSACleanup();
        return 1;
    }

    return 0;
    }

socketHandler::socketHandler() {
    send_port = 27015;
    rcv_port = 27016;
}

int socketHandler::receiveSocket(char* rcvBuffer, unsigned int length) {


    //try to receive some data, this is a blocking call
    int rcvAddrSize = sizeof (RcvAddr);
    if (recvfrom(RcvSocket, rcvBuffer, length, 0, (SOCKADDR *) &RcvAddr, &rcvAddrSize) == SOCKET_ERROR)
    {
        printf("recvfrom() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //print details of the client/peer and the data received
    //printf("Received packet from %s:%d\n", inet_ntoa(RcvAddr.sin_addr), ntohs(RcvAddr.sin_port));
    //printf("Data: %s\n" , rcvBuffer);
    return 0;
}

int
socketHandler::initSocketInternal(SOCKET &socket_param,WSAData &wsa_param,sockaddr_in &addr_param,int &i_result_param,unsigned int &port,enum socketType sType) {

    socket_param = INVALID_SOCKET;

    //----------------------
    // Initialize Winsock
    i_result_param = WSAStartup(MAKEWORD(2, 2), &wsa_param);
    if (i_result_param != NO_ERROR) {
        printf("WSAStartup failed with error: %d\n", i_result_param);
        return 1;
    }

    //---------------------------------------------
    // Create a socket for sending data
    socket_param = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_param == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //---------------------------------------------
    // Set up the addr_param structure with the IP address of
    // the receiver (in this example case "localhost")
    // and the specified port number.
    addr_param.sin_family = AF_INET;
    addr_param.sin_port = htons(port);
    addr_param.sin_addr.s_addr = inet_addr("localhost");

    bool bc = true;
    int ret = setsockopt(socket_param, SOL_SOCKET, SO_BROADCAST, (const char*)&bc, sizeof(bool));
    if(ret < 0){
        perror("set opt err:");
    }

    printf("Socket created on port %u\n",port);

    if (sType == Receive) {
        // Send initial msg to bind socket
        const char* sendBuffer = "Communication module initialized successfully";

        Rcv_iResult = sendto(RcvSocket,
                             sendBuffer, strlen(sendBuffer), 0, (SOCKADDR *) & RcvAddr, sizeof (RcvAddr));
        if (Rcv_iResult == SOCKET_ERROR) {
            wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
            closesocket(RcvSocket);
            WSACleanup();
            return 1;
        }
    }
    
    return 0;
}

