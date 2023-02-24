/**
 * @file PMSensor.cpp
 * @brief
 * 
 * @license MIT
 */

#include "PMSensor.h"
#include "Arduino.h"
#include "Stream.h"

// capture time is 5 seconds
#define CAPTURE_TIME (5000L)
// wakeup time is 15 seconds
#define WAKE_TIME (15000L)
// sample period is 1.5 minutes
#define SLEEP_TIME (1.5 * 60000L - WAKE_TIME - CAPTURE_TIME)

// 32 bytes per packet for PMS5003
SerialPM PMSensor::_pms(PLANTOWER_AUTO, PIN_PMS_RX, PIN_PMS_TX);
HardwareSerial* PMSensor::_serial;

int PMSensor::Init(int rx_pin, int tx_pin) {
  Serial.println("Initializing PMS...");
  _sleeping = false;
  _triggered = false;
  _lastMillis = millis();
  strcpy(_PM25, "null");

  _pms.init();
  if(_pms.status != _pms.OK) {
    Serial.printf("PMS sensor failed to initialize; status is %d\n", _pms.status);
    return -1;
  }
  _serial = static_cast<HardwareSerial *>(_pms.getSerialPort());
  // Increase buffer size so we can get a bunch of packets to average
  //  5 seconds / 0.2 second min interval * 36 bytes per packet = 900 bytes
  _serial->setRxBufferSize(1024);
  _pms.trigRead(); // clear out buffer
  _pms.wake();

  Serial.println("PMS starting initialization.");
  return 0;
}

void PMSensor::waitInit() {
  Serial.println("PMS finishing initialization.");

  while (millis() - _lastMillis <= WAKE_TIME) {
    delay(1);
  }
  read();
  _lastMillis = millis();
  while (millis() - _lastMillis <= CAPTURE_TIME) {
    delay(1);
  }
  read();
}

void PMSensor::read() {
  // SLEEP->WAKE->TRIGGER->CAPTURE->READ
  if (_triggered) {
    if (millis() - _lastMillis > CAPTURE_TIME) {
      float pm25 = 0.0;
      float hcho = 0.0;
      int cnt = 0;
      _lastMillis += CAPTURE_TIME;
      Serial.println("PMS read");
      _triggered = false;
      _pms.sleep();
      _sleeping = true;
      // Oversample to get higher resolution.
      //  The PMS5003 has variable rate so we get as many packets as we can and average them.
      //  32 bytes per packet for PMS5003, 36 for the PMS5003ST.
      while (_serial->available() >= 32) {
        _pms.read();
        if (_pms)
        {
          pm25 += _pms.pm25;
          hcho += _pms.hcho;
          cnt ++;
        } else {
          Serial.printf("PMS Sensor read error; status is %d\n", _pms.status);
        }
      }
      pm25 /= cnt;
      hcho /= cnt;
      hcho *= 1000; // convert from mg/m³ to µg/m³
      if (cnt) {
        snprintf(_PM25, sizeof(_PM25), "%.1f", pm25);
        snprintf(_HCHO, sizeof(_HCHO), "%.0f", hcho);
      } else {
        strcpy(_PM25, "null");
        strcpy(_HCHO, "null");
      }
    }
  } else if (_sleeping) {
    if (millis() - _lastMillis > SLEEP_TIME) {
      _lastMillis += SLEEP_TIME;
      Serial.println("PMS wake");
      _pms.wake();
      _sleeping = false;
    }
  } else {
    if (millis() - _lastMillis > WAKE_TIME) {
      _lastMillis += WAKE_TIME;
      Serial.println("PMS trigger");
      _pms.trigRead();
      _triggered = true;
    }
  }
}

const char* PMSensor::data() {
  return _PM25;
}

const char* PMSensor::AQI() {
  static char result[10];
  float data = atoi(_PM25);
  int aqi;
  if (data <= 12.0)
    aqi = ((50 - 0) / (12.0 - .0) * (data - .0) + 0);
  else if (data <= 35.4)
    aqi = ((100 - 50) / (35.4 - 12.0) * (data - 12.0) + 50);
  else if (data <= 55.4)
    aqi = ((150 - 100) / (55.4 - 35.4) * (data - 35.4) + 100);
  else if (data <= 150.4)
    aqi = ((200 - 150) / (150.4 - 55.4) * (data - 55.4) + 150);
  else if (data <= 250.4)
    aqi = ((300 - 200) / (250.4 - 150.4) * (data - 150.4) + 200);
  else if (data <= 350.4)
    aqi = ((400 - 300) / (350.4 - 250.4) * (data - 250.4) + 300);
  else if (data <= 500.4)
    aqi = ((500 - 400) / (500.4 - 350.4) * (data - 350.4) + 400);
  else
    aqi = 500;
  sprintf(result, "%d", aqi);
  return result;
}

HCHOSensor::HCHOSensor(class PMSensor& sensor) : _sensor(sensor) {}

const char* HCHOSensor::data() {
  return _sensor._HCHO;
}

const char* HCHOSensor::AQI() {
  static char result[10];
  sprintf(result, "null");
  return result;
}
