# ESP32 Meteo Rays Detector

This is the firmware for the ESP32-based meteorological lightning detector device.

## Features

- **MQTT Events**: Library for sending events and sensor data via MQTT
- **GPS Handler**: Library for managing GPS module and location tracking
- **AS3935 Sensor**: Library for the AS3935 lightning sensor

## Hardware Requirements

- ESP32 Development Board
- AS3935 Lightning Sensor
- GPS Module (UART compatible)
- WiFi connection

## Pin Configuration

- GPS RX: GPIO 16
- GPS TX: GPIO 17
- AS3935 Interrupt: GPIO 4
- I2C SDA: GPIO 21 (default)
- I2C SCL: GPIO 22 (default)

## Setup

1. Install [PlatformIO](https://platformio.org/)
2. Configure your WiFi credentials in `src/main.cpp`
3. Configure your MQTT broker address in `src/main.cpp`
4. Build and upload:
   ```bash
   cd esp32
   pio run --target upload
   ```

## Libraries

### MQTTEvents

Handles MQTT communication for publishing events and sensor data.

```cpp
MQTTEvents* mqtt = new MQTTEvents("broker.example.com", 1883, "client-id");
mqtt->connect();
mqtt->publishEvent("topic", "{\"data\":\"value\"}");
```

### GPSHandler

Manages GPS module and provides location data.

```cpp
GPSHandler* gps = new GPSHandler();
gps->begin(RX_PIN, TX_PIN);
gps->update();
if (gps->hasLocation()) {
    double lat = gps->getLatitude();
    double lon = gps->getLongitude();
}
```

### AS3935Sensor

Controls the AS3935 lightning sensor.

```cpp
AS3935Sensor* sensor = new AS3935Sensor();
sensor->begin(INT_PIN);
sensor->setIndoors(true);
uint8_t distance = sensor->getLightningDistance();
```

## MQTT Topics

- `meteo-rays/lightning`: Lightning detection events
- `meteo-rays/location`: GPS location updates

## License

See LICENSE file in the root directory.
