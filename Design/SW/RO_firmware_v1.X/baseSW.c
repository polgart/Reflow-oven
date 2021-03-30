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
#include "mcc_generated_files/pin_manager.h"


/* 
 *
 *                  EEPROM DEFINITIONS SECTION 
 *
 */

const uint16_t EEPROM_ADDRESS = 0x00;
const uint16_t INTERNAL_MAX_TEMPERATURE = 0x0000;
const uint16_t THERMOCOUPLE_MAX_TEMPERATURE = 0x0000;

/* 
 *
 *                  HEAT PROFILE SECTION 
 *
 */


/*! \brief Heat profile buffer size
 *
 *  Each byte in the buffer represents a reference temeprature
 *  at a given time. The maximum reference temperature is
 *  25+255=280 celsis degree, and the minimum reference temperature is
 *  25 celsius degree.
 */
#define HEAT_PROFILE_SIZE 1024

/*! \brief Idle state
 *
 *  The task connected to this state cannot be deleted from the task que.
 *  This task provides the toggling protection to the SSR, ensuring that
 *  the software is properly running. This task is also responsible for
 *  disableing the heating process if one of the limits is exceeded.
 */
stateTaskList* IdleState = NULL;

/*! \brief Read data from MAX31855KASA+T state
 *
 *  The task connected to this state reads 4 bytes of data from MAX31855KASA+T.
 *  Hot junction temperature is stored in 14 bit format while the cold junction
 *  temperature is only 11 bits. Besides the measured temperatures, diagnostic
 *  data can also be read from the IC.
 *  [IC datasheet](https://www.micro-semiconductor.com/datasheet/12-MAX31855TASA-T.pdf)
 */
stateTaskList* ReadTemperatureData = NULL;

/*! \brief Receive data from Nextion HMI state
 *
 *  In this task varios control commands are received from the Nextion touch
 *  screen HMI. Heating process can be enabled or disabled, new heat profile can
 *  be choosen, and it can be loaded from the EEPROM to the microcontroller.
 * [NX4832T035](https://nextion.tech/datasheets/nx4832t035/)
 */
stateTaskList* ReceiveNextionData = NULL;

/*! \brief Receive data from FTDI state
 *
 *  In this task varios control commands are received from the PC via the
 *  FTDI UART USB bridge. Heating process can be enabled or disabled,
 *  new heat profile can be choosen, and it can be loaded from the EEPROM to the
 *  microcontroller. New heat profiles (generated on the PC) can be downloaded
 *  into the microcontroller and it can be saved into the EEPROM for further use.
 *  [FT232R](http://www.farnell.com/datasheets/2007793.pdf)
 */
stateTaskList* ReceiveFTDI = NULL;

/*! \brief Broadcast temperature data to the Nextion HMI
 *
 *  Broadcast temperature data to the Nextion HMI. 
 * [NX4832T035](https://nextion.tech/datasheets/nx4832t035/)
 */
stateTaskList* TranscieveNextionDATA = NULL;

/*! \brief Broadcast temperature data to the PC
 *
 *  Broadcast temperature data to the PC [FT232R](http://www.farnell.com/datasheets/2007793.pdf)
 */
stateTaskList* TransciveFTDI = NULL;

/*! \brief Read heat profile from EEPROM
 *
 *  Heat profile is read from the EEPROM via 400kHz I2C communication interface.
 *  This task use sequential read implemented in EEPROM_driver.c for the maximum
 *  transmission speed. [24LC64](http://ww1.microchip.com/downloads/en/devicedoc/21189f.pdf)
 */
stateTaskList* ReadEEPROM = NULL;

/*! \brief Write heat profile into EEPROM
 *
 *  Heat profile is written into the EEPROM via 400kHz I2C communication interface.
 *  This task use page write implemented in EEPROM_driver.c for the maximum
 *  transmission speed. [24LC64](http://ww1.microchip.com/downloads/en/devicedoc/21189f.pdf)
 */
stateTaskList* WriteEEPROM = NULL;

uint8_t heatProfileCurrent[HEAT_PROFILE_SIZE];/*!< Currently selected heat profile. */
uint8_t heatProfileBuffer[HEAT_PROFILE_SIZE];/*!< Heat profile buffer for memory operations. It can't be used directily, it has to be loaded into heatProfileCurrent*/
uint8_t heatProfileDefult[HEAT_PROFILE_SIZE];/*!< Default heat profile, cannot be deleted */


/*! \brief Reference temperature buffer
 *
 *  This temperature buffer structure conatins a 8 bit unsigned integer pointer,
 * and an offset value. The offset used during memory operations or denotes the
 * actual temperature reference (based on time) in the array depending on the
 * context.
 */
typedef struct
{   
    uint8_t     *data;      /*!< Buffer array pointer */
    uint16_t    offset;     /*!<  Buffer offset value */
} TEMPERATURE_BUFFER;


/*! \brief Reference temperature buffer status
 *
 * Actual status of the refernce temperature buffer is stored in this enum. The
 * status.
 */
typedef enum
{
    TB_RECEIVE_FROM_PC, /*!< Temperature buffer is receiving from PC is in progress */
    TB_SEND_TO_EEPROM, /*!< Temperature buffer is tranciving to EEPROM is in progress */
    TB_RECEIVE_FROM_EEPROM, /*!< Temperature buffer is receiving from EEPROM is in progress */
    IDLE /*!< Temperature buffer is in idle state and ready to be used */
} TEMPERATURE_BUFFER_STATUS;

/*! \brief Reference temperature profile
 *
 * Reference temperature profile three TEMPERATURE_BUFFER, a TEMPERATURE_BUFFER_STATUS
 * and an addressBuffer. The addressBuffer is used to store the EEPROM address.
 * Only bufferProfile can be used for memory operations, and currentProfile only can be used
 * for heating processes.
 */
typedef struct
{
    /* Read/Write Queue */
    TEMPERATURE_BUFFER          currentProfile;     /*!< Current profile array pointer */
    TEMPERATURE_BUFFER          bufferProfile;      /*!< Buffer profile array pointer */
    TEMPERATURE_BUFFER          defaultProfile;     /*!< Default profile buffer array pointer */
    TEMPERATURE_BUFFER_STATUS   profileStatus;      /*!< Temperature profile status */
    uint16_t                    addressBuffer;      /*!< This buffers stores the EEPROM address */
} TEMPERATURE_PROFILE;

static TEMPERATURE_PROFILE temperatureHeatProfile; /*!< Global varible, it stores heat profile related data */
TEMPERATURE_BUFFER temperatureBufferArray[3];


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
            int8_t oc:1;
            uint8_t scg:1;
            uint8_t scv:1;
            uint8_t reserved2:1;
            uint16_t internal_temperature_data:12;
            uint8_t fault:1;
            uint8_t reserved1:1;
            uint16_t thermocouple_temperature_data:14;
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
 *                  NEXTION SECTION 
 *
 */

typedef enum
{
    NEXTION_NORMAL_OPERATION,
    NEXTION_WRITE_EEPROM_COMMAND_HIGH_BYTE,
    NEXTION_WRITE_EEPROM_COMMAND_LOW_BYTE,
    NEXTION_RECEIVE_PROFILE_FROM_EEPROM
} NEXTION_STATUS;

static NEXTION_STATUS nextionStatus = NEXTION_NORMAL_OPERATION;


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


void enableHeat() {
    HEAT_IN_PROGRESS = true;
}

void disableHeat() {
    HEAT_IN_PROGRESS = false;
    SSR_OUTPUT_SetLow();
}

void IdleState_callback() {
    //Temperature protection - critical feature
    uint16_t internalTemp = SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.internal_temperature_data;
    uint16_t sensorTemp = SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.thermocouple_temperature_data;
    uint16_t desiredTemp = (uint16_t)temperatureHeatProfile.currentProfile.data[temperatureHeatProfile.currentProfile.offset];
    
    if (HEAT_IN_PROGRESS) {
        if (internalTemp > INTERNAL_MAX_TEMPERATURE) {
            disableHeat();
            return;
        }
    
        if (sensorTemp > THERMOCOUPLE_MAX_TEMPERATURE || sensorTemp > desiredTemp) {
            disableHeat();
            return;
        }
        
        SSR_OUTPUT_SetHigh();
    }
            
            
    //Toggling signal protection - critical feature
    T_PROTECTION_Toggle();
}


void loadBuffer() {
    memcpy(temperatureHeatProfile.currentProfile.data, temperatureHeatProfile.bufferProfile.data, HEAT_PROFILE_SIZE);
}


bool checkStartConditions() {
    
    if (ftdiStatus!=NORMAL_OPERATION)
        return false; // Transmission in progress
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.oc)
        return false; // Thermocouple is open
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.scg)
        return false; // Thermocouple is short-circuited to GND
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.scv)
        return false; // Thermocouple is short-circuited to VCC
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.internal_temperature_data < INTERNAL_MAX_TEMPERATURE)
        return false; // Inernal maximum temperature exceeded
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.thermocouple_temperature_data < THERMOCOUPLE_MAX_TEMPERATURE)
        return false; // Thermocouple maximum temperature exceeded
    
    return true;
    
}


void ReadTemperatureData_callback() {
    uint32_t sensorData = SPI1_Exchange32bit((uint32_t)0xF0F0F0F0);
    SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].rawData = sensorData;
    (SENSOR_DATA_HANDLER.dataArrayStatus.currentData)++;
    if (SENSOR_DATA_HANDLER.dataArrayStatus.currentData > SENSOR_DATA_STORE_LENGTH) {
        SENSOR_DATA_HANDLER.dataArrayStatus.currentData = 0;
        SENSOR_DATA_HANDLER.dataArrayStatus.isUploaded = 0x1;
    }
}

void ReceiveNextionData_callback() {
    
    uint8_t msg = UART1_Read();
    switch(nextionStatus) {
        case NORMAL_OPERATION:
            switch(msg) {
                case 0:
                    msg = UART1_Read();
                    if (msg & 0x01) {
                        // Start heating - critical feature
                        if (checkStartConditions()) {
                            enableHeat();
                        }
                    }
                    if (msg & 0x02) {
                        disableHeat();
                    }
                    if (msg & 0x04) {
                        loadBuffer();
                    }
                    break;
                case 1:
                    nextionStatus = NEXTION_WRITE_EEPROM_COMMAND_HIGH_BYTE;
                case 2:
                    nextionStatus = NEXTION_RECEIVE_PROFILE_FROM_EEPROM;
            }
            break;
        case NEXTION_WRITE_EEPROM_COMMAND_HIGH_BYTE:
            temperatureHeatProfile.addressBuffer = msg << 8;
            nextionStatus = NEXTION_WRITE_EEPROM_COMMAND_LOW_BYTE;
            break;
        case NEXTION_WRITE_EEPROM_COMMAND_LOW_BYTE:
            temperatureHeatProfile.addressBuffer = msg;
            nextionStatus = NEXTION_NORMAL_OPERATION;
            break;
        case NEXTION_RECEIVE_PROFILE_FROM_EEPROM:
            addTask(IdleState,ReadEEPROM);
            nextionStatus = NEXTION_NORMAL_OPERATION;
    }
    
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
                            enableHeat();
                        }
                    }
                    if (msg & 0x02) {
                        disableHeat();
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

void genericTranciverFunction() {
    
        uint8_t data_index = SENSOR_DATA_HANDLER.dataArrayStatus.currentData;
        if (data_index==0) {
            data_index=SENSOR_DATA_STORE_LENGTH;
        } else {
            data_index--;
        }
        
        uint8_t temp_lo = SENSOR_DATA_HANDLER.dataArrayQue[data_index].s.thermocouple_temperature_data;
        uint8_t temp_hi = SENSOR_DATA_HANDLER.dataArrayQue[data_index].s.thermocouple_temperature_data >> 8;   
        uint8_t int_temp_lo = SENSOR_DATA_HANDLER.dataArrayQue[data_index].s.internal_temperature_data;
        uint8_t int_temp_hi = SENSOR_DATA_HANDLER.dataArrayQue[data_index].s.internal_temperature_data >> 8;
        
        int a=43;
        if (temp_lo==0) {
            a=data_index;
            a+=34;
            a-=34;
        }
        
        switch(transciveObj.status) {
        case TRANSCIEVE_IDLE:
            break;
        case TRANSCIEVE_FULL_HEAT_PROFILE:
            UART1_WriteBuffer(temperatureHeatProfile.currentProfile.data , HEAT_PROFILE_SIZE );
            UART2_WriteBuffer(temperatureHeatProfile.currentProfile.data , HEAT_PROFILE_SIZE );
            break;
        case TRANSCIEVE_CURRENT_DATA:
            UART1_Write(temp_lo);
            UART1_Write(temp_hi);
            UART1_Write(int_temp_lo);
            UART1_Write(int_temp_hi);

//            UART2_Write(temp_lo);
//            UART2_Write(temp_hi);
//            UART2_Write(int_temp_lo);
//            UART2_Write(int_temp_hi);
//            UART2_Write(0x00);

            break;
    }
}

    
void TranscieveNextionDATA_callback() {
    genericTranciverFunction();
}

void TransciveFTDI_callback() {
    genericTranciverFunction();
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



/* Interrupt service routines */

void baseSW_TMR2_ISR(void) {
    static char flag = 0;
    if (flag==0) {
        addTask(IdleState,TranscieveNextionDATA);
        flag=1;
    } else {
        addTask(IdleState,ReadTemperatureData);
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
    
    // Swicth on timer 2
    TMR2_Start();
    
    // Enable UARTs
    UART1_Enable();
    UART2_Enable();
    
    // Enable SPI
    
    
    
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
    transciveObj.status = TRANSCIEVE_CURRENT_DATA;
    
    
    
    return IdleState;

}

