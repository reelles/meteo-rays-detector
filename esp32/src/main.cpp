#include <Arduino.h>
#include <WiFi.h>
#include "MQTTEvents.h"
#include "GPSHandler.h"
#include "AS3935Sensor.h"

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT configuration
const char* mqtt_broker = "YOUR_MQTT_BROKER";
const int mqtt_port = 1883;
const char* mqtt_client_id = "meteo-rays-detector";

// Pin definitions
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define AS3935_INT_PIN 4

// Global objects
MQTTEvents* mqttEvents;
GPSHandler* gpsHandler;
AS3935Sensor* as3935Sensor;

// ISR for AS3935 interrupt
void IRAM_ATTR handleAS3935Interrupt() {
    as3935Sensor->handleInterrupt();
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Meteo Rays Detector - Starting...");
    
    // Connect to WiFi
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Initialize MQTT
    mqttEvents = new MQTTEvents(mqtt_broker, mqtt_port, mqtt_client_id);
    mqttEvents->connect();
    
    // Initialize GPS
    gpsHandler = new GPSHandler();
    gpsHandler->begin(GPS_RX_PIN, GPS_TX_PIN);
    
    // Initialize AS3935 Lightning Sensor
    as3935Sensor = new AS3935Sensor();
    if (as3935Sensor->begin(AS3935_INT_PIN)) {
        as3935Sensor->setIndoors(true);
        as3935Sensor->setNoiseFloor(2);
        as3935Sensor->setSpikeRejection(2);
        as3935Sensor->setWatchdogThreshold(2);
        attachInterrupt(digitalPinToInterrupt(AS3935_INT_PIN), handleAS3935Interrupt, RISING);
    }
    
    Serial.println("System ready!");
}

void loop() {
    // Update MQTT connection
    mqttEvents->loop();
    
    // Update GPS data
    gpsHandler->update();
    
    // Check for AS3935 interrupts
    if (as3935Sensor->isInterruptDetected()) {
        as3935Sensor->clearInterrupt();
        delay(2); // Wait for interrupt register to settle
        
        uint8_t intSource = as3935Sensor->getInterruptSource();
        
        if (intSource == AS3935_INT_LIGHTNING) {
            uint8_t distance = as3935Sensor->getLightningDistance();
            uint32_t energy = as3935Sensor->getLightningEnergy();
            
            Serial.print("Lightning detected! Distance: ");
            Serial.print(distance);
            Serial.print(" km, Energy: ");
            Serial.println(energy);
            
            // Prepare MQTT payload
            char payload[128];
            snprintf(payload, sizeof(payload), 
                     "{\"type\":\"lightning\",\"distance\":%d,\"energy\":%lu}", 
                     distance, energy);
            
            // Add GPS location if available
            if (gpsHandler->hasLocation()) {
                snprintf(payload, sizeof(payload), 
                         "{\"type\":\"lightning\",\"distance\":%d,\"energy\":%lu,\"lat\":%.6f,\"lon\":%.6f}", 
                         distance, energy, gpsHandler->getLatitude(), gpsHandler->getLongitude());
            }
            
            // Publish to MQTT
            mqttEvents->publishEvent("meteo-rays/lightning", payload);
        } else if (intSource == AS3935_INT_DISTURBER) {
            Serial.println("Disturber detected");
        } else if (intSource == AS3935_INT_NOISE) {
            Serial.println("Noise level too high");
        }
    }
    
    // Periodically publish GPS location
    static unsigned long lastGPSPublish = 0;
    if (millis() - lastGPSPublish > 60000) { // Every 60 seconds
        lastGPSPublish = millis();
        
        if (gpsHandler->hasLocation()) {
            Serial.println(gpsHandler->getFormattedLocation());
            
            char payload[128];
            snprintf(payload, sizeof(payload), 
                     "{\"lat\":%.6f,\"lon\":%.6f,\"alt\":%.1f,\"sats\":%d}", 
                     gpsHandler->getLatitude(), 
                     gpsHandler->getLongitude(), 
                     gpsHandler->getAltitude(), 
                     gpsHandler->getSatellites());
            
            mqttEvents->publishEvent("meteo-rays/location", payload);
        }
    }
    
    delay(10);
}
