/*
 * File Name: LT8722.h
 * Description: This library is used to control the LT8722 on the DC3145A
 *              evaluation board from Analog Devices. By default, FSPI is 
 *              used with pins 10-13 for the communication via SPI. This 
 *              can be changed if required.
 *
 * Notes: This code was written as part of my master's thesis at the 
 *        Institute for Microsensors, -actuators and -systems (IMSAS) 
 *        at the University of Bremen.
 */

#ifndef LT8722_H
#define LT8722_H

#include <Arduino.h>
#include <SPI.h>

#define VOLTAGE_LIMIT_1_25 0x00
#define VOLTAGE_LIMIT_2_50 0x01
#define VOLTAGE_LIMIT_3_75 0x02
#define VOLTAGE_LIMIT_5_00 0x03
#define VOLTAGE_LIMIT_6_25 0x04
#define VOLTAGE_LIMIT_7_50 0x05
#define VOLTAGE_LIMIT_8_75 0x06
#define VOLTAGE_LIMIT_10_00 0x07
#define VOLTAGE_LIMIT_11_25 0x08
#define VOLTAGE_LIMIT_12_50 0x09
#define VOLTAGE_LIMIT_13_75 0x0A
#define VOLTAGE_LIMIT_15_00 0x0B
#define VOLTAGE_LIMIT_16_25 0x0C
#define VOLTAGE_LIMIT_17_50 0x0D
#define VOLTAGE_LIMIT_18_75 0x0E
#define VOLTAGE_LIMIT_20_00 0x0F

#define PWM_MHZ_0_5 0x0
#define PWM_MHZ_1_0 0x1
#define PWM_MHZ_1_5 0x2
#define PWM_MHZ_2_0 0x3
#define PWM_MHZ_2_5 0x4
#define PWM_MHZ_3_0 0x5

#define PWM_ADJ_0 0x0
#define PWM_ADJ_15 0x1
#define PWM_ADJ_n15 0x2

#define PWM_DUTY_20_80 0x0
#define PWM_DUTY_15_85 0x1
#define PWM_DUTY_10_90 0x2

#define LDO_VOLTAGE_3_4 0x1
#define LDO_VOLTAGE_3_1 0x0

#define INDUCTOR_CURRENT_0_252 0x0
#define INDUCTOR_CURRENT_0_594 0x1
#define INDUCTOR_CURRENT_0_936 0x2
#define INDUCTOR_CURRENT_1_278 0x3
#define INDUCTOR_CURRENT_1_620 0x4
#define INDUCTOR_CURRENT_1_962 0x5
#define INDUCTOR_CURRENT_2_304 0x6
#define INDUCTOR_CURRENT_2_646 0x7

#define POWER_LIMIT_2_0 0x0
#define POWER_LIMIT_NONE 0x5
#define POWER_LIMIT_3_0 0xA
#define POWER_LIMIT_3_5 0xF

#define ANALOG_VOLTAGE_OUTPUT 0x3
#define ANALOG_CURRENT_OUTPUT 0x4
#define ANALOG_TEMPERATURE 0x8

class LT8722 {
public:
    //constructor and begin function
    LT8722(uint8_t spi_bus = FSPI);
    void begin(uint8_t miso = 13, uint8_t mosi = 11, uint8_t sck = 12, uint8_t cs = 10, uint8_t analogInput = 8);

    //important control functions
    bool softStart();
    bool reset();
    bool powerOff();
    bool setVoltage(double voltage);

    //functions for validating the correct functionality
    uint16_t getStatus();
    uint32_t getCommand();

    //functions for voltage and current limits
    bool setPositiveVoltageLimit(uint8_t limit);
    bool setNegativeVoltageLimit(uint8_t limit);
    bool setPositiveCurrentLimit(double limit);
    bool setNegativeCurrentLimit(double limit);

    //additional control functions
    bool setPWMFreq(uint8_t value);
    bool setPWMAdjust(uint8_t value);
    bool setPWMDutyCycle(uint8_t value);
    bool setLDOVoltage(uint8_t value);
    bool setPeakInductor(uint8_t value);
    bool setPowerLimit(uint8_t value);

    //read analog output
    double readAnalogOutput(uint8_t value);

private:
    SPIClass* spi;
    uint8_t _cs;
    uint8_t _analogInput;
};

#endif