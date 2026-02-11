# Architecture Documentation

## System Overview

The Meteo Rays Detector is a distributed IoT system for detecting and tracking meteorological lightning events using the AS3935 lightning sensor.

## Components

### 1. ESP32 Device (Edge)

**Hardware:**
- ESP32 microcontroller
- AS3935 Franklin Lightning Sensor
- GPS module (UART)
- WiFi connectivity

**Software Libraries:**

#### MQTTEvents Library
- Purpose: MQTT client wrapper for publishing events
- Features:
  - Connection management
  - Event publishing
  - Sensor data formatting
  - Auto-reconnection

#### GPSHandler Library
- Purpose: GPS module interface
- Features:
  - NMEA sentence parsing
  - Location data extraction
  - Validity checking
  - Formatted output

#### AS3935Sensor Library
- Purpose: Lightning sensor driver
- Features:
  - I2C communication
  - Interrupt handling
  - Distance calculation
  - Energy measurement
  - Indoor/outdoor configuration

**Main Application Flow:**
1. Initialize WiFi connection
2. Connect to AWS IoT Core via MQTT
3. Initialize GPS and AS3935 sensor
4. Monitor for lightning events
5. Publish events with location data
6. Periodically send location updates

### 2. AWS Cloud Infrastructure

**AWS IoT Core:**
- MQTT broker endpoint
- Device authentication (X.509 certificates)
- Topic-based message routing
- Security policies

**IoT Policy:**
- Client ID pattern: `meteo-rays-*`
- Allowed topics: `meteo-rays/*`
- Permissions: Connect, Publish, Subscribe, Receive

**IoT Topic Rule:**
- SQL: `SELECT * FROM 'meteo-rays/#'`
- Action: Invoke Lambda function
- Processes all messages on meteo-rays topics

**Lambda Function (MQTT Broker):**
- Runtime: Node.js 20.x
- Purpose: Message processing and enrichment
- Functions:
  - Event routing
  - Data validation
  - Severity calculation
  - Timestamp enrichment
  - Future: DynamoDB storage, SNS notifications

## Data Flow

```
┌─────────────┐
│   AS3935    │
│  Lightning  │
│   Sensor    │
└──────┬──────┘
       │ I2C
       │
┌──────▼──────┐     ┌─────────┐
│             │     │   GPS   │
│    ESP32    │◄────┤  Module │
│             │UART └─────────┘
└──────┬──────┘
       │ WiFi/MQTT
       │
┌──────▼──────────┐
│   AWS IoT Core  │
│  MQTT Broker    │
└──────┬──────────┘
       │
┌──────▼──────────┐
│  IoT Topic Rule │
└──────┬──────────┘
       │
┌──────▼──────────┐
│ Lambda Function │
│  (Processing)   │
└──────┬──────────┘
       │
┌──────▼──────────┐
│ Future Services │
│ DynamoDB, SNS,  │
│ etc.            │
└─────────────────┘
```

## Message Formats

### Lightning Event

Topic: `meteo-rays/lightning`

```json
{
  "type": "lightning",
  "distance": 15,
  "energy": 123456,
  "lat": 40.7128,
  "lon": -74.0060,
  "timestamp": 1234567890
}
```

### Location Update

Topic: `meteo-rays/location`

```json
{
  "lat": 40.7128,
  "lon": -74.0060,
  "alt": 10.5,
  "sats": 8,
  "timestamp": 1234567890
}
```

## Security

### Device Authentication
- X.509 certificates for each device
- Private key stored on device
- Certificate attached to IoT Policy

### Communication
- TLS 1.2+ for MQTT connections
- Certificate-based authentication
- Topic-level authorization

### IAM Permissions
- Lambda execution role with minimal permissions
- IoT service principal can invoke Lambda
- Least privilege principle

## Scalability

### Device Side
- Lightweight MQTT protocol
- Efficient message batching possible
- Low power consumption design

### Cloud Side
- Serverless Lambda (auto-scaling)
- IoT Core handles millions of devices
- Pay-per-use pricing model

## Future Enhancements

1. **Data Storage**: DynamoDB for historical data
2. **Notifications**: SNS for real-time alerts
3. **Analytics**: Athena/QuickSight for visualization
4. **Web Dashboard**: React app for monitoring
5. **API Gateway**: REST API for data access
6. **Multiple Devices**: Fleet management
7. **Machine Learning**: Pattern detection

## Development Workflow

### ESP32 Development
1. Edit code in `esp32/` directory
2. Build with PlatformIO
3. Upload to device
4. Monitor serial output

### Infrastructure Updates
1. Edit CDK code in `aws-cdk/`
2. Review changes: `npm run diff`
3. Deploy: `npm run deploy`
4. Test with device

## Monitoring and Debugging

### ESP32
- Serial monitor (115200 baud)
- Debug logs for WiFi, MQTT, sensors
- LED indicators (can be added)

### AWS
- CloudWatch Logs for Lambda
- IoT Core metrics
- X-Ray tracing (can be enabled)

## Cost Estimation

**AWS Services:**
- IoT Core: $1.00 per million messages
- Lambda: Free tier, then $0.20 per 1M requests
- CloudWatch Logs: $0.50 per GB ingested

**Example:** 1 device, 1 message/minute
- ~43,800 messages/month
- Well within free tier
- Estimated cost: < $1/month
