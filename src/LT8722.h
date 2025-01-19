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

enum class VOLTAGE_LIMIT : uint8_t{
    LIMIT_1_25  = 0x00,
    LIMIT_2_50  = 0x01,
    LIMIT_3_75  = 0x02,
    LIMIT_5_00  = 0x03,
    LIMIT_6_25  = 0x04,
    LIMIT_7_50  = 0x05,
    LIMIT_8_75  = 0x06,
    LIMIT_10_00 = 0x07,
    LIMIT_11_25 = 0x08,
    LIMIT_12_50 = 0x09,
    LIMIT_13_75 = 0x0A,
    LIMIT_15_00 = 0x0B,
    LIMIT_16_25 = 0x0C,
    LIMIT_17_50 = 0x0D,
    LIMIT_18_75 = 0x0E,
    LIMIT_20_00 = 0x0F
};

enum class PWM_MHZ : uint8_t{
    MHZ_0_5 = 0x00,
    MHZ_1_0 = 0x01,
    MHZ_1_5 = 0x02,
    MHZ_2_0 = 0x03,
    MHZ_2_5 = 0x04,
    MHZ_3_0 = 0x05
};

enum class PWM_ADJ : uint8_t{
    ADJ_0   = 0x00,
    ADJ_15  = 0x01,
    ADJ_n15 = 0x02
};

enum class PWM_DUTY : uint8_t{
    DUTY_20_80 = 0x00,
    DUTY_15_85 = 0x01,
    DUTY_10_90 = 0x02
};

enum class LDO_VOLTAGE : uint8_t{
    VOLTAGE_3_4 = 0x01,
    VOLTAGE_3_1 = 0x00
};

enum class INDUCTOR_CURRENT : uint8_t{
    CURRENT_0_252 = 0x00,
    CURRENT_0_594 = 0x01,
    CURRENT_0_936 = 0x02,
    CURRENT_1_278 = 0x03,
    CURRENT_1_620 = 0x04,
    CURRENT_1_962 = 0x05,
    CURRENT_2_304 = 0x06,
    CURRENT_2_646 = 0x07
};

enum class POWER_LIMIT : uint8_t{
    LIMIT_2_0  = 0x00,
    LIMIT_NONE = 0x05,
    LIMIT_3_0  = 0x0A,
    LIMIT_3_5  = 0x0F
};

enum class ANALOG_OUTPUT : uint8_t{
    VOLTAGE     = 0x03,
    CURRENT     = 0x04,
    TEMPERATURE = 0x08
};

class LT8722 {
public:
    //constructor and begin function
    LT8722(uint8_t sspi_bus = FSPI);
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
    bool setPositiveVoltageLimit(VOLTAGE_LIMIT limit);
    bool setNegativeVoltageLimit(VOLTAGE_LIMIT limit);
    bool setPositiveCurrentLimit(double limit);
    bool setNegativeCurrentLimit(double limit);

    //additional control functions
    bool setPWMFreq(PWM_MHZ value);
    bool setPWMAdjust(PWM_ADJ value);
    bool setPWMDutyCycle(PWM_DUTY value);
    bool setLDOVoltage(LDO_VOLTAGE value);
    bool setPeakInductor(INDUCTOR_CURRENT value);
    bool setPowerLimit(POWER_LIMIT value);

    //read analog output
    double readAnalogOutput(ANALOG_OUTPUT value);

private:
    SPIClass* spi;
    uint8_t _cs;
    uint8_t _analogInput;
};

#endif