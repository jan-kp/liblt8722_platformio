/*
 * File Name: LT8722.cpp
 * Description: This library is used to control the LT8722 on the DC3145A
 *              evaluation board from Analog Devices. By default, FSPI is 
 *              used with pins 10-13 for the communication via SPI. This 
 *              can be changed if required.
 *
 * Notes: This code was written as part of my master's thesis at the 
 *        Institute for Microsensors, -actuators and -systems (IMSAS) 
 *        at the University of Bremen.
 */

#include "LT8722.h"
#include "LT8722SPI.h"

/**************************************************************************/
/*!
    @brief Create the LT8722 object and specify the SPI type (usually FSPI)
    @param spi_bus the SPI type to use
*/
/**************************************************************************/
LT8722::LT8722(uint8_t spi_bus) {
    if(spi_bus == HSPI) {
        spi = new SPIClass(HSPI);
    } else {
        spi = new SPIClass(FSPI);
    }
}

/**************************************************************************/
/*!
    @brief Initialize the SPI interface
    @param miso the SPI MISO pin to use
    @param mosi the SPI MOSI pin to use
    @param sck the SPI clock pin to use
    @param cs the SPI CS pin to use
*/
/**************************************************************************/
void LT8722::begin(uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t cs, uint8_t analogInput) {
    spi->begin(sck, miso, mosi, cs);

    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    _cs = cs;

    _analogInput = analogInput;

    resetRegisters(spi, _cs);
    resetStatusRegister(spi, _cs);
}

/**************************************************************************/
/*!
    @brief Softstart of the LT8722 to prevent large inrush currents
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::softStart() {

    //softstart procedure 
    struct dataSPI dataPacket0 = resetRegisters(spi, _cs);
    struct dataSPI dataPacket1 = resetStatusRegister(spi, _cs);
    struct dataSPI dataPacket2 = setCommandRegister(spi, _cs, COMMAND_REG::ENABLE_REQ, ENABLE);
    struct dataSPI dataPacket3 = setOutputVoltage(spi, _cs, 2.5);
    struct dataSPI dataPacket4 = resetStatusRegister(spi, _cs);
    delay(2);
    struct dataSPI dataPacket5 = rampOutputVoltage(spi, _cs, 2.5, 1.25, 0.01, 20);
    struct dataSPI dataPacket6 = setCommandRegister(spi, _cs, COMMAND_REG::SWEN_REQ, ENABLE);
    struct dataSPI dataPacket7 = resetStatusRegister(spi, _cs);
    delay(2);

    //check for communication errors
    if (dataPacket0.error ||
        dataPacket1.error ||
        dataPacket2.error ||
        dataPacket3.error ||
        dataPacket4.error ||
        dataPacket5.error ||
        dataPacket6.error ||
        dataPacket7.error) {
        return true;
    } else {
        return false;
    }
}

/**************************************************************************/
/*!
    @brief Reset all register
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::reset() {
    struct dataSPI dataPacket0 = resetRegisters(spi, _cs);
    struct dataSPI dataPacket1 = resetStatusRegister(spi, _cs);

    //check for communication errors
    if (dataPacket0.error || dataPacket1.error) {
        return true;
    } else {
        return false;
    }
}

/**************************************************************************/
/*!
    @brief Disable the enable request and switch enable request bit to turn 
           off the output and reset the status register 
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::powerOff() {
    struct dataSPI dataPacket0 = setCommandRegister(spi, _cs, COMMAND_REG::ENABLE_REQ, DISABLE);
    struct dataSPI dataPacket1 = setCommandRegister(spi, _cs, COMMAND_REG::SWEN_REQ, DISABLE);
    struct dataSPI dataPacket2 = resetStatusRegister(spi, _cs);

    //check for communication errors
    if (dataPacket0.error || dataPacket1.error || dataPacket2.error) {
        return true;
    } else {
        return false;
    }
}

/**************************************************************************/
/*!
    @brief Set the output VOltage
    @param voltage Desired output Voltage
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setVoltage(double voltage) {
    double voltageDAC = (voltage / -16) + 1.25; //convert the voltage to fit the specifications of the L8722

    struct dataSPI dataPacket = setOutputVoltage(spi, _cs, voltageDAC);

    //check for communication errors
    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Return the data of the status register, bit [10-0]
    @return Data of the status register
*/
/**************************************************************************/
uint16_t LT8722::getStatus() {
    uint16_t status;

    struct dataSPI dataPacket = readStatus(spi, _cs);

    status = (static_cast<uint16_t>(dataPacket.status[0]) << 8) | dataPacket.status[1]; //convert status byte array to uint16_t 

    return status;
}

/**************************************************************************/
/*!
    @brief Return the data of the command register, bit [18-0]
    @return Data of the command register
*/
/**************************************************************************/
uint32_t LT8722::getCommand() {
    uint32_t data;

    struct dataSPI dataPacket = readRegister(spi, _cs, 0x00);

    data = (static_cast<uint32_t>(dataPacket.data[0]) << 24) | (static_cast<uint32_t>(dataPacket.data[1]) << 16) | (static_cast<uint16_t>(dataPacket.data[2]) << 8) | dataPacket.data[3]; //convert data byte array to uint32_t 

    return data;
}

/**************************************************************************/
/*!
    @brief Define the maximum positive voltage limit
    @param limit Predefined positive voltage limit
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setPositiveVoltageLimit(VOLTAGE_LIMIT limit) {
    uint8_t limitValue = static_cast<uint8_t>(limit);
    uint8_t data[] = {0x00, 0x00, 0x00, limitValue};

    struct dataSPI dataPacket = writeRegister(spi, _cs, 0x05, data);

    //check for communication errors
    return dataPacket.error;
}


/**************************************************************************/
/*!
    @brief Define the maximum negative voltage limit
    @param limit Predefined negative voltage limit
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setNegativeVoltageLimit(VOLTAGE_LIMIT limit) {
    uint8_t limitValue = static_cast<uint8_t>(limit);
    limitValue = ~limitValue & 0x0F;                    //convert to negative limit
    uint8_t data[] = {0x00, 0x00, 0x00, limitValue};

    struct dataSPI dataPacket = writeRegister(spi, _cs, 0x06, data);

    //check for communication errors
    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Define the maximum positive current limit
    @param limit Positive current limit
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setPositiveCurrentLimit(double limit) {
    uint8_t data[4];
    uint16_t currentLimit = -((limit - 6.8) / 0.01328); //convert the current limit to fit the specifications of the L8722
    
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = currentLimit >> 8;
    data[3] = currentLimit;

    struct dataSPI dataPacket = writeRegister(spi, _cs, 0x03, data);

    //check for communication errors
    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Define the maximum negative current limit
    @param limit Negative voltage limit
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setNegativeCurrentLimit(double limit) {
    uint8_t data[4];
    uint16_t currentLimit = -(-limit / 0.01328); //convert the current limit to fit the specifications of the L8722
    
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = currentLimit >> 8;
    data[3] = currentLimit;

    struct dataSPI dataPacket = writeRegister(spi, _cs, 0x02, data);

    //check for communication errors
    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Set the PWM switch frequency
    @param value Predefined PWM switch frequency
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setPWMFreq(PWM_MHZ value){
    uint8_t freqValue = static_cast<uint8_t>(value);
    struct dataSPI dataPacket = setCommandRegister(spi, _cs, COMMAND_REG::SW_FRQ_SET, freqValue);

    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Set the PWM switch frequency adjustment
    @param value Predefined PWM switch frequency adjustment
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setPWMAdjust(PWM_ADJ value){
    uint8_t adjValue = static_cast<uint8_t>(value);
    struct dataSPI dataPacket = setCommandRegister(spi, _cs, COMMAND_REG::SW_FRQ_ADJ, adjValue);

    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Set the PWM duty cycle
    @param value Predefined PWM duty cycle
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/  
bool LT8722::setPWMDutyCycle(PWM_DUTY value){
    uint8_t dutyValue = static_cast<uint8_t>(value);
    struct dataSPI dataPacket = setCommandRegister(spi, _cs, COMMAND_REG::SYS_DC, dutyValue);

    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Set the VCC LDO regulation (3.4V / 3.1V)
    @param value Predefined VCC LDO regulation
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/  
bool LT8722::setLDOVoltage(LDO_VOLTAGE value){
    uint8_t voltageValue = static_cast<uint8_t>(value);
    struct dataSPI dataPacket = setCommandRegister(spi, _cs, COMMAND_REG::VCC_VREG, voltageValue);

    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Set the typical peak inductor current after BST–SW refresh period
    @param value Predefined peak inductor current
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/  
bool LT8722::setPeakInductor(INDUCTOR_CURRENT value){
    uint8_t currentValue = static_cast<uint8_t>(value);
    struct dataSPI dataPacket = setCommandRegister(spi, _cs, COMMAND_REG::SW_VC_INT, currentValue);

    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Set the linear power stage MOSFET power limit
    @param value Predefined MOSFET power limit
    @return Error (True) if an error accrued during the SPI communication
*/
/**************************************************************************/
bool LT8722::setPowerLimit(POWER_LIMIT value){
    uint8_t powerValue = static_cast<uint8_t>(value);
    struct dataSPI dataPacket = setCommandRegister(spi, _cs, COMMAND_REG::PWR_LIM, powerValue);

    return dataPacket.error;
}

/**************************************************************************/
/*!
    @brief Read the selected value of the analog output pin
    @param value Predefined value to be read
    @return value of the selected analog output
*/
/**************************************************************************/
double LT8722::readAnalogOutput(ANALOG_OUTPUT value){
    uint8_t outputValue = static_cast<uint8_t>(value);
    double output = 0.0;
    double voltage = 0.0;
    double voltage1P25 = 0.0;
    double voltage1P65 = 0.0;

    //read and convert voltage according to the requested value
    switch (value)
    {
    case ANALOG_OUTPUT::VOLTAGE:
        enableAnalogOutput(spi, _cs);
        setAnalogOutput(spi, _cs, outputValue);
        delay(10);
        voltage = analogReadMilliVolts(_analogInput);
        voltage /= 1000;

        setAnalogOutput(spi, _cs, 0x6);
        delay(10);
        voltage1P25 = analogReadMilliVolts(_analogInput);
        voltage1P25 /= 1000;
        disableAnalogOutput(spi, _cs);

        output = (-voltage + voltage1P25) * 16;
        break;
    case ANALOG_OUTPUT::CURRENT:
        enableAnalogOutput(spi, _cs);
        setAnalogOutput(spi, _cs, outputValue);
        delay(10);
        voltage = analogReadMilliVolts(_analogInput);
        voltage /= 1000;

        setAnalogOutput(spi, _cs, 0x7);
        delay(10);
        voltage1P65 = analogReadMilliVolts(_analogInput);
        voltage1P65 /= 1000;
        disableAnalogOutput(spi, _cs);

        output = (-voltage + voltage1P65) * 8;
        break;
    case ANALOG_OUTPUT::TEMPERATURE:
        enableAnalogOutput(spi, _cs);
        setAnalogOutput(spi, _cs, outputValue);
        delay(10);
        voltage = analogReadMilliVolts(_analogInput);
        voltage /= 1000;
        disableAnalogOutput(spi, _cs);

        output = (voltage - 1.421125) / 0.004715;
        break;
    default:
        output = 0.0;
        break;
    }

    return output;
}