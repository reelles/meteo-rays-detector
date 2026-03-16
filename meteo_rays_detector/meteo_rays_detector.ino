/**
 * meteo-rays-detector
 *
 * Lightning detection system based on the AS3935 Franklin Lightning Sensor.
 * Detects lightning events and estimates distance to the storm.
 *
 * Hardware:
 *   - Arduino Uno / Nano / Mega
 *   - AS3935 sensor module (SPI interface)
 *
 * Wiring (SPI):
 *   AS3935 VCC  -> Arduino 3.3V
 *   AS3935 GND  -> Arduino GND
 *   AS3935 SCK  -> Arduino pin 13 (SCK)
 *   AS3935 MOSI -> Arduino pin 11 (MOSI)
 *   AS3935 MISO -> Arduino pin 12 (MISO)
 *   AS3935 CS   -> Arduino pin 10 (CS_PIN)
 *   AS3935 IRQ  -> Arduino pin 2  (IRQ_PIN, interrupt)
 *   AS3935 SI   -> GND (selects SPI mode)
 *
 * License: GPL-3.0
 */

#include <SPI.h>
#include "config.h"

// ---------------------------------------------------------------------------
// AS3935 Register addresses
// ---------------------------------------------------------------------------
#define REG_AFE_GB        0x00  // Analog front-end / gain boost
#define REG_THRESHOLD     0x01  // Noise floor / watchdog threshold
#define REG_LIGHTNING_REG 0x02  // Lightning threshold / clear stats
#define REG_INT_MASK_ANT  0x03  // Interrupt mask / antenna tuning
#define REG_ENERGY_LSB    0x04  // Energy of single lightning (LSB)
#define REG_ENERGY_MSB    0x05  // Energy of single lightning (MSB)
#define REG_ENERGY_MMSB   0x06  // Energy of single lightning (MMSB)
#define REG_DISTANCE      0x07  // Distance estimation
#define REG_DISP_FLAGS    0x08  // Display / tune capacitors
#define REG_CALIB_TRCO    0x3A  // Calibration TRCO
#define REG_CALIB_SRCO    0x3B  // Calibration SRCO
#define REG_RESET         0x3C  // Reset register
#define REG_CALIB_RCO     0x3D  // Calibrate RCO

// AS3935 interrupt types
#define INT_NOISE         0x01
#define INT_DISTURBER     0x04
#define INT_LIGHTNING     0x08

// Distance out-of-range value
#define DISTANCE_OUT_OF_RANGE 0x3F

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
volatile bool irqFired = false;

// ---------------------------------------------------------------------------
// Interrupt Service Routine
// ---------------------------------------------------------------------------
void IRAM_ATTR onIRQ() {
  irqFired = true;
}

// ---------------------------------------------------------------------------
// SPI helpers
// ---------------------------------------------------------------------------
static void spiBegin() {
  digitalWrite(CS_PIN, LOW);
}

static void spiEnd() {
  digitalWrite(CS_PIN, HIGH);
}

/**
 * Read a single register from AS3935.
 * The read command sets bit 6 of the address byte.
 */
uint8_t as3935Read(uint8_t reg) {
  spiBegin();
  SPI.transfer((reg & 0x3F) | 0x40);  // read flag
  SPI.transfer(0x00);                  // dummy
  uint8_t val = SPI.transfer(0x00);
  spiEnd();
  return val;
}

/**
 * Write a value to a register, preserving bits outside the mask.
 */
void as3935Write(uint8_t reg, uint8_t mask, uint8_t value) {
  uint8_t current = as3935Read(reg);
  current &= ~mask;
  current |= (value & mask);
  spiBegin();
  SPI.transfer(reg & 0x3F);   // write flag (bit 6 = 0)
  SPI.transfer(0x00);          // dummy
  SPI.transfer(current);
  spiEnd();
}

// ---------------------------------------------------------------------------
// AS3935 high-level functions
// ---------------------------------------------------------------------------

/** Send full reset command. */
void as3935Reset() {
  spiBegin();
  SPI.transfer(REG_RESET & 0x3F);
  SPI.transfer(0x00);
  SPI.transfer(0x96);  // magic reset value
  spiEnd();
  delay(2);
}

/** Calibrate the internal RCO oscillators. */
void as3935CalibrateRCO() {
  spiBegin();
  SPI.transfer(REG_CALIB_RCO & 0x3F);
  SPI.transfer(0x00);
  SPI.transfer(0x96);
  spiEnd();
  delay(2);
}

/**
 * Set the environment (indoors / outdoors).
 * Indoors uses higher gain to compensate for shielding.
 */
void as3935SetIndoor(bool indoor) {
  // Bits [5:1] of REG_AFE_GB  ->  0x12 = indoor, 0x0E = outdoor
  uint8_t val = indoor ? 0x12 : 0x0E;
  as3935Write(REG_AFE_GB, 0x3E, val << 1);
}

/**
 * Set noise floor level (0–7, default 2).
 * Higher = less sensitive to electrical noise but may miss weak events.
 */
void as3935SetNoiseFloor(uint8_t level) {
  as3935Write(REG_THRESHOLD, 0x70, (level & 0x07) << 4);
}

/**
 * Set watchdog threshold (0–15, default 2).
 * Controls sensitivity to signal spikes.
 */
void as3935SetWatchdogThreshold(uint8_t threshold) {
  as3935Write(REG_THRESHOLD, 0x0F, threshold & 0x0F);
}

/**
 * Set minimum number of lightning events required before alerting (1,5,9,16).
 */
void as3935SetMinLightnings(uint8_t minLightnings) {
  uint8_t val;
  switch (minLightnings) {
    case 5:  val = 0x01; break;
    case 9:  val = 0x02; break;
    case 16: val = 0x03; break;
    default: val = 0x00; break;  // 1
  }
  as3935Write(REG_LIGHTNING_REG, 0x30, val << 4);
}

/** Mask disturbers (set true to ignore man-made signals). */
void as3935MaskDisturbers(bool mask) {
  as3935Write(REG_INT_MASK_ANT, 0x20, mask ? 0x20 : 0x00);
}

/** Read distance to storm front in km (63 = out of range). */
uint8_t as3935GetDistance() {
  return as3935Read(REG_DISTANCE) & 0x3F;
}

/** Read the interrupt type (see INT_* constants). */
uint8_t as3935GetInterruptType() {
  return as3935Read(REG_INT_MASK_ANT) & 0x0F;
}

/** Read 20-bit lightning energy (higher = stronger event). */
uint32_t as3935GetEnergy() {
  uint32_t lsb  = as3935Read(REG_ENERGY_LSB);
  uint32_t msb  = as3935Read(REG_ENERGY_MSB);
  uint32_t mmsb = as3935Read(REG_ENERGY_MMSB) & 0x1F;
  return (mmsb << 16) | (msb << 8) | lsb;
}

/** Clear the lightning statistics registers. */
void as3935ClearStats() {
  as3935Write(REG_LIGHTNING_REG, 0x40, 0x40);
  as3935Write(REG_LIGHTNING_REG, 0x40, 0x00);
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) { /* wait for USB serial on Leonardo/Due */ }

  Serial.println(F("=== meteo-rays-detector ==="));
  Serial.println(F("Initializing AS3935 lightning sensor..."));

  // CS pin
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // IRQ pin (active HIGH, triggers on event)
  pinMode(IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), onIRQ, RISING);

  // SPI bus
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);  // ~1 MHz (AS3935 max 2 MHz)
  SPI.setDataMode(SPI_MODE1);            // CPOL=0, CPHA=1
  SPI.setBitOrder(MSBFIRST);

  delay(2);  // AS3935 power-up time

  // Full reset then reconfigure
  as3935Reset();

  as3935SetIndoor(INDOOR_MODE);
  as3935SetNoiseFloor(NOISE_FLOOR_LEVEL);
  as3935SetWatchdogThreshold(WATCHDOG_THRESHOLD);
  as3935SetMinLightnings(MIN_LIGHTNINGS);
  as3935MaskDisturbers(MASK_DISTURBERS);

  as3935CalibrateRCO();

  Serial.println(F("Sensor ready. Listening for lightning events..."));
  Serial.println();
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------
void loop() {
  if (!irqFired) return;

  irqFired = false;

  // The AS3935 datasheet recommends waiting 2 ms after IRQ before reading
  delay(2);

  uint8_t intType = as3935GetInterruptType();

  switch (intType) {
    case INT_LIGHTNING: {
      uint8_t  dist   = as3935GetDistance();
      uint32_t energy = as3935GetEnergy();

      Serial.print(F("[LIGHTNING] "));
      if (dist == DISTANCE_OUT_OF_RANGE) {
        Serial.print(F("Distance: out of range"));
      } else {
        Serial.print(F("Distance: ~"));
        Serial.print(dist);
        Serial.print(F(" km"));
      }
      Serial.print(F("  |  Energy: "));
      Serial.println(energy);

#ifdef ENABLE_LED
      flashLED(3, 100);
#endif
      break;
    }

    case INT_DISTURBER:
      Serial.println(F("[DISTURBER] Man-made interference detected."));
      break;

    case INT_NOISE:
      Serial.println(F("[NOISE] Noise level too high. Consider adjusting NOISE_FLOOR_LEVEL."));
      break;

    default:
      Serial.print(F("[UNKNOWN] Interrupt type: 0x"));
      Serial.println(intType, HEX);
      break;
  }
}

// ---------------------------------------------------------------------------
// Optional LED feedback
// ---------------------------------------------------------------------------
#ifdef ENABLE_LED
void flashLED(uint8_t times, uint16_t intervalMs) {
  for (uint8_t i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(intervalMs);
    digitalWrite(LED_PIN, LOW);
    delay(intervalMs);
  }
}
#endif
