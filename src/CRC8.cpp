/*
 * File Name: CRC.cpp
 * Description: Functions to generate the CRC value required for the SPI
 *              communication with the LT8722.
 *
 * Notes: This code was written as part of my master's thesis at the 
 *        Institute for Microsensors, -actuators and -systems (IMSAS) 
 *        at the University of Bremen.
 */

#include "CRC8.h"

/**************************************************************************/
/*!
    @brief Calculate the CRC for two bytes
    @param data Data for the CRC calculation
    @return CRC value
*/
/**************************************************************************/
uint8_t getCRC2(uint8_t *data) {
  uint8_t crc = 0x00;
  for (uint8_t i = 0; i < 2; i++) {
    crc = CRC_8_TABLE[(crc ^ data[i])];
  }
  return crc;
}

/**************************************************************************/
/*!
    @brief Calculate the CRC for six bytes
    @param data1 First two bytes for the CRC calculation
    @param data2 Remaining bytes for the CRC calculation
    @return CRC value
*/
/**************************************************************************/
uint8_t getCRC6(uint8_t *data1, uint8_t *data2) {
  uint8_t wholeData[6];
  combineArray(data1, data2, wholeData);

  uint8_t crc = 0x00;
  for (uint8_t i = 0; i < 6; i++) {
    crc = CRC_8_TABLE[(crc ^ wholeData[i])];
  }
  return crc;
}

/**************************************************************************/
/*!
    @brief Check the correctness of theCRC of the received data
    @param stats Received status bytes
    @param data Received data bytes
    @param length Length of the data to be considered
    @param crc Received CRC value
    @return Result of the CRC check => False = CRC is correct; True = CRC is
            not correct
*/
/**************************************************************************/
bool checkCRC(uint8_t *status, uint8_t *data, uint8_t length, uint8_t crc) {
  if (length == 2) {
    uint8_t calculatedCRC = getCRC2(status);
    if (calculatedCRC == crc) {
      return true;
    } else {
      return false;
    }
  } else {
    uint8_t calculatedCRC = getCRC6(status, data);
    if (calculatedCRC == crc) {
      return true;
    } else {
      return false;
    }
  }
}

/**************************************************************************/
/*!
    @brief Combine two array with the length of two and six
    @param array1 First array (intended for status array)
    @param array2 Second array (intended for data array)
    @param array Output array as pointer
*/
/**************************************************************************/
void combineArray(uint8_t *array1, uint8_t *array2, uint8_t *array) {
  for (int i = 0; i < 2; i++) {
    array[i] = array1[i];
  }

  for (int i = 0; i < 4; i++) {
    array[i + 2] = array2[i];
  }
}