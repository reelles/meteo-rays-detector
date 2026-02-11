#include "GPSHandler.h"

GPSHandler::GPSHandler(HardwareSerial* serial) {
    this->gpsSerial = serial;
    this->hasValidLocation = false;
}

void GPSHandler::begin(int rxPin, int txPin, unsigned long baud) {
    gpsSerial->begin(baud, SERIAL_8N1, rxPin, txPin);
    Serial.println("GPS initialized");
}

void GPSHandler::update() {
    while (gpsSerial->available() > 0) {
        char c = gpsSerial->read();
        if (gps.encode(c)) {
            if (gps.location.isValid()) {
                hasValidLocation = true;
            }
        }
    }
}

bool GPSHandler::hasLocation() {
    return hasValidLocation && gps.location.isValid();
}

double GPSHandler::getLatitude() {
    return gps.location.lat();
}

double GPSHandler::getLongitude() {
    return gps.location.lng();
}

double GPSHandler::getAltitude() {
    return gps.altitude.meters();
}

int GPSHandler::getSatellites() {
    return gps.satellites.value();
}

String GPSHandler::getFormattedLocation() {
    if (!hasLocation()) {
        return "No GPS fix";
    }
    
    String location = "Lat: ";
    location += String(getLatitude(), 6);
    location += ", Lon: ";
    location += String(getLongitude(), 6);
    location += ", Alt: ";
    location += String(getAltitude(), 1);
    location += "m, Sats: ";
    location += String(getSatellites());
    
    return location;
}

bool GPSHandler::isValid() {
    return gps.location.isValid();
}
