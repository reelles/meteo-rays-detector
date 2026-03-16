/**
 * config.h — User configuration for meteo-rays-detector
 *
 * Edit these values to match your hardware setup and preferences.
 */

#pragma once

// ---------------------------------------------------------------------------
// Pin definitions
// ---------------------------------------------------------------------------

/** SPI Chip Select pin for the AS3935. */
#define CS_PIN   10

/** Interrupt pin connected to AS3935 IRQ line (must support interrupts). */
#define IRQ_PIN  2

// Uncomment to enable visual LED feedback on lightning detection.
// #define ENABLE_LED
#ifdef ENABLE_LED
  #define LED_PIN  LED_BUILTIN
#endif

// ---------------------------------------------------------------------------
// Serial output
// ---------------------------------------------------------------------------

#define SERIAL_BAUD 9600

// ---------------------------------------------------------------------------
// AS3935 sensor settings
// ---------------------------------------------------------------------------

/**
 * INDOOR_MODE
 *   true  - Indoor use (higher AFE gain, compensates for shielding)
 *   false - Outdoor use
 */
#define INDOOR_MODE true

/**
 * NOISE_FLOOR_LEVEL  (0–7, default: 2)
 *   Higher values reduce noise sensitivity.
 *   Increase if you see frequent INT_NOISE interrupts.
 */
#define NOISE_FLOOR_LEVEL 2

/**
 * WATCHDOG_THRESHOLD  (0–15, default: 2)
 *   Controls how strict the spike filter is.
 *   Increase to reduce false positives from disturbers.
 */
#define WATCHDOG_THRESHOLD 2

/**
 * MIN_LIGHTNINGS
 *   Minimum number of lightning events in the last 15 minutes before
 *   alerting. Valid values: 1, 5, 9, 16.
 */
#define MIN_LIGHTNINGS 1

/**
 * MASK_DISTURBERS
 *   true  - Suppress INT_DISTURBER interrupts (quieter output)
 *   false - Report disturbers (useful for debugging)
 */
#define MASK_DISTURBERS false
