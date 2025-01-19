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
dataSPI readStatus(SPIClass* spi, uint8_t cs);
dataSPI readRegister(SPIClass* spi, uint8_t cs, uint8_t address);
dataSPI writeRegister(SPIClass* spi, uint8_t cs, uint8_t address, uint8_t *data);
dataSPI changeBitsInRegister(SPIClass* spi, uint8_t cs, uint8_t address, uint8_t startBit, uint8_t numBits, uint8_t value);

//functions to reset specific registers
dataSPI resetRegisters(SPIClass* spi, uint8_t cs);
dataSPI resetStatusRegister(SPIClass* spi, uint8_t cs);

//functions to change specific register values
dataSPI setCommandRegister(SPIClass* spi, uint8_t cs, COMMAND_REG symbol, uint8_t value);
dataSPI setOutputVoltage(SPIClass* spi, uint8_t cs, double voltage);
dataSPI rampOutputVoltage(SPIClass* spi, uint8_t cs, double start, double end, double stepSize, uint8_t duration);

//functions for analog output control
dataSPI enableAnalogOutput(SPIClass* spi, uint8_t cs);
dataSPI disableAnalogOutput(SPIClass* spi, uint8_t cs);
dataSPI setAnalogOutput(SPIClass* spi, uint8_t cs, uint8_t value);

#endif