/* 
 * File:   EEPROM_driver.h
 * Author: EDU_OBOS_5057
 *
 * Created on 2021. február 20., 23:36
 */

#ifndef EEPROM_DRIVER_H
#define	EEPROM_DRIVER_H
#include "mcc_generated_files/i2c1.h"
#define DEVICE_RETRY_MAX       100  // define the retry count
#define DEVICE_ADDRESS         0x50 // slave device address
#define DEVICE_TIMEOUT  50   // define slave timeout 


#ifdef	__cplusplus
extern "C" {
#endif

    I2C1_MESSAGE_STATUS EEPROM_Read(uint16_t slaveDeviceAddress,uint16_t dataAddress,uint8_t *pData,uint16_t nCount);
    I2C1_MESSAGE_STATUS EEPROM_Write(uint16_t slaveDeviceAddress,uint16_t dataAddress,uint8_t *pData,uint16_t nCount);
    I2C1_MESSAGE_STATUS EEPROM_WritePage(uint16_t slaveDeviceAddress,uint16_t dataAddress,uint8_t *pData);


#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_DRIVER_H */

