//
// Created by EDU_OBOS_5057 on 2021. 04. 03..
//



#ifndef RO_PC_COM_V1_STATEMACHINE_H
#define RO_PC_COM_V1_STATEMACHINE_H

#include <cstdint>

class dataParser;

enum State {
    IDLE,
    PREAMBLE1,
    PREAMBLE2,
    TEMP_LO,
    TEMP_HI,
    INT_TEMP_LO,
    INT_TEMP_HI,
    DESIRED_LO,
    DESIRED_HI,
    TIME_LO,
    TIME_HI,
    HEAT_PROFILE_NAME,
    HEAT_PROFILE_ID,
    HEAT_PROFILE_SIZE_LO,
    HEAT_PROFILE_SIZE_HI,
    HEAT_PROFILE_TEMPERATURE,
    HEAT_PROFILE_TIME
};

enum DataType {
    TEMPERATURE,
    TEMPERATURE_WITH_HET_ENABLED,
    HEAT_PROFILE,
    TEMPERATURE_WITH_HEAT_ENABLED_TEST_MODE
};

class stateMachine {
public:
    void setDataParser(dataParser *input);
    stateMachine();
    void addData(unsigned char c);
private:
    void passData2Parser();
    enum State state;
    enum DataType type;
    double chamberTemp; // In C
    double boardTemp; // In C
    double desired_temp; // In C
    int time; // In ms
    dataParser* stateMachineDataParser;
    char name[8];
    uint32_t msg_ctr;
    uint8_t string_offset;
    uint8_t heat_profile_id;
    uint16_t heat_profile_size;
    uint16_t heat_profile_temperature[512];
    uint16_t heat_profile_time[512];

};


#endif //RO_PC_COM_V1_STATEMACHINE_H
