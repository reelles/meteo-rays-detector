#ifndef MQTT_EVENTS_H
#define MQTT_EVENTS_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTEvents {
private:
    PubSubClient* client;
    const char* broker;
    int port;
    const char* clientId;
    
public:
    MQTTEvents(const char* broker, int port, const char* clientId);
    bool connect();
    bool publishEvent(const char* topic, const char* payload);
    bool publishSensorData(const char* topic, float value);
    void loop();
    bool isConnected();
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    bool subscribe(const char* topic);
};

#endif // MQTT_EVENTS_H
