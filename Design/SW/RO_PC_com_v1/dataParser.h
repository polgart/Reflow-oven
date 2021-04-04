//
// Created by EDU_OBOS_5057 on 2021. 04. 03..
//

#ifndef RO_PC_COM_V1_DATAPARSER_H
#define RO_PC_COM_V1_DATAPARSER_H

#include <iostream>
#include <string>
#include "stateMachine.h"
#include "socketHandler.h"


class dataParser {
public:
    dataParser(socketHandler* Socket);
    void addData(unsigned char c);
    void sendDataViaSocket(std::string data);
private:
    stateMachine* dataParserStateMachine;
    socketHandler* dataParserSocketHandler;
    socketHandler* dataParserSocket;
};


#endif //RO_PC_COM_V1_DATAPARSER_H
