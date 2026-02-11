import * as cdk from 'aws-cdk-lib';
import { Construct } from 'constructs';
import * as iot from 'aws-cdk-lib/aws-iot';
import * as lambda from 'aws-cdk-lib/aws-lambda';
import * as iam from 'aws-cdk-lib/aws-iam';
import * as logs from 'aws-cdk-lib/aws-logs';
import * as path from 'path';

export class MeteoRaysDetectorStack extends cdk.Stack {
  constructor(scope: Construct, id: string, props?: cdk.StackProps) {
    super(scope, id, props);

    // IoT Policy for devices
    const iotPolicy = new iot.CfnPolicy(this, 'MeteoRaysIoTPolicy', {
      policyName: 'MeteoRaysDetectorPolicy',
      policyDocument: {
        Version: '2012-10-17',
        Statement: [
          {
            Effect: 'Allow',
            Action: [
              'iot:Connect',
            ],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:client/meteo-rays-*`,
            ],
          },
          {
            Effect: 'Allow',
            Action: [
              'iot:Publish',
            ],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:topic/meteo-rays/*`,
            ],
          },
          {
            Effect: 'Allow',
            Action: [
              'iot:Subscribe',
            ],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:topicfilter/meteo-rays/*`,
            ],
          },
          {
            Effect: 'Allow',
            Action: [
              'iot:Receive',
            ],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:topic/meteo-rays/*`,
            ],
          },
        ],
      },
    });

    // Lambda function for MQTT broker/processor
    const mqttBrokerLambda = new lambda.Function(this, 'MQTTBrokerLambda', {
      runtime: lambda.Runtime.NODEJS_20_X,
      handler: 'index.handler',
      code: lambda.Code.fromAsset(path.join(__dirname, '../lambda')),
      functionName: 'meteo-rays-mqtt-broker',
      description: 'MQTT broker/processor for Meteo Rays Detector',
      timeout: cdk.Duration.seconds(30),
      memorySize: 256,
      logRetention: logs.RetentionDays.ONE_WEEK,
      environment: {
        IOT_ENDPOINT: `${this.account}.iot.${this.region}.amazonaws.com`,
      },
    });

    // Grant Lambda permission to publish to IoT
    mqttBrokerLambda.addToRolePolicy(
      new iam.PolicyStatement({
        effect: iam.Effect.ALLOW,
        actions: [
          'iot:Publish',
          'iot:DescribeEndpoint',
        ],
        resources: ['*'],
      })
    );

    // IoT Topic Rule to trigger Lambda
    const iotTopicRule = new iot.CfnTopicRule(this, 'MeteoRaysTopicRule', {
      topicRulePayload: {
        sql: "SELECT * FROM 'meteo-rays/#'",
        description: 'Forward all meteo-rays messages to Lambda processor',
        actions: [
          {
            lambda: {
              functionArn: mqttBrokerLambda.functionArn,
            },
          },
        ],
        awsIotSqlVersion: '2016-03-23',
      },
      ruleName: 'MeteoRaysMessageProcessor',
    });

    // Grant IoT permission to invoke Lambda
    mqttBrokerLambda.addPermission('IoTInvokePermission', {
      principal: new iam.ServicePrincipal('iot.amazonaws.com'),
      sourceArn: iotTopicRule.attrArn,
    });

    // Outputs
    new cdk.CfnOutput(this, 'IoTPolicyName', {
      value: iotPolicy.policyName!,
      description: 'IoT Policy name for device certificates',
    });

    new cdk.CfnOutput(this, 'LambdaFunctionArn', {
      value: mqttBrokerLambda.functionArn,
      description: 'ARN of the MQTT broker Lambda function',
    });

    new cdk.CfnOutput(this, 'IoTEndpoint', {
      value: `${this.account}.iot.${this.region}.amazonaws.com`,
      description: 'IoT endpoint for MQTT connections',
    });

    new cdk.CfnOutput(this, 'TopicRuleArn', {
      value: iotTopicRule.attrArn,
      description: 'ARN of the IoT Topic Rule',
    });
  }
}
