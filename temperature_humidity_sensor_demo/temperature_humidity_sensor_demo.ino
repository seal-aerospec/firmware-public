#include "tmpHumiditySensor.h"

void setup(){
  Serial.begin(9600);

  setUpEnvSensor();
}

void loop(){
  Serial.print("Temp = ");
  Serial.println(readTemperature());
  Serial.print("Humidity = ");
  Serial.println(readHumidity());
  delay(1000);
}
