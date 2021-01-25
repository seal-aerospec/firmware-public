#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include <Arduino.h>
#include <Wire.h> //Needed for I2C to GPS
#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
#include <MicroNMEA.h> //http://librarymanager/All#MicroNMEA

// Sets up I2C connection for GPS sensor
// Returns true if connected to GPS properly
bool setUpGpsSensor();

// Call before gpsDataValid()
// Lets GPS process incoming data
// Returns false if no new data
bool processIncomingData();

// Checks if data is valid
// Returns true if so
bool gpsDataValid();

// Returns latitude
double getLatitude();

// Returns longitude
double getLongitude();

// Return altitude in m
double getAltitude();


void printTime();

#endif
