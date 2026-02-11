#ifndef AS3935_SENSOR_H
#define AS3935_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

// AS3935 I2C addresses
#define AS3935_ADDR_DEFAULT 0x03
#define AS3935_ADDR_ALT1    0x02
#define AS3935_ADDR_ALT2    0x01
#define AS3935_ADDR_ALT3    0x00

// Register addresses
#define AS3935_REG_AFE_GAIN     0x00
#define AS3935_REG_THRESHOLD    0x01
#define AS3935_REG_LIGHTNING    0x02
#define AS3935_REG_INT          0x03
#define AS3935_REG_ENERGY_LSB   0x04
#define AS3935_REG_ENERGY_MSB   0x05
#define AS3935_REG_ENERGY_MMSB  0x06
#define AS3935_REG_DISTANCE     0x07
#define AS3935_REG_FREQ_DISP    0x08
#define AS3935_REG_CALIB        0x3A
#define AS3935_REG_PRESET       0x3C

// Interrupt types
#define AS3935_INT_NONE         0x00
#define AS3935_INT_NOISE        0x01
#define AS3935_INT_DISTURBER    0x04
#define AS3935_INT_LIGHTNING    0x08

class AS3935Sensor {
private:
    uint8_t i2cAddress;
    int interruptPin;
    volatile bool interruptDetected;
    
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void maskBits(uint8_t reg, uint8_t mask, uint8_t value);
    
public:
    AS3935Sensor(uint8_t address = AS3935_ADDR_DEFAULT);
    bool begin(int intPin);
    void reset();
    void setIndoors(bool indoors);
    void setNoiseFloor(uint8_t level);
    void setSpikeRejection(uint8_t level);
    void setWatchdogThreshold(uint8_t level);
    uint8_t getInterruptSource();
    uint8_t getLightningDistance();
    uint32_t getLightningEnergy();
    void clearStatistics();
    void handleInterrupt();
    bool isInterruptDetected();
    void clearInterrupt();
};

#endif // AS3935_SENSOR_H
