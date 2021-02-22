#include <stdlib.h>
#include <string.h>
#include "baseSW.h"
#include "stateTaskHandler.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/i2c1.h"
#include "EEPROM_driver.h"


/* 
 *
 *                  EEPROM DEFINITIONS SECTION 
 *
 */

static uint16_t EEPROM_ADDRESS = 0x00;

/* 
 *
 *                  HEAT PROFILE SECTION 
 *
 */

#define HEAT_PROFILE_SIZE 1024

stateTaskList* IdleState = NULL;
stateTaskList* ReadTemperatureData = NULL;
stateTaskList* ReceiveNextionData = NULL;
stateTaskList* ReceiveFTDI = NULL;
stateTaskList* TranscieveNextionDATA = NULL;
stateTaskList* TransciveFTDI = NULL;
stateTaskList* ReadEEPROM = NULL;
stateTaskList* WriteEEPROM = NULL;

uint8_t heatProfileCurrent[HEAT_PROFILE_SIZE];
uint8_t heatProfileBuffer[HEAT_PROFILE_SIZE];
uint8_t heatProfileDefult[HEAT_PROFILE_SIZE];


typedef struct
{
    /* Read/Write Queue */
    uint8_t     *data;      // Buffer array pointer
    uint16_t    offset;     // Buffer offset value
} TEMPERATURE_BUFFER;

typedef enum
{
    TB_RECEIVE_FROM_PC,
    TB_SEND_TO_EEPROM,
    TB_RECEIVE_FROM_EEPROM,
    IDLE
} TEMPERATURE_BUFFER_STATUS;

typedef struct
{
    /* Read/Write Queue */
    TEMPERATURE_BUFFER          currentProfile;     // Current profile array pointer
    TEMPERATURE_BUFFER          bufferProfile;      // Buffer profile array pointer
    TEMPERATURE_BUFFER          defaultProfile;     // Default profile buffer array pointer
    TEMPERATURE_BUFFER_STATUS   profileStatus;      // Temperature profile status
    uint16_t                    addressBuffer;      // This buffers stores the EEPROM address
} TEMPERATURE_PROFILE;

static TEMPERATURE_PROFILE temperatureHeatProfile;
static TEMPERATURE_BUFFER temperatureBufferArray[3];


/* 
 *
 *                  MAX31855 INTERFACE SECTION 
 *
 */


#define SENSOR_DATA_STORE_LENGTH 10 // Maximum 127

typedef union
{
    struct
    {
            uint16_t thermocouple_temperature_data:14;
            uint8_t reserved1:1;
            uint8_t fault:1;
            uint16_t internal_temperature_data:12;
            uint8_t reserved2:1;
            uint8_t scv:1;
            uint8_t scg:1;
            uint8_t oc:1;
    } s;
    uint32_t rawData;
} IC_MAX31855_DATA;

typedef struct {
    IC_MAX31855_DATA* dataArrayQue;
    struct {
        uint8_t isUploaded:1;
        uint8_t currentData:7;
    } dataArrayStatus;
} SENSOR_DATA;

static SENSOR_DATA SENSOR_DATA_HANDLER;
static IC_MAX31855_DATA SENSOR_DATA_ARRAYS[SENSOR_DATA_STORE_LENGTH];


/* 
 *
 *                  FTDI SECTION 
 *
 */

typedef enum
{
    NORMAL_OPERATION,
    RECEIVE_HEAT_PROFILE,
    SEND_HEAT_PROFILE_TO_EEPROM,
    RECEIVE_PROFILE_FROM_EEPROM,
    WRITE_EEPROM_COMMAND_HIGH_BYTE,
    WRITE_EEPROM_COMMAND_LOW_BYTE
} FTDI_STATUS;

static FTDI_STATUS ftdiStatus = NORMAL_OPERATION;


/* 
 *
 *                  BASE SOFTWARE GENERIC DATA SECTION 
 *
 */

typedef union {
    struct {
    uint8_t heatProfileID:5;
    uint8_t startHeating:1;
    uint8_t stopHeating:1;
    uint8_t setHeatProfile:1;
    } properties;
    uint8_t raw;
} BS_DATA_OBJ;
static uint8_t HEAT_IN_PROGRESS = false;


/* 
 *
 *                  TRANSCIEVE STATES
 *
 */

typedef enum {
    TRANSCIEVE_FULL_HEAT_PROFILE,
    TRANSCIEVE_CURRENT_DATA,
    TRANSCIEVE_IDLE
} TRANSCIEVE_STATUS;

typedef struct {
    TRANSCIEVE_STATUS status;
    bool NextionTransmissionInProgress;
    bool FTDITransmissionInProgress;
} TRANSCIEVE_OBJ;

static TRANSCIEVE_OBJ transciveObj;

/* 
 *
 *                  IMPLEMENTATION SECTION 
 *
 */
void IdleState_callback() {
    //Temperature protection - critical feature
    
    //Toggling signal protection - critical feature
    
}

void ReadTemperatureData_callback() {
    uint32_t sensorData = SPI1_Exchange32bit((uint32_t)0x0);
    SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].rawData = sensorData;
    (SENSOR_DATA_HANDLER.dataArrayStatus.currentData)++;
    if (SENSOR_DATA_HANDLER.dataArrayStatus.currentData == SENSOR_DATA_STORE_LENGTH) {
        SENSOR_DATA_HANDLER.dataArrayStatus.currentData = 0;
        SENSOR_DATA_HANDLER.dataArrayStatus.isUploaded = 0x1;
    }
}

void ReceiveNextionData_callback() {
    
    uint8_t msg = UART1_Read();
    if ((msg | 0xFB) == 0xFF) {
        // Start heating - critical feature
    }
    if ((msg | 0xFD) == 0xFF) {
        //Stop heating - critical feature
    }
    if ((msg | 0xFE) == 0xFF) {
        //Set selected heat profile - non critical feature
    }
    
}

bool checkStartConditions() {
    
    uint16_t internalMaxTemp = 0x0000;
    uint16_t thermocoupleMaxTemp = 0x0000;
    
    if (ftdiStatus!=NORMAL_OPERATION)
        return false; // Transmission in progress
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.oc)
        return false; // Thermocouple is open
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.scg)
        return false; // Thermocouple is short-circuited to GND
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.scv)
        return false; // Thermocouple is short-circuited to VCC
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.internal_temperature_data < internalMaxTemp)
        return false; // Inernal maximum temperature exceeded
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.thermocouple_temperature_data < thermocoupleMaxTemp)
        return false; // Thermocouple maximum temperature exceeded
    
    return true;
    
}

void ReceiveFTDI_callback() {
    uint8_t msg = UART2_Read();
    switch(ftdiStatus) {
        case NORMAL_OPERATION:
            switch(msg) {
                case 0:
                    msg = UART2_Read();
                    if (msg & 0x01) {
                        // Start heating - critical feature
                        if (checkStartConditions()) {
                            HEAT_IN_PROGRESS = true;
                        }
                    }
                    if (msg & 0x02) {
                        HEAT_IN_PROGRESS = false;
                    }
                    if (msg & 0x04) {
                        loadBuffer();
                    }
                    break;
                case 1:
                    temperatureHeatProfile.profileStatus = TB_RECEIVE_FROM_PC;
                    ftdiStatus = RECEIVE_HEAT_PROFILE;
                    break;
                case 2:
                    temperatureHeatProfile.profileStatus = TB_SEND_TO_EEPROM;
                    ftdiStatus = SEND_HEAT_PROFILE_TO_EEPROM;
                case 3:
                    temperatureHeatProfile.profileStatus = TB_RECEIVE_FROM_EEPROM;
                    ftdiStatus = RECEIVE_PROFILE_FROM_EEPROM;
                case 4:
                    ftdiStatus = WRITE_EEPROM_COMMAND_HIGH_BYTE;
            }
            break;
        case RECEIVE_HEAT_PROFILE:
            if (temperatureHeatProfile.bufferProfile.offset<HEAT_PROFILE_SIZE) {
                temperatureHeatProfile.bufferProfile.data[temperatureHeatProfile.bufferProfile.offset]=msg;
                temperatureHeatProfile.bufferProfile.offset++;
            } else {
                temperatureHeatProfile.bufferProfile.offset = 0;
                ftdiStatus = NORMAL_OPERATION;
                temperatureHeatProfile.profileStatus = IDLE;
            }
            break;
        case WRITE_EEPROM_COMMAND_HIGH_BYTE:
            temperatureHeatProfile.addressBuffer = msg << 8;
            ftdiStatus = WRITE_EEPROM_COMMAND_LOW_BYTE;
            break;
        case WRITE_EEPROM_COMMAND_LOW_BYTE:
            temperatureHeatProfile.addressBuffer = msg;
            ftdiStatus = NORMAL_OPERATION;
            break;
        case SEND_HEAT_PROFILE_TO_EEPROM:
            addTask(IdleState,WriteEEPROM);
            ftdiStatus = NORMAL_OPERATION;
            break;
        case RECEIVE_PROFILE_FROM_EEPROM:
            addTask(IdleState,ReadEEPROM);
            ftdiStatus = NORMAL_OPERATION;
    }
}
    genericTranciverFunction();
void TranscieveNextionDATA_callback() {
    genericTranciverFunction();
}

void TransciveFTDI_callback() {
    
}

void genericTranciverFunction() {
        switch(transciveObj.status) {
        case TRANSCIEVE_IDLE:
            break;
        case TRANSCIEVE_FULL_HEAT_PROFILE:
            break;
        case TRANSCIEVE_CURRENT_DATA:
            break;
    }
}

void ReadEEPROM_callback() {
    EEPROM_Read(EEPROM_ADDRESS,temperatureHeatProfile.addressBuffer,temperatureHeatProfile.bufferProfile.data,HEAT_PROFILE_SIZE);
    temperatureHeatProfile.profileStatus = IDLE;
}

/**
  WriteEEPROM_callback

  @Summary
 Copy data from temperature buffer to EEPROM

  @Description
    This defines the object in the i2c queue. Each entry is a composed
    of a list of TRBs, the number of the TRBs and the status of the
    currently processed TRB.
 */

void WriteEEPROM_callback() {
    /* If the buffer input is not equal to zero there is a transmission in proggress
     and the data in buffer is not consistent. */
    if (temperatureHeatProfile.bufferProfile.offset == 0) {
        uint8_t page_size = 32;
        uint8_t write_cycles_max = HEAT_PROFILE_SIZE / page_size;
        uint8_t current_write_cycle;
        uint16_t offset;

        for (current_write_cycle = 0; current_write_cycle < write_cycles_max; current_write_cycle++) {
            offset = temperatureHeatProfile.bufferProfile.offset;
            EEPROM_WritePage(EEPROM_ADDRESS,temperatureHeatProfile.addressBuffer,&(temperatureHeatProfile.bufferProfile.data[offset*page_size]));
        }

    }
    temperatureHeatProfile.profileStatus = IDLE;
}

void loadBuffer() {
    memcpy(temperatureHeatProfile.currentProfile.data, temperatureHeatProfile.bufferProfile.data, HEAT_PROFILE_SIZE);
}




/* Interrupt service routines */

void baseSW_TMR2_ISR(void) {
    static char flag = 0;
    if (flag==0) {
        addTask(IdleState,TranscieveNextionDATA);
        flag=1;
    } else {
        addTask(IdleState,TransciveFTDI);
        flag=0;
    }
}

void baseSW_UART1_tx_ISR(void) {
    addTask(IdleState,TranscieveNextionDATA);
}

void baseSW_UART2_tx_ISR(void) {
    addTask(IdleState,TransciveFTDI);
}

void baseSW_UART1_rx_ISR(void) {
    addTask(IdleState,ReceiveNextionData);
}

void baseSW_UART2_rx_ISR(void) {
    addTask(IdleState,ReceiveFTDI);
}

stateTaskList* baseSW_Initialize(void) {
    
    // Initialize states
    IdleState = createTask(IdleState_callback);
    ReadTemperatureData = createTask(ReadTemperatureData_callback);
    ReceiveNextionData = createTask(ReceiveNextionData_callback);
    ReceiveFTDI = createTask(ReceiveFTDI_callback);
    TranscieveNextionDATA = createTask(TranscieveNextionDATA_callback);
    TransciveFTDI = createTask(TransciveFTDI_callback);
    ReadEEPROM = createTask(ReadEEPROM_callback);
    WriteEEPROM = createTask(WriteEEPROM_callback);
    
    // Initialize task handler
    initilaizeTaskHandler(IdleState);
    
    // Initilize ISRs
    TMR2_SetInterruptHandler(baseSW_TMR2_ISR);
    UART1_SetRxInterruptHandler(baseSW_UART1_rx_ISR); // UART1 -> NEXTION
    UART2_SetRxInterruptHandler(baseSW_UART2_rx_ISR); // UART2 -> FTDI
    
    
    // Initialize heat profile handler
    
    temperatureHeatProfile.currentProfile = temperatureBufferArray[0];
    temperatureHeatProfile.bufferProfile = temperatureBufferArray[1];
    temperatureHeatProfile.defaultProfile = temperatureBufferArray[2];
    
    temperatureHeatProfile.currentProfile.data = heatProfileCurrent;
    temperatureHeatProfile.currentProfile.offset = 0x0;
    
    temperatureHeatProfile.bufferProfile.data = heatProfileBuffer;
    temperatureHeatProfile.bufferProfile.offset = 0x0;
    
    temperatureHeatProfile.defaultProfile.data = heatProfileDefult;
    temperatureHeatProfile.defaultProfile.offset = 0x0;
    
    temperatureHeatProfile.profileStatus = IDLE;
    
    //Initialize temperature sensor interface
    
    SENSOR_DATA_HANDLER.dataArrayQue = SENSOR_DATA_ARRAYS;
    SENSOR_DATA_HANDLER.dataArrayStatus.isUploaded = 0x0;
    SENSOR_DATA_HANDLER.dataArrayStatus.currentData = 0x0;
    
    //Transcieve object
    transciveObj.FTDITransmissionInProgress=false;
    transciveObj.NextionTransmissionInProgress=false;
    transciveObj.status = TRANSCIEVE_IDLE;
    
    
    
    return IdleState;

}

