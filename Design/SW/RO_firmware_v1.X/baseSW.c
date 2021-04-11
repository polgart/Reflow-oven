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

const uint16_t EEPROM_ADDRESS = 0x00;   /*!< EEPROM address */
const uint16_t INTERNAL_MAX_TEMPERATURE = 0x320; /*!< INTERNAL_MAX_TEMPERATURE = Max board temperature x 4 */
const uint16_t THERMOCOUPLE_MAX_TEMPERATURE = 0x04B0;   /*!< THERMOCOUPLE_MAX_TEMPERATURE = Max chamber temperature x 2 */

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
#define HEAT_PROFILE_SIZE 255

/*! \brief Idle state
 *
 *  The task connected to this state cannot be deleted from the task que.
 *  This task provides the toggling protection to the SSR, ensuring that
 *  the software is properly running. This task is also responsible for
 *  disabling the heating process if one of the limits is exceeded.
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

uint16_t heatProfileCurrent[HEAT_PROFILE_SIZE];/*!< Currently selected heat profile. */
uint16_t heatProfileBuffer[HEAT_PROFILE_SIZE];/*!< Heat profile buffer for memory operations. It can't be used directily, it has to be loaded into heatProfileCurrent*/
uint16_t heatProfileDefult[HEAT_PROFILE_SIZE];/*!< Default heat profile, cannot be deleted */


uint16_t heatProfileCurrentTime[HEAT_PROFILE_SIZE];/*!< Currently selected heat profile time. */
uint16_t heatProfileBufferTime[HEAT_PROFILE_SIZE];/*!< Heat profile time buffer for memory operations. It can't be used directily, it has to be loaded into heatProfileCurrent*/
uint16_t heatProfileDefultTime[HEAT_PROFILE_SIZE];/*!< Default heat profile time, cannot be deleted */

/*! \brief Reference temperature buffer
 *
 *  This temperature buffer structure conatins a 8 bit unsigned integer pointer,
 * and an offset value. The offset used during memory operations or denotes the
 * actual temperature reference (based on time) in the array depending on the
 * context.
 */
typedef struct
{   
    uint16_t    *data;      /*!< Buffer array pointer, temperature = data*4 */
    uint16_t    *time;      /*!< Data time * 100[ms] */
    uint16_t    offset;     /*!<  Buffer offset value */
    uint16_t    size;       /*!<  Number of records in the temperature buffer */
    uint8_t     valid;      /*!<  TEMPERATURE_BUFFER is valid is valid=0x1, invalid else */
    char        name[8];    /*!<  TEMPERATURE_BUFFER name, 8 byte */
    uint8_t     ID;         /*!<  TEMPERATURE_BUFFER ID, 1 byte */
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


#define SENSOR_DATA_STORE_LENGTH 10 /*!< Number of measurements we want to store in the ring buffer */


/*! \brief Temperature sensor data mapping
 *
 * This structure is used to store the read from the temperature sensor. Most
 * important fileds are thermocouple_temperature_data and fault. Maximum board
 * temperature is 127C, minimum is -55 with 0.0625C resolution. Maximum board
 * temperature is 1600C, minimum is -250 with 0.25C resolution. For more information
 * check out the MAX31855 datasheet.
 */
typedef union
{
    struct
    {
            int8_t oc:1;    /*!< Open circuit detected */
            uint8_t scg:1;  /*!< Short circuit to ground detected */
            uint8_t scv:1;  /*!< Short circuit to Vcc detected */
            uint8_t reserved2:1;    /*!< Reserved by the vendor */
            uint16_t internal_temperature_data:12;  /*!< Board temperature */
            uint8_t fault:1;    /*!< Fault detected */
            uint8_t reserved1:1;    /*!< Reserved by the vendor */
            uint16_t thermocouple_temperature_data:14;  /*!< Chamber temperature */
    } s;
    uint32_t rawData;
} IC_MAX31855_DATA;


/*! \brief Measuremnt data array structure
 *
 * This structure holds a maximum of SENSOR_DATA_STORE_LENGTH measurements read
 * from the temperature sensor. currentData data denotes the array index which
 * will be overwritten is new data arrives from the sensor. This structure
 * implements a ring buffer, so currentData only can be between 0 and 
 * SENSOR_DATA_STORE_LENGTH - 1.
 */
typedef struct {
    IC_MAX31855_DATA* dataArrayQue; /*!< Pointer to measurement data */
    struct {
        uint8_t isUploaded:1; /*!< 1 is all records are filled with measurement data in the ring buffer, otherwise 0.  */
        uint8_t currentData:7; /*!< Ring buffer pointer */
    } dataArrayStatus; /*!< Measurement data status desriptor */
} SENSOR_DATA;

static SENSOR_DATA SENSOR_DATA_HANDLER; /*!< Global variable, used for access the measurements. IC_MAX31855_DATA can be accessed through this interface. */
static IC_MAX31855_DATA SENSOR_DATA_ARRAYS[SENSOR_DATA_STORE_LENGTH]; /*!< Global variable, used for storing measurements data. Direct access is not supperted. */


/* 
 *
 *                  FTDI SECTION 
 *
 */

/*! \brief PC communication status
 *
 * PC communication status used to determine the current state int the PC
 * communication state machine. The communication state machine is implemented
 * in ReceiveFTDI_callback() function. This function is always when new data is
 * arrived from the FTDI UART-USB bridge. The current state is the function of
 * the prevoiuos state and one or more incoming bytes.
 * 
 * Control signals are:
 * 10: Simple command data, new command data is received
 * 1: Start heting process
 * 2: Stop heating proces
 * 4: Load data from the buffer to the current heat profile     
 * --
 * 1: Change status to RECEIVE_HEAT_PROFILE
 * --
 * 2: Change status to SEND_HEAT_PROFILE_TO_EEPROM
 * --
 * 3: Change status to RECEIVE_PROFILE_FROM_EEPROM
 * --
 * 4: Change status to WRITE_EEPROM_COMMAND_HIGH_BYTE
 *    That state is always followed byte WRITE_EEPROM_COMMAND_LOW_BYTE
 *    In this two states the program is setting the heat profile's address buffer.
 *    If SEND_HEAT_PROFILE_TO_EEPROM is selected the data will be written to
 *    the location defined by the address buffer.
 */

typedef enum
{
    NORMAL_OPERATION, /*!< Normal operation, the MCU receives control signals. */
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
    uint16_t desired_temp;
} BS_DATA_TYP;
static BS_DATA_TYP BS_DATA_OBJ;
static uint8_t HEAT_IN_PROGRESS = false;


/* 
 *
 *                  TRANSCIEVE STATES
 *
 */

typedef enum {
    TRANSCIEVE_FULL_HEAT_PROFILE,
    TRANSCIEVE_CURRENT_DATA,
    TRANSCIEVE_CURRENT_DATA_WITH_HEAT_ENABLED,
    TRANSCIEVE_CURRENT_DATA_WITH_HEAT_ENABLED_TEST_MODE,
    TRANSCIEVE_HEAT_PROFILE,
    TRANSCIEVE_IDLE
} TRANSCIEVE_STATUS;

typedef enum {
    HEAT_PROFILE_NAME,
    HEAT_PROFILE_ID,
    HEAT_PROFILE_LENGTH,
    HEAT_PROFILE_TEMPERATURE,
    HEAT_PROFILE_TIME
} TRANSCIEVE_PENDING_DATA_TYPE;

typedef struct {
    TRANSCIEVE_PENDING_DATA_TYPE type;
    uint16_t data_offset_counter;
} TRANSCIEVE_PENDING_DATA;

typedef struct {
    TRANSCIEVE_PENDING_DATA pendingData;
    TRANSCIEVE_STATUS status;
    bool NextionTransmissionInProgress;
    bool FTDITransmissionInProgress;
} TRANSCIEVE_OBJ;

static TRANSCIEVE_OBJ transciveObj;

/* 
 *
 *                  TIMER STATES
 *
 */

typedef enum {
    RUNNING,
    STOPPED
} HEAT_TIMER_STATUS;

typedef struct {
    HEAT_TIMER_STATUS status;
    uint16_t timeStamp;
} HEAT_TIMER_OBJ;

static HEAT_TIMER_OBJ heatTimerObj;

/* 
 *
 *                  IMPLEMENTATION SECTION 
 *
 */


void enableHeat() {
    HEAT_IN_PROGRESS = true;
    heatTimerObj.status = RUNNING;
    transciveObj.status = TRANSCIEVE_CURRENT_DATA_WITH_HEAT_ENABLED;
}

void disableHeat() {
    HEAT_IN_PROGRESS = false;
    heatTimerObj.status = STOPPED;
    heatTimerObj.timeStamp = 0x0;
    transciveObj.status = TRANSCIEVE_CURRENT_DATA;
    SSR_OUTPUT_SetHigh();
}

void IdleState_callback() {
    //Toggling signal protection - critical feature
    T_PROTECTION_Toggle();
}


void loadBufferHeatProfile() {
    temperatureHeatProfile.currentProfile.valid=0x0;
    memcpy(temperatureHeatProfile.currentProfile.data, temperatureHeatProfile.bufferProfile.data, HEAT_PROFILE_SIZE);
    memcpy(temperatureHeatProfile.currentProfile.time, temperatureHeatProfile.bufferProfile.time, HEAT_PROFILE_SIZE);
    memcpy(temperatureHeatProfile.currentProfile.name, temperatureHeatProfile.bufferProfile.name, 8);
    temperatureHeatProfile.currentProfile.offset=0;
    temperatureHeatProfile.currentProfile.ID=temperatureHeatProfile.bufferProfile.ID;
    temperatureHeatProfile.currentProfile.size=temperatureHeatProfile.bufferProfile.size;
    temperatureHeatProfile.currentProfile.valid=0x1;
}

void loadDefaultHeatProfile() {
    memcpy(temperatureHeatProfile.currentProfile.data, temperatureHeatProfile.defaultProfile.data, HEAT_PROFILE_SIZE);
    memcpy(temperatureHeatProfile.currentProfile.time, temperatureHeatProfile.defaultProfile.time, HEAT_PROFILE_SIZE);
    memcpy(temperatureHeatProfile.currentProfile.name, temperatureHeatProfile.defaultProfile.name, 8);
    temperatureHeatProfile.currentProfile.offset=0;
    temperatureHeatProfile.currentProfile.ID=0;
    temperatureHeatProfile.currentProfile.size=temperatureHeatProfile.defaultProfile.size;
    temperatureHeatProfile.currentProfile.valid=0x1;
}

bool checkStartConditions() {
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.oc)
        return false; // Thermocouple is open
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.scg)
        return false; // Thermocouple is short-circuited to GND
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.scv)
        return false; // Thermocouple is short-circuited to VCC
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.internal_temperature_data > INTERNAL_MAX_TEMPERATURE)
        return false; // Inernal maximum temperature exceeded
    
    if (SENSOR_DATA_HANDLER.dataArrayQue[SENSOR_DATA_HANDLER.dataArrayStatus.currentData].s.thermocouple_temperature_data > THERMOCOUPLE_MAX_TEMPERATURE)
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

uint16_t readLeatestTemperatreMeasuremntX4() {
        uint8_t data_index = SENSOR_DATA_HANDLER.dataArrayStatus.currentData;
        if (data_index==0) {
            data_index=SENSOR_DATA_STORE_LENGTH;
        } else {
            data_index--;
        }
        
        uint16_t d = 0x0;
        d |= SENSOR_DATA_HANDLER.dataArrayQue[data_index].s.thermocouple_temperature_data;
        d |= SENSOR_DATA_HANDLER.dataArrayQue[data_index].s.thermocouple_temperature_data >> 8;  
        return d;
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
                        loadBufferHeatProfile();
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
    uint8_t msg = 0x0;
    uint8_t debug_var=0;
    
    do {
    
        switch(ftdiStatus) {
            case NORMAL_OPERATION:
                while (!UART2_RX_DATA_AVAILABLE);
                msg = UART2_Read();

                switch(msg) {
                    case 10:
                        while (!UART2_RX_DATA_AVAILABLE);
                        msg = UART2_Read();

                        switch (msg) {
                            case 0x01:
                                // Start heating - critical feature
                                if (checkStartConditions()) {
                                    enableHeat();
                                }
                                break;
                            case 0x02:
                                disableHeat();
                                break;
                            case 0x03:
                                transciveObj.status = TRANSCIEVE_HEAT_PROFILE;
                                break;
                            case 0x04:
                                loadBufferHeatProfile();
                                break;
                            case 0x05:
                                loadDefaultHeatProfile();
                                break;
                            case 0x06:
                                transciveObj.status = TRANSCIEVE_CURRENT_DATA_WITH_HEAT_ENABLED_TEST_MODE;
                                break;
                        }
                        break;
                    case 1:
                        temperatureHeatProfile.profileStatus = TB_RECEIVE_FROM_PC;
                        ftdiStatus = RECEIVE_HEAT_PROFILE;
                        transciveObj.FTDITransmissionInProgress = true;
                        transciveObj.pendingData.data_offset_counter = 0;
                        transciveObj.pendingData.type = HEAT_PROFILE_NAME;
                        temperatureHeatProfile.bufferProfile.valid = 0;
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
                //State machine for receving heat profile
                while(ftdiStatus==RECEIVE_HEAT_PROFILE) {
                    while (!UART2_RX_DATA_AVAILABLE);
                    msg = UART2_Read();
                    switch (transciveObj.pendingData.type) {
                        case HEAT_PROFILE_NAME:
                            temperatureHeatProfile.bufferProfile.name[transciveObj.pendingData.data_offset_counter] = msg;
                            debug_var=debug_var+1;
                            if (++transciveObj.pendingData.data_offset_counter == 8) {
                                transciveObj.pendingData.data_offset_counter=0;
                                transciveObj.pendingData.type = HEAT_PROFILE_ID;
                            }
                            break;
                        case HEAT_PROFILE_ID:
                            temperatureHeatProfile.bufferProfile.ID = msg;
                            transciveObj.pendingData.type = HEAT_PROFILE_LENGTH;
                            break;
                        case HEAT_PROFILE_LENGTH:
                            if (transciveObj.pendingData.data_offset_counter == 0) {
                                temperatureHeatProfile.bufferProfile.size = (uint16_t)msg;
                                transciveObj.pendingData.data_offset_counter++;
                            }
                            else {
                                temperatureHeatProfile.bufferProfile.size += (uint16_t)msg << 8;
                                transciveObj.pendingData.data_offset_counter=0;
                                transciveObj.pendingData.type = HEAT_PROFILE_TEMPERATURE;
                            }
                            break;
                        case HEAT_PROFILE_TEMPERATURE:
                            if (transciveObj.pendingData.data_offset_counter % 2 == 0)
                                temperatureHeatProfile.bufferProfile.data[transciveObj.pendingData.data_offset_counter/2] = (uint16_t)msg;
                            else
                                temperatureHeatProfile.bufferProfile.data[transciveObj.pendingData.data_offset_counter/2] += (uint16_t)msg<<8;

                            if (++transciveObj.pendingData.data_offset_counter == ((temperatureHeatProfile.bufferProfile.size + 1) << 1)) {
                                transciveObj.pendingData.data_offset_counter=0;
                                transciveObj.pendingData.type = HEAT_PROFILE_TIME;
                            }
                            break;
                        case HEAT_PROFILE_TIME:
                            if (transciveObj.pendingData.data_offset_counter % 2 == 0)
                                temperatureHeatProfile.bufferProfile.time[transciveObj.pendingData.data_offset_counter/2] = (uint16_t)msg;
                            else
                                temperatureHeatProfile.bufferProfile.time[transciveObj.pendingData.data_offset_counter/2] += (uint16_t)msg<<8;

                            if (++transciveObj.pendingData.data_offset_counter == ((temperatureHeatProfile.bufferProfile.size + 1) << 1)) {
                                temperatureHeatProfile.bufferProfile.valid = 1;
                                ftdiStatus = NORMAL_OPERATION;
                            }
                            break;
                        default:
                            transciveObj.FTDITransmissionInProgress = false;
                            ftdiStatus = NORMAL_OPERATION;
                            break;
                    }
                }
                break;
            case WRITE_EEPROM_COMMAND_HIGH_BYTE:
                temperatureHeatProfile.addressBuffer = msg << 8;
                ftdiStatus = WRITE_EEPROM_COMMAND_LOW_BYTE;
                break;
            case WRITE_EEPROM_COMMAND_LOW_BYTE:
                temperatureHeatProfile.addressBuffer |= msg;
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
    } while (!UART2_TRANSFER_STATUS_RX_EMPTY);
}

void genericTranciverFunction() {
    
        // Get sensor data
    
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
                
        // Get time
        uint16_t timeStamp = heatTimerObj.timeStamp;
        uint8_t lowerTimeStamp = (uint8_t)timeStamp;
        uint8_t upperTimeStamp = (uint8_t)(timeStamp >> 8);
        
        // Get desired value
        uint16_t desVal = BS_DATA_OBJ.desired_temp;
        uint8_t lowerDesVal = (uint8_t)desVal;
        uint8_t upperDesVal = (uint8_t)(desVal >> 8);
        
        // Temporary variables for data transmission
        uint16_t a,it;
        
        switch(transciveObj.status) {
        case TRANSCIEVE_IDLE:
            break;
        case TRANSCIEVE_FULL_HEAT_PROFILE:
            /* TODO : WriteBuffer accepts unit8_t, heat profile data is uint16_t*/
            //UART1_WriteBuffer(temperatureHeatProfile.currentProfile.data , HEAT_PROFILE_SIZE );
            //UART2_WriteBuffer(temperatureHeatProfile.currentProfile.data , HEAT_PROFILE_SIZE );
            break;
        case TRANSCIEVE_CURRENT_DATA:
            //UART1_Write(0xFF);
            //UART1_Write(0xFF);
            //UART1_Write(temp_lo);
            //UART1_Write(temp_hi);
            //UART1_Write(int_temp_lo);
            //UART1_Write(int_temp_hi);

            UART2_Write(0xFF);
            UART2_Write(0xFF);
            UART2_Write(temp_lo);
            UART2_Write(temp_hi);
            UART2_Write(int_temp_lo);
            UART2_Write(int_temp_hi);

            break;
            
        case TRANSCIEVE_CURRENT_DATA_WITH_HEAT_ENABLED:
            UART2_Write(0xFF);
            UART2_Write(0xFE);
            UART2_Write(temp_lo);
            UART2_Write(temp_hi);
            UART2_Write(int_temp_lo);
            UART2_Write(int_temp_hi);
            UART2_Write(lowerTimeStamp);
            UART2_Write(upperTimeStamp);
            break;
        case TRANSCIEVE_HEAT_PROFILE:
            // DEBUG: Data sent to pin header
            a = (temperatureHeatProfile.currentProfile.size+1);

             
            while (U2STAbits.UTXBF);
                UART2_Write(0xFF);
            while (U2STAbits.UTXBF);
                UART2_Write(0xFD);
            for (it=0;it<8;it++) {
                while (U2STAbits.UTXBF);
                    UART2_Write(temperatureHeatProfile.currentProfile.name[it]);
            }
            while (U2STAbits.UTXBF);
                UART2_Write(temperatureHeatProfile.currentProfile.ID);
            while (U2STAbits.UTXBF);
                UART2_Write(temperatureHeatProfile.currentProfile.size);
            while (U2STAbits.UTXBF);
                UART2_Write(temperatureHeatProfile.currentProfile.size >> 8);
                
            for (it=0;it<a;it++) {
                while (U2STAbits.UTXBF);
                     UART2_Write(temperatureHeatProfile.currentProfile.data[it]);
                while (U2STAbits.UTXBF);
                     UART2_Write(temperatureHeatProfile.currentProfile.data[it] >> 8);
            }
            
            for (it=0;it<a;it++) {
                while (U2STAbits.UTXBF);
                     UART2_Write(temperatureHeatProfile.currentProfile.time[it]);
                while (U2STAbits.UTXBF);
                     UART2_Write(temperatureHeatProfile.currentProfile.time[it] >> 8);
            }
            transciveObj.status = TRANSCIEVE_CURRENT_DATA;
            break;
        case TRANSCIEVE_CURRENT_DATA_WITH_HEAT_ENABLED_TEST_MODE:
            UART2_Write(0xFF);
            UART2_Write(0xFC);
            UART2_Write(temp_lo);
            UART2_Write(temp_hi);
            UART2_Write(int_temp_lo);
            UART2_Write(int_temp_hi);
            UART2_Write(lowerTimeStamp);
            UART2_Write(upperTimeStamp);
            while (U2STAbits.UTXBF);
            UART2_Write(lowerDesVal);
            UART2_Write(upperDesVal);
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
    /* TODO: temp data is uint16_t */
    //EEPROM_Read(EEPROM_ADDRESS,temperatureHeatProfile.addressBuffer,temperatureHeatProfile.bufferProfile.data,HEAT_PROFILE_SIZE);
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
    
    /* TODO : Temp data is uint16_t*/
    
    /* If the buffer input is not equal to zero there is a transmission in proggress
     and the data in buffer is not consistent. */
    if (temperatureHeatProfile.bufferProfile.offset == 0) {
        uint8_t page_size = 32;
        uint8_t write_cycles_max = HEAT_PROFILE_SIZE / page_size;
        uint8_t current_write_cycle;
        uint16_t offset;

        for (current_write_cycle = 0; current_write_cycle < write_cycles_max; current_write_cycle++) {
            offset = temperatureHeatProfile.bufferProfile.offset;
            //EEPROM_WritePage(EEPROM_ADDRESS,temperatureHeatProfile.addressBuffer,&(temperatureHeatProfile.bufferProfile.data[offset*page_size]));
        }

    }
    temperatureHeatProfile.profileStatus = IDLE;
}



/* Interrupt service routines */

void baseSW_TMR2_ISR(void) {
    
    // SSR control
    if (HEAT_IN_PROGRESS) {
        if (checkStartConditions()) {
            // Increase timeStamp by 100 ms
            uint16_t cTimeStamp = (heatTimerObj.timeStamp++);
            
            //Get current temperature offset pointer
            uint16_t cOffset = temperatureHeatProfile.currentProfile.offset;
            
            //Get the number of records in the heat profile
            uint16_t mOffset = temperatureHeatProfile.currentProfile.size;

            // Check if we reached the end of the heating process
            if (cOffset+1 > mOffset) {
                disableHeat();
                return;
            } else {
                // Calculate desired temperature
                while (temperatureHeatProfile.currentProfile.time[cOffset+1] < cTimeStamp) {
                    if ((++cOffset)+1 > mOffset) {
                        disableHeat();
                        return;
                    }
                }
                temperatureHeatProfile.currentProfile.offset = cOffset;
                
                
                // Get the desired temperature & time in nearest defined points
                int16_t temperature1 = temperatureHeatProfile.currentProfile.data[cOffset];
                int16_t temperature2 = temperatureHeatProfile.currentProfile.data[cOffset+1];
                int16_t time1 = temperatureHeatProfile.currentProfile.time[cOffset];
                int16_t time2 = temperatureHeatProfile.currentProfile.time[cOffset+1];
                
                int16_t dTtime = time2 - time1;
                int16_t dTtemp = temperature2 - temperature1;
                int16_t dcTtime = cTimeStamp - time1;
                
                
                
                // Calculate the desired temperature
                double ratio = (double)dcTtime/(double)dTtime;
                double diff = ratio * (double)(dTtemp);
                BS_DATA_OBJ.desired_temp = (uint16_t)(diff+temperature1);

                // Get measured temperature
                uint16_t current_temperature_X4 = readLeatestTemperatreMeasuremntX4();
                
                if (BS_DATA_OBJ.desired_temp > current_temperature_X4) {
                    SSR_OUTPUT_SetLow();
                }
                else {
                    SSR_OUTPUT_SetHigh();
                }
            }
            
        } else {
            disableHeat();
        }
    }
    
    // Data transmission
    
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
    
    //Disable heat
    disableHeat();
    
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
    temperatureHeatProfile.currentProfile.time = heatProfileCurrentTime;
    temperatureHeatProfile.currentProfile.offset = 0x0;
    
    temperatureHeatProfile.bufferProfile.data = heatProfileBuffer;
    temperatureHeatProfile.bufferProfile.time = heatProfileBufferTime;
    temperatureHeatProfile.bufferProfile.offset = 0x0;
    
    temperatureHeatProfile.defaultProfile.data = heatProfileDefult;
    temperatureHeatProfile.defaultProfile.time = heatProfileDefultTime;
    temperatureHeatProfile.defaultProfile.offset = 0x0;
    
    temperatureHeatProfile.profileStatus = TRANSCIEVE_CURRENT_DATA;
    
    //Initialize temperature sensor interface
    
    SENSOR_DATA_HANDLER.dataArrayQue = SENSOR_DATA_ARRAYS;
    SENSOR_DATA_HANDLER.dataArrayStatus.isUploaded = 0x0;
    SENSOR_DATA_HANDLER.dataArrayStatus.currentData = 0x0;
    
    //Transcieve object
    transciveObj.FTDITransmissionInProgress=false;
    transciveObj.NextionTransmissionInProgress=false;
    transciveObj.status = TRANSCIEVE_HEAT_PROFILE;
    
    
    // Create default heat profile
    temperatureHeatProfile.defaultProfile.time[0]=0;    // 0 ms
    temperatureHeatProfile.defaultProfile.time[1]=750;  // 75s = 750 * 100 ms
    temperatureHeatProfile.defaultProfile.time[2]=1550; // 155s = 1550 * 100 ms
    temperatureHeatProfile.defaultProfile.time[3]=2150; // 215s = 2150 * 100 ms
    temperatureHeatProfile.defaultProfile.time[4]=2350; // 235s = 2350 * 100 ms
    temperatureHeatProfile.defaultProfile.time[5]=2550; // 255s = 2550 * 100 ms
    temperatureHeatProfile.defaultProfile.time[6]=3700; // 370s = 3700 * 100 ms
    
    temperatureHeatProfile.defaultProfile.data[0]=100;  // 25 * 4 = 100
    temperatureHeatProfile.defaultProfile.data[1]=600;  // 150 * 4 = 600
    temperatureHeatProfile.defaultProfile.data[2]=720;  // 180 * 4 = 720
    temperatureHeatProfile.defaultProfile.data[3]=960;  // 240 * 4 = 960
    temperatureHeatProfile.defaultProfile.data[4]=1000; // 250 * 4 = 1000
    temperatureHeatProfile.defaultProfile.data[5]=960;  // 240 * 4 = 960
    temperatureHeatProfile.defaultProfile.data[6]=100;  // 25 * 4 = 100
    
    temperatureHeatProfile.defaultProfile.valid = 0x1;  // Heat profile is valid
    temperatureHeatProfile.defaultProfile.size = 6;   // Maximum offset in records
    temperatureHeatProfile.defaultProfile.offset = 0x0; // Offset pointer
    temperatureHeatProfile.defaultProfile.ID = 0x0;
    temperatureHeatProfile.defaultProfile.name[0] = 'D';
    temperatureHeatProfile.defaultProfile.name[1] = 'e';
    temperatureHeatProfile.defaultProfile.name[2] = 'f';
    temperatureHeatProfile.defaultProfile.name[3] = 'a';
    temperatureHeatProfile.defaultProfile.name[4] = 'u';
    temperatureHeatProfile.defaultProfile.name[5] = 'l';
    temperatureHeatProfile.defaultProfile.name[6] = 't';
    temperatureHeatProfile.defaultProfile.name[7] = 0x0;
    
    // Initialie current buffer with the default heat profile
    loadDefaultHeatProfile();
    
    return IdleState;

}

