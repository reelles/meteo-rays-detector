#!/usr/bin/env node
import 'source-map-support/register';
import * as cdk from 'aws-cdk-lib';
import { MeteoRaysDetectorStack } from '../lib/meteo-rays-detector-stack';

const app = new cdk.App();

new MeteoRaysDetectorStack(app, 'MeteoRaysDetectorStack', {
  env: {
    account: process.env.CDK_DEFAULT_ACCOUNT,
    region: process.env.CDK_DEFAULT_REGION,
  },
  description: 'Infrastructure for Meteo Rays Detector - IoT and Lambda Broker',
});

app.synth();
