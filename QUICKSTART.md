# Quick Start Guide

This guide will help you get started with the Meteo Rays Detector system.

## Prerequisites

### For ESP32 Development
- [Visual Studio Code](https://code.visualstudio.com/) or any text editor
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) extension
- ESP32 development board
- AS3935 lightning sensor module
- GPS module (UART/Serial compatible)
- WiFi network access

### For AWS Infrastructure
- [Node.js](https://nodejs.org/) 18 or higher
- [AWS CLI](https://aws.amazon.com/cli/) configured with credentials
- [AWS CDK CLI](https://docs.aws.amazon.com/cdk/latest/guide/cli.html): `npm install -g aws-cdk`
- Active AWS account

## Step 1: Deploy AWS Infrastructure

1. Navigate to the CDK project:
   ```bash
   cd aws-cdk
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Bootstrap CDK (first time only):
   ```bash
   cdk bootstrap
   ```

4. Deploy the stack:
   ```bash
   npm run deploy
   ```

5. Note the outputs, especially the IoT endpoint:
   ```
   Outputs:
   MeteoRaysDetectorStack.IoTEndpoint = xxxxx.iot.us-east-1.amazonaws.com
   MeteoRaysDetectorStack.IoTPolicyName = MeteoRaysDetectorPolicy
   ```

## Step 2: Create IoT Device Credentials

1. Create an IoT Thing in AWS Console or CLI:
   ```bash
   aws iot create-thing --thing-name meteo-rays-device-001
   ```

2. Create and download certificates:
   ```bash
   aws iot create-keys-and-certificate \
     --set-as-active \
     --certificate-pem-outfile certificate.pem.crt \
     --public-key-outfile public.pem.key \
     --private-key-outfile private.pem.key
   ```

3. Note the certificate ARN from the output

4. Attach the policy to the certificate:
   ```bash
   aws iot attach-policy \
     --policy-name MeteoRaysDetectorPolicy \
     --target <certificate-arn>
   ```

5. Attach the certificate to the thing:
   ```bash
   aws iot attach-thing-principal \
     --thing-name meteo-rays-device-001 \
     --principal <certificate-arn>
   ```

## Step 3: Configure ESP32 Firmware

1. Navigate to the ESP32 project:
   ```bash
   cd ../esp32
   ```

2. Edit `src/main.cpp` and update the configuration:
   ```cpp
   // WiFi credentials
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   
   // MQTT configuration
   const char* mqtt_broker = "xxxxx.iot.us-east-1.amazonaws.com";
   const int mqtt_port = 8883;  // Use 8883 for secure connection
   const char* mqtt_client_id = "meteo-rays-device-001";
   ```

3. Add certificate files to your ESP32 project (create a certs folder):
   ```
   esp32/
   ├── certs/
   │   ├── certificate.pem.crt
   │   ├── private.pem.key
   │   └── AmazonRootCA1.pem
   ```
   
   Download Amazon Root CA:
   ```bash
   curl -o esp32/certs/AmazonRootCA1.pem https://www.amazontrust.com/repository/AmazonRootCA1.pem
   ```

## Step 4: Hardware Setup

1. **Connect AS3935 Sensor to ESP32:**
   - VCC → 3.3V
   - GND → GND
   - SDA → GPIO 21 (default I2C)
   - SCL → GPIO 22 (default I2C)
   - IRQ → GPIO 4

2. **Connect GPS Module to ESP32:**
   - VCC → 3.3V or 5V (check your module)
   - GND → GND
   - TX → GPIO 16 (ESP32 RX)
   - RX → GPIO 17 (ESP32 TX)

3. **Power the ESP32:**
   - USB cable or external power supply
   - Ensure stable 3.3V for sensors

## Step 5: Build and Upload Firmware

1. Build the project:
   ```bash
   pio run
   ```

2. Upload to ESP32:
   ```bash
   pio run --target upload
   ```

3. Monitor serial output:
   ```bash
   pio device monitor
   ```

   You should see:
   ```
   Meteo Rays Detector - Starting...
   Connecting to WiFi: YourSSID
   WiFi connected
   IP address: 192.168.1.xxx
   Connected to MQTT broker
   GPS initialized
   AS3935 sensor initialized
   System ready!
   ```

## Step 6: Verify System Operation

1. **Check Lambda Logs:**
   ```bash
   aws logs tail /aws/lambda/meteo-rays-mqtt-broker --follow
   ```

2. **Monitor IoT Core Messages (from AWS Console):**
   - Navigate to AWS IoT Core → Test
   - Subscribe to topic: `meteo-rays/#`
   - Watch for incoming messages

3. **Test Lightning Detection:**
   - Trigger the AS3935 sensor (nearby lightning or manual interrupt)
   - Observe serial output on ESP32
   - Verify message appears in Lambda logs

4. **Check GPS Location:**
   - Wait for GPS to acquire satellites (may take a few minutes outdoors)
   - Check serial output for location data
   - Verify location messages published every 60 seconds

## Troubleshooting

### ESP32 Won't Connect to WiFi
- Verify SSID and password
- Check WiFi signal strength
- Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)

### MQTT Connection Failed
- Verify IoT endpoint is correct
- Check certificates are properly installed
- Ensure certificate is activated and policy is attached
- Verify device clock is accurate (required for TLS)

### AS3935 Not Detected
- Check I2C connections (SDA, SCL)
- Verify sensor address (default 0x03)
- Use I2C scanner to detect device

### GPS No Fix
- Ensure GPS antenna has clear view of sky
- Move outdoors for better satellite reception
- Wait several minutes for initial fix
- Check baud rate (default 9600)

### Lambda Not Receiving Messages
- Check IoT Topic Rule is active
- Verify Lambda has permission to be invoked by IoT
- Check CloudWatch Logs for errors

## Next Steps

1. **Customize Event Processing:**
   - Edit `aws-cdk/lambda/index.ts`
   - Add DynamoDB storage
   - Implement SNS notifications
   - Create custom analytics

2. **Add More Devices:**
   - Repeat Steps 2-5 for each device
   - Use unique client IDs
   - Track multiple sensors

3. **Build a Dashboard:**
   - Create web interface to visualize data
   - Use AWS Amplify or custom React app
   - Display real-time lightning events on map

4. **Enhance Security:**
   - Implement device fleet management
   - Add OTA (Over-The-Air) updates
   - Enable CloudWatch alarms

## Support

For issues or questions:
- Check the [ARCHITECTURE.md](ARCHITECTURE.md) for system details
- Review component READMEs in `esp32/` and `aws-cdk/`
- Check AWS IoT Core documentation
- Review PlatformIO documentation

## License

See [LICENSE](LICENSE) for details.
