//
// Created by EDU_OBOS_5057 on 2021. 04. 03..
//

#include "dataParser.h"
#include "stateMachine.h"
#include <string>


void dataParser::addData(unsigned char c) {
    this->dataParserStateMachine->addData(c);
}

dataParser::dataParser(socketHandler* Socket) {
    this->dataParserStateMachine = new stateMachine();
    this->dataParserStateMachine->setDataParser(this);
    this->dataParserSocket = Socket;
}

void dataParser::sendDataViaSocket(std::string str) {
    printf("%s\n",str.c_str());
    this->dataParserSocket->sendSocket(str.c_str());
}
