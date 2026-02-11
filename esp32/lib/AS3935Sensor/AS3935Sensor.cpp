#include "AS3935Sensor.h"

AS3935Sensor::AS3935Sensor(uint8_t address) {
    this->i2cAddress = address;
    this->interruptDetected = false;
}

bool AS3935Sensor::begin(int intPin) {
    this->interruptPin = intPin;
    pinMode(interruptPin, INPUT);
    
    Wire.begin();
    
    // Test communication
    uint8_t presetValue = readRegister(AS3935_REG_PRESET);
    if (presetValue == 0xFF || presetValue == 0x00) {
        Serial.println("AS3935 sensor not detected!");
        return false;
    }
    
    Serial.println("AS3935 sensor initialized");
    reset();
    return true;
}

void AS3935Sensor::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t AS3935Sensor::readRegister(uint8_t reg) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    
    Wire.requestFrom(i2cAddress, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

void AS3935Sensor::maskBits(uint8_t reg, uint8_t mask, uint8_t value) {
    uint8_t regValue = readRegister(reg);
    regValue &= ~mask;
    regValue |= (value & mask);
    writeRegister(reg, regValue);
}

void AS3935Sensor::reset() {
    writeRegister(AS3935_REG_PRESET, 0x96);
    delay(2);
    Serial.println("AS3935 reset complete");
}

void AS3935Sensor::setIndoors(bool indoors) {
    // AFE_GB: Indoor (0x12) or Outdoor (0x0E)
    if (indoors) {
        maskBits(AS3935_REG_AFE_GAIN, 0x3E, 0x24); // 10010 = Indoor
    } else {
        maskBits(AS3935_REG_AFE_GAIN, 0x3E, 0x1C); // 01110 = Outdoor
    }
}

void AS3935Sensor::setNoiseFloor(uint8_t level) {
    if (level > 7) level = 7;
    maskBits(AS3935_REG_THRESHOLD, 0x70, level << 4);
}

void AS3935Sensor::setSpikeRejection(uint8_t level) {
    if (level > 15) level = 15;
    maskBits(AS3935_REG_THRESHOLD, 0x0F, level);
}

void AS3935Sensor::setWatchdogThreshold(uint8_t level) {
    if (level > 15) level = 15;
    maskBits(AS3935_REG_LIGHTNING, 0x0F, level);
}

uint8_t AS3935Sensor::getInterruptSource() {
    return readRegister(AS3935_REG_INT) & 0x0F;
}

uint8_t AS3935Sensor::getLightningDistance() {
    uint8_t distance = readRegister(AS3935_REG_DISTANCE) & 0x3F;
    return distance;
}

uint32_t AS3935Sensor::getLightningEnergy() {
    uint32_t energy = 0;
    energy = readRegister(AS3935_REG_ENERGY_MMSB);
    energy = (energy << 8) | readRegister(AS3935_REG_ENERGY_MSB);
    energy = (energy << 8) | readRegister(AS3935_REG_ENERGY_LSB);
    return energy;
}

void AS3935Sensor::clearStatistics() {
    maskBits(AS3935_REG_LIGHTNING, 0x40, 0x40);
    delay(2);
    maskBits(AS3935_REG_LIGHTNING, 0x40, 0x00);
}

void AS3935Sensor::handleInterrupt() {
    interruptDetected = true;
}

bool AS3935Sensor::isInterruptDetected() {
    return interruptDetected;
}

void AS3935Sensor::clearInterrupt() {
    interruptDetected = false;
}
