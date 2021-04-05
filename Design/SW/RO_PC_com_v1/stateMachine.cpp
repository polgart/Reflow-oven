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
            if (c == 0xFE) {
                this->type = TEMPERATURE_WITH_HET_ENABLED;
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
            if (this->type == TEMPERATURE) {
                this->state = IDLE;
                this->passData2Parser();
            }
            else if (this->type == TEMPERATURE_WITH_HET_ENABLED) {
                this->state = INT_TEMP_HI;
            }
            break;
        case INT_TEMP_HI:
            this->time = (int)c;
            this->state = TIME_LO;
            break;
        case TIME_LO:
            this->time += 256 * (int)(c);
            this->time *= 10;
            this->state = IDLE;
            this->passData2Parser();
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
    std::string board,chamber,time;
    switch (this->type) {
        case TEMPERATURE:
            board = std::to_string(this->boardTemp);
            chamber = std::to_string(this->chamberTemp);
            this->stateMachineDataParser->sendDataViaSocket(std::string("c"));
            this->stateMachineDataParser->sendDataViaSocket(chamber);
            this->stateMachineDataParser->sendDataViaSocket(std::string("b"));
            this->stateMachineDataParser->sendDataViaSocket(board);
            break;
        case TEMPERATURE_WITH_HET_ENABLED:
            board = std::to_string(this->boardTemp);
            chamber = std::to_string(this->chamberTemp);
            time = std::to_string(this->time);
            this->stateMachineDataParser->sendDataViaSocket(std::string("c"));
            this->stateMachineDataParser->sendDataViaSocket(chamber);
            this->stateMachineDataParser->sendDataViaSocket(std::string("b"));
            this->stateMachineDataParser->sendDataViaSocket(board);
            this->stateMachineDataParser->sendDataViaSocket(std::string("d"));
            this->stateMachineDataParser->sendDataViaSocket(time);
            break;
            break;
    }

}
