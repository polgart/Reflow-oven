#include <xc.h>
#include "EEPROM_driver.h"
#include "mcc_generated_files/i2c1.h"
#include "piclib30_wrapper.h"

I2C1_MESSAGE_STATUS EEPROM_Read(
                               uint16_t slaveDeviceAddress,
                               uint16_t dataAddress,
                               uint8_t *pData,
                               uint16_t nCount)
{
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    uint8_t     writeBuffer[3];
    uint16_t    timeOut, slaveTimeOut;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // build the write buffer first
    // starting address of the EEPROM memory
    writeBuffer[0] = (dataAddress >> 8);                        // high address
    writeBuffer[1] = (uint8_t)(dataAddress);                    // low low address

    // we need to create the TRBs for a random read sequence to the EEPROM
    // Build TRB for sending address
    I2C1_MasterWriteTRBBuild(    &readTRB[0],
                                    writeBuffer,
                                    2,
                                    slaveDeviceAddress);
    // Build TRB for receiving data
    I2C1_MasterReadTRBBuild(     &readTRB[1],
                                    pData,
                                    nCount,
                                    slaveDeviceAddress);

    timeOut = 0;
    slaveTimeOut = 0;

    while(status != I2C1_MESSAGE_FAIL)
    {
        // now send the transactions
        I2C1_MasterTRBInsert(2, readTRB, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING)
        {
            // add some delay here
            __delay_ms(50);
            // timeout checking
            // check for max retry and skip this byte
            if (slaveTimeOut == DEVICE_TIMEOUT)
                return status;
            else
                slaveTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C1_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here

        // check for max retry and skip this byte
        if (timeOut == DEVICE_RETRY_MAX)
            return status;
        else
            timeOut++;

    }
    return status;

}

I2C1_MESSAGE_STATUS EEPROM_Write(
                               uint16_t slaveDeviceAddress,
                               uint16_t dataAddress,
                               uint8_t *pData,
                               uint16_t nCount) {

    uint8_t         writeBuffer[3];
    uint16_t        counter, timeOut, slaveTimeOut;

    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    for (counter = 0; counter < nCount; counter++)
    {

        // build the write buffer first
        // starting address of the EEPROM memory
        writeBuffer[0] = (dataAddress >> 8);                // high address
        writeBuffer[1] = (uint8_t)(dataAddress);            // low low address

        // data to be written
        writeBuffer[2] = *pData++;

        // Now it is possible that the slave device will be slow.
        // As a work around on these slaves, the application can
        // retry sending the transaction
        timeOut = 0;
        slaveTimeOut = 0;

        while(status != I2C1_MESSAGE_FAIL)
        {
            // write one byte to EEPROM (3 is the number of bytes to write)
            I2C1_MasterWrite(  writeBuffer,
                                    3,
                                    slaveDeviceAddress,
                                    &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C1_MESSAGE_PENDING)
            {
                // add some delay here

                // timeout checking
                // check for max retry and skip this byte
                if (slaveTimeOut == DEVICE_TIMEOUT)
                    break;
                else
                    slaveTimeOut++;
            } 
            if ((slaveTimeOut == DEVICE_TIMEOUT) || 
                (status == I2C1_MESSAGE_COMPLETE))
                break;

            // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
            //               or I2C1_DATA_NO_ACK,
            // The device may be busy and needs more time for the last
            // write so we can retry writing the data, this is why we
            // use a while loop here

            // check for max retry and skip this byte
            if (timeOut == DEVICE_RETRY_MAX)
                break;
            else
                timeOut++;
        }

        if (status == I2C1_MESSAGE_FAIL)
        {
            break;
        }
        dataAddress++;

    }
    return status;
}

I2C1_MESSAGE_STATUS EEPROM_WritePage(
                               uint16_t slaveDeviceAddress,
                               uint16_t dataAddress,
                               uint8_t *pData)
{
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK writeTRB[2];
    uint8_t     writeBuffer[3];
    uint16_t    timeOut, slaveTimeOut;
    uint8_t nCount = 32;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // build the write buffer first
    // starting address of the EEPROM memory
    writeBuffer[0] = (dataAddress >> 8);                        // high address
    writeBuffer[1] = (uint8_t)(dataAddress);                    // low low address

    // we need to create the TRBs for a random read sequence to the EEPROM
    // Build TRB for sending address
    I2C1_MasterWriteTRBBuild(    &writeTRB[0],
                                    writeBuffer,
                                    2,
                                    slaveDeviceAddress);
    // Build TRB for receiving data
    I2C1_MasterWriteTRBBuild(     &writeTRB[1],
                                    pData,
                                    nCount,
                                    slaveDeviceAddress);

    timeOut = 0;
    slaveTimeOut = 0;

    while(status != I2C1_MESSAGE_FAIL)
    {
        // now send the transactions
        I2C1_MasterTRBInsert(2, writeTRB, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING)
        {
            // add some delay here
            __delay_ms(50);
            // timeout checking
            // check for max retry and skip this byte
            if (slaveTimeOut == DEVICE_TIMEOUT)
                return status;
            else
                slaveTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C1_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here

        // check for max retry and skip this byte
        if (timeOut == DEVICE_RETRY_MAX)
            return status;
        else
            timeOut++;

    }
    return status;

}