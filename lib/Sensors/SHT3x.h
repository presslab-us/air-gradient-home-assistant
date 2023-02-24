/**
 * @file SHT3x.h
 * @license MIT
 */

#ifndef SHT3x_h
#define SHT3x_h

#include "QualitySensor.h"
#include "SHTSensor.h"

class TemperatureSensor : public QualitySensor {
public:
  TemperatureSensor(class SHTSensor& sht);
  void read() override;
  const char* data() override;
  const char* AQI() override;
private:
  class SHTSensor& sht;
  char _data[10];
  char _AQI[10];
};

class HumiditySensor : public QualitySensor {
public:
  HumiditySensor(class SHTSensor& sht);
  void read() override;
  const char* data() override;
  const char* AQI() override;
private:
  class SHTSensor& sht;
  char _data[10];
  char _AQI[10];
};

#endif