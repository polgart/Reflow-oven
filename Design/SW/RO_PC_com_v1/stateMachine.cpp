//
// Created by EDU_OBOS_5057 on 2021. 04. 03..
//

#include "stateMachine.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include "dataParser.h"

void stateMachine::addData(unsigned char c) {
    switch (this->state) {
        case IDLE:
            if (c == 0xFF)
                this->state = PREAMBLE1;
            break;
        case PREAMBLE1:
            if (c == 0xFF) {
                this->type = TEMPERATURE;
                this->state = PREAMBLE2;
            }
            break;
        case PREAMBLE2:
            this->chamberTemp = (double)c / 4;
            this->state = TEMP_LO;
            break;
        case TEMP_LO:
            this->chamberTemp += (double)c * 64;
            this->state = TEMP_HI;
            break;
        case TEMP_HI:
            this->boardTemp = (double)c / 16;
            this->state = INT_TEMP_LO;
            break;
        case INT_TEMP_LO:
            this->boardTemp += (double)c * 16;
            this->state = IDLE;
            this->passData2Parser();
            break;


    }
}



stateMachine::stateMachine() {
    this->state = IDLE;
    this->boardTemp = 0;
    this->chamberTemp = 0;
}

void stateMachine::setDataParser(dataParser* input) {
    this->stateMachineDataParser = input;
}

void stateMachine::passData2Parser() {
    switch (this->type) {
        case TEMPERATURE:
            std::string board = std::to_string(this->boardTemp);
            std::string chamber = std::to_string(this->chamberTemp);
            this->stateMachineDataParser->sendDataViaSocket(std::string("c"));
            this->stateMachineDataParser->sendDataViaSocket(chamber);
            this->stateMachineDataParser->sendDataViaSocket(std::string("b"));
            this->stateMachineDataParser->sendDataViaSocket(board);
            break;
    }

}
