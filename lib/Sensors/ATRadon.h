/**
 * @file ATRadon.h
 * @license MIT
 */

#ifndef ATRadon_h
#define ATRadon_h

#include <SoftwareSerial.h>
#include "QualitySensor.h"

#ifdef ARDUINO_LOLIN_S2_MINI
  #define PIN_RAD_TX 12
  #define PIN_RAD_RX 11
  #define PIN_RAD_EN 5
#endif

class ATRadon {
public:
  int Init(int pin_rx = PIN_RAD_RX, int pin_tx = PIN_RAD_TX, int pin_en = PIN_RAD_EN);
  int getRaw();
  void requestSample();
  int readSample();
  const char* getRadonWeek();
  const char* getRadonLong();
  int toAQI(int value);
  static int dataLen;
private:
  SoftwareSerial *_serial;
  static char _data[144];
  int _pin_en;
  char _day[10];
  char _week[10];
  char _long[10];
};

class ATRadon_weekSensor : public QualitySensor {
public:
  ATRadon_weekSensor(class ATRadon& rad);
  void read() override;
  const char* data() override;
  const char* AQI() override;
private:
  class ATRadon& rad;
  char _AQI[10];
};

class ATRadon_longSensor : public QualitySensor {
public:
  ATRadon_longSensor(class ATRadon& rad);
  void read() override;
  const char* data() override;
  const char* AQI() override;
private:
  class ATRadon& rad;
  char _AQI[10];
};

#endif