/**
 * @file QualitySensor.h
 * @license MIT
 */

#ifndef QualitySensor_h
#define QualitySensor_h

#include <Arduino.h>

struct SENSOR_CONFIG {
  char sensorName[32];
  char mqtt_valuePath[16];
  char api_valuePath[16];
  char unit[8];
  //  ['aqi', 'battery', 'carbon_dioxide', 'carbon_monoxide', 'current', 'energy', 'gas', 'humidity', 'illuminance', 'monetary', 'nitrogen_dioxide', 'nitrogen_monoxide', 'nitrous_oxide', 'ozone', 'pm1', 'pm10', 'pm25', 'power', 'power_factor', 'pressure', 'signal_strength', 'sulphur_dioxide', 'temperature', 'timestamp', 'volatile_organic_compounds', 'voltage']
  char deviceClass[32];
  class QualitySensor &sensor;
  bool display;
  bool publish;
};

class QualitySensor {
public:
  virtual void read() = 0;
  virtual const char* data() = 0;
  virtual const char* AQI() {return NULL;};
};

#endif
