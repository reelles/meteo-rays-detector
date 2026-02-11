#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

class GPSHandler {
private:
    TinyGPSPlus gps;
    HardwareSerial* gpsSerial;
    bool hasValidLocation;
    
public:
    GPSHandler(HardwareSerial* serial = &Serial1);
    void begin(int rxPin, int txPin, unsigned long baud = 9600);
    void update();
    bool hasLocation();
    double getLatitude();
    double getLongitude();
    double getAltitude();
    int getSatellites();
    String getFormattedLocation();
    bool isValid();
    bool hasTime();
    unsigned long getTimestamp();
    void getDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second);
};

#endif // GPS_HANDLER_H
