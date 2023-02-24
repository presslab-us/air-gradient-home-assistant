/**
 * @file PMSensor.h
 * @license MIT
 */

#ifndef PMSensor_h
#define PMSensor_h

#include <stdint.h>
#include <SoftwareSerial.h>
#include <PMserial.h>

#include "QualitySensor.h"

#ifdef ARDUINO_LOLIN_S2_MINI
  #define PIN_PMS_TX 9
  #define PIN_PMS_RX 7
#endif



class Stream;

class PMSensor : public QualitySensor {
  friend class HCHOSensor;
public:
  int Init(int pin_rx = PIN_PMS_RX, int pin_tx = PIN_PMS_TX);
  void waitInit();
  void read() override;
  const char* data() override;
  const char* AQI() override;
private:
  static SerialPM _pms;
  static HardwareSerial* _serial;
  unsigned long _lastMillis;
  bool _sleeping;
  bool _triggered;
  char _PM25[10];
protected:
  char _HCHO[10];
};

class HCHOSensor : public QualitySensor {
public:
  HCHOSensor(PMSensor& sensor);
  void read() override {};
  const char* data() override;
  const char* AQI() override;
private:
  const PMSensor& _sensor;
};

#endif