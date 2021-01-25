#include "gpsSensor.h"

// SDA -> 21
// SCL ->22
void setup()
{
  Serial.begin(115200);
  setUpGpsSensor();
}

void loop()
{
  processIncomingData(); //See if new data is available. Process bytes as they come in.

  if(gpsDataValid())
  {
    double latitude = getLatitude();
    double longitude = getLongitude();
    Serial.print("Latitude (deg): ");
    Serial.println(latitude, 6);  
    Serial.print("Longitude (deg): ");
    Serial.println(longitude, 6);
  }  
  delay(1000); //Don't pound too hard on the I2C bus
}
