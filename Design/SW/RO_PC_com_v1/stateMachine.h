//
// Created by EDU_OBOS_5057 on 2021. 04. 03..
//



#ifndef RO_PC_COM_V1_STATEMACHINE_H
#define RO_PC_COM_V1_STATEMACHINE_H

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
    TIME_HI
};

enum DataType {
    TEMPERATURE,
    TEMPERATURE_WITH_HET_ENABLED,
    HEAT_PROFILE
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
    int time; // In ms
    dataParser* stateMachineDataParser;


};


#endif //RO_PC_COM_V1_STATEMACHINE_H
