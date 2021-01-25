#ifndef __tmpHumiditySensor_h__
#define __tmpHumiditySensor_h__

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

namespace consts{
  constexpr int TMP_HUM_SDA {21};
  constexpr int TMP_HUM_SCL {22};
  constexpr int TMP_HUM_I2C_FREQ {9768};
}

void setUpEnvSensor();

float readTemperature();

float readHumidity();

#endif
