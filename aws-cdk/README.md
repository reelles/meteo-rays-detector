# AWS CDK - Meteo Rays Detector

This directory contains the AWS CDK infrastructure code for the Meteo Rays Detector IoT platform.

## Architecture

The infrastructure includes:

- **AWS IoT Core**: MQTT broker for device communication
- **IoT Policy**: Security policy for device connections and publish/subscribe permissions
- **Lambda Function**: MQTT message processor and broker
- **IoT Topic Rule**: Routes messages from IoT Core to Lambda

## Components

### IoT Policy

Allows devices with client ID `meteo-rays-*` to:
- Connect to AWS IoT Core
- Publish to topics under `meteo-rays/*`
- Subscribe to topics under `meteo-rays/*`

### Lambda Broker

Processes all messages published to `meteo-rays/#` topics:
- Lightning detection events (`meteo-rays/lightning`)
- GPS location updates (`meteo-rays/location`)

Features:
- Event enrichment (severity calculation, timestamps)
- Extensible for additional processing (DynamoDB, SNS, etc.)

## Prerequisites

- Node.js 18+ and npm
- AWS CLI configured with appropriate credentials
- AWS CDK CLI installed (`npm install -g aws-cdk`)

## Setup

1. Install dependencies:
   ```bash
   cd aws-cdk
   npm install
   ```

2. Bootstrap CDK (first time only):
   ```bash
   cdk bootstrap
   ```

3. Review the CloudFormation template:
   ```bash
   npm run synth
   ```

## Deployment

Deploy the stack:
```bash
npm run deploy
```

This will create:
- IoT Policy
- Lambda function
- IoT Topic Rule
- All necessary IAM roles and permissions

## Outputs

After deployment, you'll get:
- **IoTPolicyName**: Name of the IoT policy to attach to device certificates
- **LambdaFunctionArn**: ARN of the Lambda function
- **IoTEndpoint**: MQTT endpoint for device connections
- **TopicRuleArn**: ARN of the IoT topic rule

## Configuration

### Environment Variables

The Lambda function receives:
- `IOT_ENDPOINT`: The AWS IoT endpoint for the region

### Topics

- `meteo-rays/lightning`: Lightning detection events
- `meteo-rays/location`: GPS location updates

## Development

Build TypeScript:
```bash
npm run build
```

Watch for changes:
```bash
npm run watch
```

Compare deployed stack with current state:
```bash
npm run diff
```

## Device Setup

1. Create an IoT Thing in AWS IoT Core
2. Generate certificates for the device
3. Attach the `MeteoRaysDetectorPolicy` to the certificate
4. Configure the ESP32 with:
   - Certificate
   - Private key
   - IoT endpoint (from stack outputs)

## Cleanup

To remove all resources:
```bash
npm run destroy
```

## License

See LICENSE file in the root directory.
