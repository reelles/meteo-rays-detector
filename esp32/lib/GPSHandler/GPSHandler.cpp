#include "GPSHandler.h"
#include <time.h>

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

bool GPSHandler::hasTime() {
    return gps.time.isValid() && gps.date.isValid();
}

unsigned long GPSHandler::getTimestamp() {
    if (!hasTime()) {
        return 0;
    }
    
    // Return Unix timestamp (seconds since 1970-01-01) from GPS UTC time
    // GPS provides UTC time, so we need to convert without timezone adjustment
    struct tm timeinfo;
    timeinfo.tm_year = gps.date.year() - 1900;
    timeinfo.tm_mon = gps.date.month() - 1;
    timeinfo.tm_mday = gps.date.day();
    timeinfo.tm_hour = gps.time.hour();
    timeinfo.tm_min = gps.time.minute();
    timeinfo.tm_sec = gps.time.second();
    timeinfo.tm_isdst = 0;
    
    // Set timezone to UTC to prevent local timezone offset
    setenv("TZ", "UTC", 1);
    tzset();
    time_t timestamp = mktime(&timeinfo);
    
    return (unsigned long)timestamp;
}

void GPSHandler::getDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second) {
    year = gps.date.year();
    month = gps.date.month();
    day = gps.date.day();
    hour = gps.time.hour();
    minute = gps.time.minute();
    second = gps.time.second();
}
