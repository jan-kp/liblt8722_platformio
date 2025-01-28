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

    /**************************************************************************/
    /*!
        @brief Create the LT8722 object and specify the SPI type (usually FSPI)
        @param spi_bus the SPI type to use
    */
    /**************************************************************************/
    LT8722(uint8_t spi_bus = FSPI);

    /**************************************************************************/
    /*!
        @brief Initialize the SPI interface
        @param miso the SPI MISO pin to use
        @param mosi the SPI MOSI pin to use
        @param sck the SPI clock pin to use
        @param cs the SPI CS pin to use
    */
    /**************************************************************************/
    void begin(uint8_t miso = 13, uint8_t mosi = 11, uint8_t sck = 12, uint8_t cs = 10, uint8_t analogInput = 8);

    //important control functions

    /**************************************************************************/
    /*!
        @brief Softstart of the LT8722 to prevent large inrush currents
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool softStart();

    /**************************************************************************/
    /*!
        @brief Reset all register
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool reset();

    /**************************************************************************/
    /*!
        @brief Disable the enable request and switch enable request bit to turn 
            off the output and reset the status register 
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool powerOff();

    /**************************************************************************/
    /*!
        @brief Set the output VOltage
        @param voltage Desired output Voltage
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setVoltage(double voltage);

    //functions for validating the correct functionality

    /**************************************************************************/
    /*!
        @brief Return the data of the status register, bit [10-0]
        @return Data of the status register
    */
    /**************************************************************************/
    uint16_t getStatus();

    /**************************************************************************/
    /*!
        @brief Return the data of the command register, bit [18-0]
        @return Data of the command register
    */
    /**************************************************************************/
    uint32_t getCommand();

    //functions for voltage and current limits

    /**************************************************************************/
    /*!
        @brief Define the maximum positive voltage limit
        @param limit Predefined positive voltage limit
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setPositiveVoltageLimit(VOLTAGE_LIMIT limit);

    /**************************************************************************/
    /*!
        @brief Define the maximum negative voltage limit
        @param limit Predefined negative voltage limit
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setNegativeVoltageLimit(VOLTAGE_LIMIT limit);

    /**************************************************************************/
    /*!
        @brief Define the maximum positive current limit
        @param limit Positive current limit
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setPositiveCurrentLimit(double limit);

    /**************************************************************************/
    /*!
        @brief Define the maximum negative current limit
        @param limit Negative voltage limit
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setNegativeCurrentLimit(double limit);

    //additional control functions

    /**************************************************************************/
    /*!
        @brief Set the PWM switch frequency
        @param value Predefined PWM switch frequency
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setPWMFreq(PWM_MHZ value);

    /**************************************************************************/
    /*!
        @brief Set the PWM switch frequency adjustment
        @param value Predefined PWM switch frequency adjustment
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setPWMAdjust(PWM_ADJ value);

    /**************************************************************************/
    /*!
        @brief Set the PWM duty cycle
        @param value Predefined PWM duty cycle
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/  
    bool setPWMDutyCycle(PWM_DUTY value);

    /**************************************************************************/
    /*!
        @brief Set the VCC LDO regulation (3.4V / 3.1V)
        @param value Predefined VCC LDO regulation
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/  
    bool setLDOVoltage(LDO_VOLTAGE value);

    /**************************************************************************/
    /*!
        @brief Set the typical peak inductor current after BST–SW refresh period
        @param value Predefined peak inductor current
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/  
    bool setPeakInductor(INDUCTOR_CURRENT value);

    /**************************************************************************/
    /*!
        @brief Set the linear power stage MOSFET power limit
        @param value Predefined MOSFET power limit
        @return Error (True) if an error accrued during the SPI communication
    */
    /**************************************************************************/
    bool setPowerLimit(POWER_LIMIT value);

    //read analog output

    /**************************************************************************/
    /*!
        @brief Read the selected value of the analog output pin
        @param value Predefined value to be read
        @return value of the selected analog output
    */
    /**************************************************************************/
    double readAnalogOutput(ANALOG_OUTPUT value);

private:
    SPIClass* spi;
    uint8_t _cs;
    uint8_t _analogInput;
};

#endif