#include "MQTTEvents.h"

WiFiClient espClient;

MQTTEvents::MQTTEvents(const char* broker, int port, const char* clientId) {
    this->broker = broker;
    this->port = port;
    this->clientId = clientId;
    this->client = new PubSubClient(espClient);
    this->client->setServer(broker, port);
}

bool MQTTEvents::connect() {
    if (client->connected()) {
        return true;
    }
    
    Serial.print("Connecting to MQTT broker: ");
    Serial.println(broker);
    
    if (client->connect(clientId)) {
        Serial.println("Connected to MQTT broker");
        return true;
    } else {
        Serial.print("Failed to connect to MQTT broker, rc=");
        Serial.println(client->state());
        return false;
    }
}

bool MQTTEvents::publishEvent(const char* topic, const char* payload) {
    if (!client->connected()) {
        if (!connect()) {
            return false;
        }
    }
    
    Serial.print("Publishing to topic: ");
    Serial.println(topic);
    
    return client->publish(topic, payload);
}

bool MQTTEvents::publishSensorData(const char* topic, float value) {
    char payload[32];
    snprintf(payload, sizeof(payload), "%.2f", value);
    return publishEvent(topic, payload);
}

void MQTTEvents::loop() {
    if (!client->connected()) {
        connect();
    }
    client->loop();
}

bool MQTTEvents::isConnected() {
    return client->connected();
}

void MQTTEvents::setCallback(MQTT_CALLBACK_SIGNATURE) {
    client->setCallback(callback);
}

bool MQTTEvents::subscribe(const char* topic) {
    if (!client->connected()) {
        if (!connect()) {
            return false;
        }
    }
    return client->subscribe(topic);
}
