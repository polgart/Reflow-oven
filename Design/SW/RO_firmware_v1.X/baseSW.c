#include <stdlib.h>
#include "baseSW.h"
#include "stateTaskHandler.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"


stateTaskList* IdleState = NULL;
stateTaskList* ReadTemperatureData = NULL;
stateTaskList* ReceiveNextionData = NULL;
stateTaskList* ReceiveFTDI = NULL;
stateTaskList* TranscieveNextionDATA = NULL;
stateTaskList* TransciveFTDI = NULL;


/**
  baseSW_TMR2_ISR 
  @Summary
 * Timer 2 inetrrupt service routine
  @Description
 * Send data to Nextion and PC alternately
 * 
 */

void IdleState_callback() {
    //Temperature protection
    
    //Toggling signal protection
    
}

void ReadTemperatureData_callback() {
    
}

void ReceiveNextionData_callback() {
    //El kell dönteni hány byteos legyen a protokol
}

void ReceiveFTDI_callback() {
    //El kell dönteni hány byteos legyen a protokol
}

void TranscieveNextionDATA_callback() {
    UART1_WriteBuffer();
}

void TransciveFTDI_callback() {
    UART2_WriteBuffer();
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
    
    // Initialize task handler
    initilaizeTaskHandler(IdleState);

    // Initilize ISRs
    TMR2_SetInterruptHandler(baseSW_TMR2_ISR);
    UART1_SetRxInterruptHandler(baseSW_UART1_rec_ISR); // UART1 -> NEXTION
    UART2_SetRxInterruptHandler(baseSW_UART2_rec_ISR); // UART2 -> FTDI
    
    
    return IdleState;

}

