/*
 * File Name: LT8722SPI.cpp
 * Description: Functions required to communicate with the LT8722 via the SPI 
 *              interface. Basic functions are implemented to communicate at
 *              register level and more advanced functions that utilize  these 
 *              functions to change specific bits or areas of the registers.
 *
 * Notes: This code was written as part of my master's thesis at the 
 *        Institute for Microsensors, -actuators and -systems (IMSAS) 
 *        at the University of Bremen.
 */

#include "LT8722SPI.h"
#include "CRC8.h"

/**************************************************************************/
/*!
    @brief Read the status register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing data, status,crc, ack and error
*/
/**************************************************************************/
dataSPI readStatus(SPIClass* spi, uint8_t cs){
  struct dataSPI dataPacket;

  uint8_t command = 0xF0;                       //status acquisition command
  uint8_t address = (0x01 << 1) & 0xFE;         //SPI_STATUS address A[7:1] 
  uint8_t sendingPacket[] = {command, address};

  spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  dataPacket.status[0] = spi->transfer((int8_t)sendingPacket[0]);
  dataPacket.status[1] = spi->transfer((int8_t)sendingPacket[1]);
  dataPacket.crc = spi->transfer((int8_t)getCRC2(sendingPacket));
  dataPacket.ack = spi->transfer((int8_t)0x00);
  digitalWrite(cs, HIGH);
  spi->endTransaction();

  //fill dataPacket struct with empty data bytes 
  for (uint8_t i = 0; i < 4; i++)
  {
    dataPacket.data[i] = (int8_t)0x00;
  }
  
  //check for crc errors
  if (dataPacket.ack == 0xA5) {
    if (checkCRC(dataPacket.status, dataPacket.data, 2, dataPacket.crc)) {
      dataPacket.error = false;
    } else {
      dataPacket.error = true;
    }
  } else {
    dataPacket.error = true;
  }
  
  return dataPacket;
}

/**************************************************************************/
/*!
    @brief Read the data of a specified register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param address Address of the register to be read
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI readRegister(SPIClass* spi, uint8_t cs, uint8_t address) {
  struct dataSPI dataPacket;

  uint8_t command = 0xF4;                       //data read command
  address = (address << 1) & 0xFE;              //register address A[7:1] 
  uint8_t sendingPacket[] = {command, address};

  spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  dataPacket.status[0] = spi->transfer((int8_t)sendingPacket[0]);
  dataPacket.status[1] = spi->transfer((int8_t)sendingPacket[1]);
  dataPacket.data[0] = spi->transfer((int8_t)getCRC2(sendingPacket));
  dataPacket.data[1] = spi->transfer((int8_t)0x00);
  dataPacket.data[2] = spi->transfer((int8_t)0x00);
  dataPacket.data[3] = spi->transfer((int8_t)0x00);
  dataPacket.crc = spi->transfer((int8_t)0x00);
  dataPacket.ack = spi->transfer((int8_t)0x00);
  digitalWrite(cs, HIGH);
  spi->endTransaction();

  //check for crc errors
  if (dataPacket.ack == 0xA5) {
    if (checkCRC(dataPacket.status, dataPacket.data, 6, dataPacket.crc)) {
      dataPacket.error = false;
    } else {
      dataPacket.error = true;
    }
  } else {
    dataPacket.error = true;
  }
  
  return dataPacket;
}

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
dataSPI writeRegister(SPIClass* spi, uint8_t cs, uint8_t address, uint8_t *data) {
  struct dataSPI dataPacket;

  uint8_t command = 0xF2;                       //data write command
  address = (address << 1) & 0xFE;              //register address A[7:1] 
  uint8_t sendingPacket[] = {command, address};

  spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  dataPacket.status[0] = spi->transfer((int8_t)sendingPacket[0]);
  dataPacket.status[1] = spi->transfer((int8_t)sendingPacket[1]);
  dataPacket.crc = spi->transfer((int8_t)data[0]);
  dataPacket.data[0] = spi->transfer((int8_t)data[1]);
  dataPacket.data[1] = spi->transfer((int8_t)data[2]);
  dataPacket.data[2] = spi->transfer((int8_t)data[3]);
  dataPacket.data[3] = spi->transfer(getCRC6(sendingPacket, data));
  dataPacket.ack = spi->transfer((int8_t)0x00);
  digitalWrite(cs, HIGH);
  spi->endTransaction();

  //check for crc errors
  if (dataPacket.ack == 0xA5) {
    if (checkCRC(dataPacket.status, dataPacket.data, 2, dataPacket.crc)) {
      dataPacket.error = false;
    } else {
      dataPacket.error = true;
    }
  } else {
    dataPacket.error = true;
  }
  
  return dataPacket;
}


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
dataSPI changeBitsInRegister(SPIClass* spi, uint8_t cs, uint8_t address, uint8_t startBit, uint8_t numBits, uint8_t value) {
  struct dataSPI dataPacket1 = readRegister(spi, cs, address);

  //change bits in dataPacket1 according to startBit, numBit and value
  for (uint8_t i = 0; i < numBits; i++) {
    uint8_t bitPosition = startBit + i;
    int byteIndex = 3 - (bitPosition / 8);
    int bitOffset = bitPosition % 8;
    bool bitValue = (value >> i) & 0x01;

    if (bitValue) {
      dataPacket1.data[byteIndex] |= (1 << bitOffset);
    } else {
      dataPacket1.data[byteIndex] &= ~(1 << bitOffset);
    }
  }

  struct dataSPI dataPacket2 = writeRegister(spi, cs, address, dataPacket1.data);

  //check for crc errors
  if (dataPacket1.ack == 0xA5 && dataPacket2.ack == 0xA5) {
    if (!(dataPacket1.error || dataPacket2.error)) {
      dataPacket1.error = false;
    } else {
      dataPacket1.error = true;
    }
  } else {
    dataPacket1.error = true;
  }

  return dataPacket1;
}

/**************************************************************************/
/*!
    @brief Reset all registers apart from the status registers
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI resetRegisters(SPIClass* spi, uint8_t cs) {
  setCommandRegister(spi, cs, COMMAND_REG::SPI_RST, ENABLE);                              //set SPI_RST bit in command register to 1
  struct dataSPI dataPacket = setCommandRegister(spi, cs, COMMAND_REG::SPI_RST, DISABLE); //set SPI_RST bit in command register to 0

  return dataPacket;
}


/**************************************************************************/
/*!
    @brief Reset the status register
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI resetStatusRegister(SPIClass* spi, uint8_t cs) {
  uint8_t data[] = {0x00, 0x00, 0x00, 0x00};

  struct dataSPI dataPacket = writeRegister(spi, cs, 0x1, data);  //set all bits of status register to 0

  return dataPacket;
}

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
dataSPI setCommandRegister(SPIClass* spi, uint8_t cs, COMMAND_REG symbol, uint8_t value) {
  uint8_t regSymbol = static_cast<uint8_t>(symbol);
  struct dataSPI dataPacket;

  //change command register according to symbol and value
  switch (symbol)
  {
  case COMMAND_REG::ENABLE_REQ:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 1, value); 
    break;
  case COMMAND_REG::SWEN_REQ:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 1, value); 
    break;
  case COMMAND_REG::SW_FRQ_SET:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 3, value); 
    break;
  case COMMAND_REG::SW_FRQ_ADJ:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 2, value); 
    break;
  case COMMAND_REG::SYS_DC:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 2, value); 
    break;
  case COMMAND_REG::VCC_VREG:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 1, value); 
    break;
  case COMMAND_REG::SW_VC_INT:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 3, value); 
    break;
  case COMMAND_REG::SPI_RST:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 1, value); 
    break;
  case COMMAND_REG::PWR_LIM:
    dataPacket = changeBitsInRegister(spi, cs, 0x00, regSymbol, 4, value); 
    break;
  default:
    dataPacket.error = 1;
    break;
  }

  return dataPacket;
}

/**************************************************************************/
/*!
    @brief Set the output voltage
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param voltage Desired output Voltage
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI setOutputVoltage(SPIClass* spi, uint8_t cs, double voltage) {
  uint32_t registerValue = 0;
  uint8_t data[4];

  //calculate needed register value to set the correct output voltage limit
  if (voltage >= 1.25) {
    registerValue = 4294967296 - ((voltage - 1.25) / (2.5 * pow(2, -25)));
  } else {
    registerValue = (voltage - 1.25) / -(2.5 * pow(2, -25));
  }

  //fill data array bit by bit with the new register value to set the output voltage
  for (uint8_t i = 0; i < 4; i++) {
    data[3 - i] = registerValue >> (i * 8);
  }

  struct dataSPI dataPacket = writeRegister(spi, cs, 0x4, data);

  return dataPacket;
}

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
dataSPI rampOutputVoltage(SPIClass* spi, uint8_t cs, double start, double end, double stepSize, uint8_t duration) {
  double currentValue = start;
  double steps = fabs((end - start) / stepSize);
  double delayTime = duration / steps; 
 
  //calculate the needed output voltage according to start, end, stepSize and duration
  for (uint32_t i = 0; i <= (steps - 1); i++) {
    if (start >= end) {
      currentValue -= stepSize;
    } else {
      currentValue += stepSize;
    }
    setOutputVoltage(spi, cs, currentValue);
    delay(delayTime);
  }

  return readRegister(spi, cs, 0x4);
}

/**************************************************************************/
/*!
    @brief Enable the analog output
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI enableAnalogOutput(SPIClass* spi, uint8_t cs){
    struct dataSPI dataPacket = changeBitsInRegister(spi, cs, 0x07, 6, 1, 0x01); //set AOUT_EN bit of SPIS_AMUX register to one

    return dataPacket;
}

/**************************************************************************/
/*!
    @brief Disable the analog output
    @param spi SPI object
    @param cs Chip select (sc) pin
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI disableAnalogOutput(SPIClass* spi, uint8_t cs){
    struct dataSPI dataPacket = changeBitsInRegister(spi, cs, 0x07, 6, 1, 0x00); //set AOUT_EN bit of SPIS_AMUX register to zero

    return dataPacket;
}

/**************************************************************************/
/*!
    @brief Set the analog output AMUX value
    @param spi SPI object
    @param cs Chip select (sc) pin
    @param value Analog output AMUX value
    @return dataSPI structure containing status, data, crc, ack and error
*/
/**************************************************************************/
dataSPI setAnalogOutput(SPIClass* spi, uint8_t cs, uint8_t value){
    struct dataSPI dataPacket = changeBitsInRegister(spi, cs, 0x07, 0, 4, value); //set AMUX[3:0] bits of SPIS_AMUX register to desired value

    return dataPacket;
}