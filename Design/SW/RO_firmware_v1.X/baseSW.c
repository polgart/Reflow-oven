#include <stdlib.h>
#include "baseSW.h"
#include "stateTaskHandler.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/i2c1.h"


stateTaskList* IdleState = NULL;
stateTaskList* ReadTemperatureData = NULL;
stateTaskList* ReceiveNextionData = NULL;
stateTaskList* ReceiveFTDI = NULL;
stateTaskList* TranscieveNextionDATA = NULL;
stateTaskList* TransciveFTDI = NULL;
stateTaskList* ReadEEPROM = NULL;
stateTaskList* WriteEEPROM = NULL;



void IdleState_callback() {
    //Temperature protection - critical feature
    
    //Toggling signal protection - critical feature
    
}

void ReadTemperatureData_callback() {
    uint32_t sensorData = SPI1_Exchange32bit((uint32_t)0x0);
    // Process sensor data - critical feature
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

void ReceiveFTDI_callback() {    
    uint8_t msg = UART1_Read();
    switch(msg) {
        case 0:
            msg = UART1_Read();
            if ((msg | 0xFB) == 0xFF) {
                // Start heating - critical feature
            }
            if ((msg | 0xFD) == 0xFF) {
                //Stop heating - critical feature
            }
            if ((msg | 0xFE) == 0xFF) {
                //Set selected heat profile - non critical feature
            }
            break;
        case 1:
            //Configuration mode  - non critical feature
            break;
        default:
            //Error handler - non critical feature
            break;
    }
}

void TranscieveNextionDATA_callback() {
    //UART1_WriteBuffer();
}

void TransciveFTDI_callback() {
    //UART2_WriteBuffer();
}

void ReadEEPROM_callback() {
    //I2C1_MasterRead(); Read data from EEPROM - non critical feature
}

void WriteEEPROM_callback() {
    //I2C_MasterWrite(); Write data to EEPROM - non critical feature
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
    
    
    return IdleState;

}

