# meteo-rays-detector

Arduino-based lightning detection system using the **AS3935 Franklin Lightning Sensor**.
Detects lightning events in real time and estimates the distance to the storm front.

---

## Features

- Detects cloud-to-ground and intra-cloud lightning
- Estimates storm distance (~1–40 km)
- Reports lightning energy (relative intensity)
- Distinguishes between lightning, man-made disturbers, and noise events
- Fully configurable via `config.h` (indoor/outdoor, sensitivity, thresholds)
- Optional LED feedback on lightning detection

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | Arduino Uno / Nano / Mega (5 V logic with level shifter, or 3.3 V board) |
| Lightning sensor | AS3935 Franklin Lightning Sensor module |
| Interface | SPI |

> **Voltage warning:** The AS3935 operates at 3.3 V. If using a 5 V Arduino,
> add a level shifter on the SPI and IRQ lines, or use a 3.3 V board (Arduino
> Pro Mini 3.3 V, MKR series, etc.).

### Wiring

```
AS3935 Pin   Arduino Pin
-----------  -----------
VCC          3.3V
GND          GND
SCK          13 (SCK)
MOSI         11 (MOSI)
MISO         12 (MISO)
CS           10 (configurable via CS_PIN)
IRQ          2  (configurable via IRQ_PIN, must support interrupts)
SI           GND  <- selects SPI mode
```

---

## Project structure

```
meteo_rays_detector/
├── meteo_rays_detector.ino   # Main Arduino sketch
└── config.h                  # User configuration (pins, sensitivity, mode)
```

---

## Configuration (`config.h`)

| Parameter | Default | Description |
|-----------|---------|-------------|
| `CS_PIN` | `10` | SPI chip select pin |
| `IRQ_PIN` | `2` | Interrupt pin from AS3935 |
| `SERIAL_BAUD` | `9600` | Serial monitor baud rate |
| `INDOOR_MODE` | `true` | `true` = indoors, `false` = outdoors |
| `NOISE_FLOOR_LEVEL` | `2` | Noise floor 0-7 (higher = less sensitive) |
| `WATCHDOG_THRESHOLD` | `2` | Spike filter 0-15 (higher = fewer false positives) |
| `MIN_LIGHTNINGS` | `1` | Min events before alert (1, 5, 9, or 16) |
| `MASK_DISTURBERS` | `false` | Suppress disturber interrupts |
| `ENABLE_LED` | *(commented out)* | Uncomment to flash built-in LED on lightning |

---

## Serial output

Open the Serial Monitor at the configured baud rate (`9600` by default).
Example output:

```
=== meteo-rays-detector ===
Initializing AS3935 lightning sensor...
Sensor ready. Listening for lightning events...

[LIGHTNING] Distance: ~18 km  |  Energy: 524288
[DISTURBER] Man-made interference detected.
[NOISE] Noise level too high. Consider adjusting NOISE_FLOOR_LEVEL.
```

---

## Tuning tips

- **Too many `[NOISE]` events** - increase `NOISE_FLOOR_LEVEL` (try 3 or 4).
- **Too many `[DISTURBER]` events** - set `MASK_DISTURBERS true` or increase `WATCHDOG_THRESHOLD`.
- **Missing real lightning** - decrease `NOISE_FLOOR_LEVEL` or `WATCHDOG_THRESHOLD`.
- **Indoors with poor sensitivity** - set `INDOOR_MODE true` (already the default).

---

## Dependencies

No external libraries required. The sketch uses only the built-in **SPI** library.

---

## License

GNU General Public License v3.0 - see [LICENSE](LICENSE).
