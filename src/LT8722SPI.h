/*
 * File Name: LT8722SPI.h
 * Description: Functions required to communicate with the LT8722 via the SPI 
 *              interface. Basic functions are implemented to communicate at
 *              register level and more advanced functions that utilize  these 
 *              functions to change specific bits or areas of the registers.
 *
 * Notes: This code was written as part of my master's thesis at the 
 *        Institute for Microsensors, -actuators and -systems (IMSAS) 
 *        at the University of Bremen.
 */

#ifndef LT8722SPI_H
#define LT8722SPI_H

#include <Arduino.h>
#include <SPI.h>

enum class COMMAND_REG : uint8_t{
    ENABLE_REQ = 0,
    SWEN_REQ   = 1,
    SW_FRQ_SET = 2,
    SW_FRQ_ADJ = 5,
    SYS_DC     = 7,
    VCC_VREG   = 9,
    SW_VC_INT  = 11,
    SPI_RST    = 14,
    PWR_LIM    = 15,
};

#define DISABLE 0x00
#define ENABLE  0x01

struct dataSPI {
    uint8_t status[2];
    uint8_t data[4];
    uint8_t crc;
    uint8_t ack;
    bool error;
};

//basic functions for on register level communications

/**************************************************************************/
/*!
    @brief Read the status register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing data, status,crc, ack and error
*/
/**************************************************************************/
dataSPI readStatus(SPIClass* spi, uint8_t cs);

/**************************************************************************/
/*!
    @brief Read the data of a specified register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param address Address of the register to be read
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI readRegister(SPIClass* spi, uint8_t cs, uint8_t address);

/**************************************************************************/
/*!
    @brief Write given data to a specified register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param address Address of the register to be written to
    @param data Data to be written to the register
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI writeRegister(SPIClass* spi, uint8_t cs, uint8_t address, uint8_t *data);

/**************************************************************************/
/*!
    @brief Change certain bits of a specified register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param address Address of the register to be written to
    @param startBit First bit to be changed
    @param numBit Number of bits to be changed
    @param value DValue of the bits to be changed
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI changeBitsInRegister(SPIClass* spi, uint8_t cs, uint8_t address, uint8_t startBit, uint8_t numBits, uint8_t value);

//functions to reset specific registers

/**************************************************************************/
/*!
    @brief Reset all registers apart from the status registers
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI resetRegisters(SPIClass* spi, uint8_t cs);

/**************************************************************************/
/*!
    @brief Reset the status register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI resetStatusRegister(SPIClass* spi, uint8_t cs);

//functions to change specific register values

/**************************************************************************/
/*!
    @brief Change the settings of the command register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param symbol Symbol of the command register to be changed
    @param value New value for the symbol of the command register
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI setCommandRegister(SPIClass* spi, uint8_t cs, COMMAND_REG symbol, uint8_t value);

/**************************************************************************/
/*!
    @brief Set the output voltage
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param voltage Desired output Voltage
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI setOutputVoltage(SPIClass* spi, uint8_t cs, double voltage);

/**************************************************************************/
/*!
    @brief Ramp the output voltage from a start value to an end value in a 
           given period of time
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param start initial output voltage
    @param end desired output voltage
    @param stepSize Step size for voltage increase
    @param duration Duration of the voltage increase
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI rampOutputVoltage(SPIClass* spi, uint8_t cs, double start, double end, double stepSize, uint8_t duration);

//functions for analog output control

/**************************************************************************/
/*!
    @brief Enable the analog output
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI enableAnalogOutput(SPIClass* spi, uint8_t cs);

/**************************************************************************/
/*!
    @brief Disable the analog output
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI disableAnalogOutput(SPIClass* spi, uint8_t cs);

/**************************************************************************/
/*!
    @brief Set the analog output AMUX value
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param value Analog output AMUX value
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI setAnalogOutput(SPIClass* spi, uint8_t cs, uint8_t value);

#endif