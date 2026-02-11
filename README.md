# Meteo Rays Detector

Monorepo for a meteorological lightning detection system with IoT integration.

## Overview

This project consists of two main components:

1. **ESP32 Firmware** (`/esp32`): Embedded software for ESP32 microcontroller
2. **AWS CDK Infrastructure** (`/aws-cdk`): Cloud infrastructure for IoT data processing

## Architecture

### ESP32 Device

The ESP32-based device includes:
- **AS3935 Lightning Sensor**: Detects lightning strikes and measures distance
- **GPS Module**: Provides geographical location data
- **MQTT Client**: Publishes events to AWS IoT Core

### Cloud Infrastructure

AWS services for data processing:
- **AWS IoT Core**: MQTT broker for device connectivity
- **Lambda Function**: Processes and enriches incoming sensor data
- **IoT Topic Rules**: Routes messages to appropriate handlers

## Project Structure

```
meteo-rays-detector/
├── esp32/                      # ESP32 firmware project
│   ├── lib/                    # Custom libraries
│   │   ├── MQTTEvents/        # MQTT event publishing
│   │   ├── GPSHandler/        # GPS data management
│   │   └── AS3935Sensor/      # Lightning sensor driver
│   ├── src/                    # Application source code
│   ├── platformio.ini          # PlatformIO configuration
│   └── README.md               # ESP32 project documentation
│
└── aws-cdk/                    # AWS CDK infrastructure
    ├── bin/                    # CDK app entry point
    ├── lib/                    # CDK stack definitions
    ├── lambda/                 # Lambda function code
    ├── cdk.json                # CDK configuration
    ├── package.json            # Node.js dependencies
    └── README.md               # AWS CDK documentation
```

## Features

### ESP32 Firmware
- Real-time lightning detection with distance estimation
- GPS location tracking
- MQTT event publishing to AWS IoT
- Configurable sensor parameters
- WiFi connectivity

### Cloud Infrastructure
- Secure IoT device authentication and authorization
- Message routing and processing
- Event enrichment (severity levels, timestamps)
- Scalable serverless architecture

## Getting Started

### ESP32 Development

See [esp32/README.md](esp32/README.md) for:
- Hardware requirements
- Pin configuration
- Build and upload instructions
- Library documentation

### AWS Infrastructure

See [aws-cdk/README.md](aws-cdk/README.md) for:
- Prerequisites
- Deployment instructions
- Configuration options
- Device setup guide

## Data Flow

1. ESP32 detects lightning event via AS3935 sensor
2. Device captures GPS location
3. Event published to AWS IoT Core via MQTT
4. IoT Topic Rule triggers Lambda function
5. Lambda enriches and processes the event
6. Data stored/forwarded to additional services (future enhancement)

## MQTT Topics

- `meteo-rays/lightning`: Lightning detection events with location
- `meteo-rays/location`: Periodic GPS location updates

## License

See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.