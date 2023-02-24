/**
 * @file SHT3X.cpp
 * @license MIT
 */

#include "SHT3x.h"

TemperatureSensor::TemperatureSensor(class SHTSensor& sht) : sht(sht) {}

void TemperatureSensor::read() {
  if (!sht.readSample()) {
    Serial.println("SHT3X read failed.");
    strcpy(_data, "null");
    return;
  }
  snprintf(_data, sizeof(_data), "%0.2lf", sht.getTemperature());
}

const char* TemperatureSensor::data() {
  return _data;
}

const char* TemperatureSensor::AQI() {
  float data = atoi(_data);
  
  data = fabs(data - 22.0); // deviation from 22C

  int aqi;
  if (data <= 5.0)
    aqi = ((50 - 0) / (5.0 - .0) * (data - .0) + 0);
  else if (data <= 7.0)
    aqi = ((100 - 50) / (7.0 - 5.0) * (data - 5.0) + 50);
  else if (data <= 10.0)
    aqi = ((150 - 100) / (10.0 - 7.0) * (data - 7.0) + 100);
  else if (data <= 15.0)
    aqi = ((200 - 150) / (15.0 - 10.0) * (data - 10.0) + 150);
  else if (data <= 20.0)
    aqi = ((300 - 200) / (20.0 - 15.0) * (data - 15.0) + 200);
  else if (data <= 25.0)
    aqi = ((500 - 300) / (25.0 - 20.0) * (data - 20.0) + 300);
  else
    aqi = 500;
  snprintf(_AQI, sizeof(_AQI), "%d", aqi);
  return _AQI;
}

HumiditySensor::HumiditySensor(class SHTSensor& sht) : sht(sht) {}

void HumiditySensor::read() {
  if (!sht.readSample()) {
    Serial.println("SHT3X read failed.");
    strcpy(_data, "null");
    return;
  }
  snprintf(_data, sizeof(_data), "%0.2lf", sht.getHumidity());
}

const char* HumiditySensor::data() {
  return _data;
}

const char* HumiditySensor::AQI() {
  float data = atoi(_data);
  
  data = fabs(data - 50.0); // deviation from 50%

  int aqi;
  if (data <= 10.0)
    aqi = ((50 - 0) / (10.0 - .0) * (data - .0) + 0);
  else if (data <= 20.0)
    aqi = ((100 - 50) / (20.0 - 10.0) * (data - 10.0) + 50);
  else if (data <= 30.0)
    aqi = ((150 - 100) / (30.0 - 20.0) * (data - 20.0) + 100);
  else if (data <= 50.0)
    aqi = ((200 - 150) / (50.0 - 10.0) * (data - 10.0) + 150);
  else
    aqi = 200;
  snprintf(_AQI, sizeof(_AQI), "%d", aqi);
  return _AQI;
}
