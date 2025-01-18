/*
 * File Name: Basic_Voltage_Control.cpp
 * Description: The following code is an example for the LT8722 library. This 
 *              example illustrates the correct use of the library by setting
 *              the output voltage of the LT8722 alternately to 2V and 4V.
 * 
 * Revision History:
 * Date: 2025-01-17 Author: Jan kleine Piening Comments: Initial version created
 *
 * Author: Jan kleine Piening Start Date: 2025-01-17
 *
 * Notes: This code was written as part of my master's thesis at the 
 *        Institute for Microsensors, -actuators and -systems (IMSAS) 
 *        at the University of Bremen.
 */

#include <Arduino.h>
#include <LT8722.h>

LT8722 peltierDriver;                                           //create a LT8722 object with FSPI

bool error;                                                     //error variable for the SPI communication

void setup() {
  Serial.begin(115200);
  delay(5000);

  peltierDriver.begin();                                        //initialize the SPI interface with the standard pins
  peltierDriver.softStart();                                    //softstart of the LT8722 (resets all registers)
  peltierDriver.setPositiveVoltageLimit(VOLTAGE_LIMIT_5_00);    //set the positive voltage limit to 5V
  peltierDriver.setNegativeVoltageLimit(VOLTAGE_LIMIT_5_00);    //set the negative voltage limit to -5V
  peltierDriver.setPositiveCurrentLimit(4.5);                   //set the positive current limit to 4.5A
  peltierDriver.setNegativeCurrentLimit(4.5);                   //set the negative current limit to -4.5A
}

void loop() {
  error = peltierDriver.setVoltage(2);                          //set the output voltage to 2V
  Serial.println("Error");
  Serial.println(error);                                        //print the error status of the SPI communication
  delay(5000);

  error = peltierDriver.setVoltage(4);                          //set the output voltage to 2V
  Serial.println("Error");
  Serial.println(error);                                        //print the error status of the SPI communication
  delay(5000);
}