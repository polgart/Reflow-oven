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
            else if (c == 0xFE) {
                this->type = TEMPERATURE_WITH_HET_ENABLED;
                this->state = PREAMBLE2;
            }
            else if (c == 0xFD) {
                this->type = HEAT_PROFILE;
                this->string_offset = 0;
                this->state = HEAT_PROFILE_NAME;
            }
            else if (c == 0xFC) {
                this->type = TEMPERATURE_WITH_HEAT_ENABLED_TEST_MODE;
                this->state = PREAMBLE2;
            }
            else {
                this->state = IDLE;
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
            else if (this->type == TEMPERATURE_WITH_HET_ENABLED || this->type == TEMPERATURE_WITH_HEAT_ENABLED_TEST_MODE) {
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
            if (this->type == TEMPERATURE_WITH_HET_ENABLED) {
                this->passData2Parser();
                this->state = IDLE;
            }
            else {
                this->state = DESIRED_LO;
            }
            break;
        case DESIRED_LO:
            this->desired_temp = (double)c / 4;
            this->state = DESIRED_HI;
            break;
        case DESIRED_HI:
            this->desired_temp += (double)c * 64;
            this->state = IDLE;
            this->passData2Parser();
            break;
        case HEAT_PROFILE_NAME:
            this->name[this->string_offset]=c;
            if (++this->string_offset == 8)
                this->state = HEAT_PROFILE_ID;
            break;
        case HEAT_PROFILE_ID:
            this->heat_profile_id = c;
            this->state = HEAT_PROFILE_SIZE_LO;
            break;
        case HEAT_PROFILE_SIZE_LO:
            this->heat_profile_size = c;
            this->state = HEAT_PROFILE_SIZE_HI;
            break;
        case HEAT_PROFILE_SIZE_HI:
            this->heat_profile_size += c*256;
            this->string_offset = 0;
            this->state = HEAT_PROFILE_TEMPERATURE;
            break;
        case HEAT_PROFILE_TEMPERATURE:
            if (this->string_offset % 2 == 0) {
                this->heat_profile_temperature[this->string_offset/2] = c;
            }
            else {
                this->heat_profile_temperature[this->string_offset/2] += c*256;
            }
            if (++this->string_offset==2*(this->heat_profile_size+1)) {
                this->string_offset = 0;
                this->state = HEAT_PROFILE_TIME;
            }
            break;
        case HEAT_PROFILE_TIME:
            if (this->string_offset % 2 == 0) {
                this->heat_profile_time[this->string_offset/2] = c;
            }
            else {
                this->heat_profile_time[this->string_offset/2] += c*256;
            }
            if (++this->string_offset==2*(this->heat_profile_size+1)) {
                this->passData2Parser();
                this->state = IDLE;
            }
            break;
        default:
            printf("Communication error.");

    }
}



stateMachine::stateMachine() {
    this->state = IDLE;
    this->boardTemp = 0;
    this->chamberTemp = 0;
    this->msg_ctr = 0;
}

void stateMachine::setDataParser(dataParser* input) {
    this->stateMachineDataParser = input;
}

void stateMachine::passData2Parser() {
    std::string board,chamber,time,id,data,name,length,desired_temp;
    int it;
    switch (this->type) {
        case TEMPERATURE:
            board = std::to_string(this->boardTemp);
            chamber = std::to_string(this->chamberTemp);
            this->stateMachineDataParser->sendDataViaSocket(std::string("c"));
            this->stateMachineDataParser->sendDataViaSocket(chamber);
            this->stateMachineDataParser->sendDataViaSocket(std::string("b"));
            this->stateMachineDataParser->sendDataViaSocket(board);
            this->stateMachineDataParser->sendDataViaSocket(std::string("EOF")); //End of message
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
            this->stateMachineDataParser->sendDataViaSocket(std::string("EOF")); //End of message
            break;
        case HEAT_PROFILE:

            length = std::to_string(this->heat_profile_size);
            id = std::to_string(this->heat_profile_id);
            name = std::string(this->name);

            this->stateMachineDataParser->sendDataViaSocket(std::string("e"));
            this->stateMachineDataParser->sendDataViaSocket(name);
            this->stateMachineDataParser->sendDataViaSocket(std::string("f"));
            this->stateMachineDataParser->sendDataViaSocket(id);
            this->stateMachineDataParser->sendDataViaSocket(std::string("g"));
            this->stateMachineDataParser->sendDataViaSocket(length);

            this->stateMachineDataParser->sendDataViaSocket(std::string("h"));
            for (it = 0; it <= this->heat_profile_size; it++) {
                data = std::to_string(this->heat_profile_temperature[it]);
                this->stateMachineDataParser->sendDataViaSocket(data);
            }

            this->stateMachineDataParser->sendDataViaSocket(std::string("i"));
            for (it = 0; it <= this->heat_profile_size; it++) {
                time = std::to_string(this->heat_profile_time[it]);
                this->stateMachineDataParser->sendDataViaSocket(time);
            }
            this->stateMachineDataParser->sendDataViaSocket(std::string("EOF")); //End of message
            break;
        case TEMPERATURE_WITH_HEAT_ENABLED_TEST_MODE:
            board = std::to_string(this->boardTemp);
            chamber = std::to_string(this->chamberTemp);
            time = std::to_string(this->time);
            desired_temp = std::to_string(this->desired_temp);
            this->stateMachineDataParser->sendDataViaSocket(std::string("c"));
            this->stateMachineDataParser->sendDataViaSocket(chamber);
            this->stateMachineDataParser->sendDataViaSocket(std::string("b"));
            this->stateMachineDataParser->sendDataViaSocket(board);
            this->stateMachineDataParser->sendDataViaSocket(std::string("d"));
            this->stateMachineDataParser->sendDataViaSocket(time);
            this->stateMachineDataParser->sendDataViaSocket(std::string("j"));
            this->stateMachineDataParser->sendDataViaSocket(desired_temp);
            this->stateMachineDataParser->sendDataViaSocket(std::string("EOF")); //End of message
            break;
    }

}
