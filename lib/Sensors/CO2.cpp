/**
 * @file CO2.cpp
 * @brief
 * 
 * @license MIT
 */

#include "CO2.h"
#include "Arduino.h"
#include "SoftwareSerial.h"

void CO2::Init(int rx_pin, int tx_pin) {
  Serial.println("Initializing CO2...");
#if !defined(SOC_USB_OTG_SUPPORTED)
  _serial = new SoftwareSerial(rx_pin, tx_pin);
  static_cast<SoftwareSerial *>(_serial)->begin(9600);
#else
  _serial = new HardwareSerial(0);
  static_cast<HardwareSerial *>(_serial)->begin(9600, SERIAL_8N1, rx_pin, tx_pin);
#endif
  _sensor_S8 = new S8_UART(*_serial);

  delay(1000);
  if(_sensor_S8->get_co2() == 0) {
    Serial.println("CO2 Sensor Failed to Initialize ");
  }
  else{
    Serial.println("CO2 Successfully Initialized. Heating up for 10s");
    delay(10000);
  }
}

void CO2::read() {
  if (!_sensor_S8) {
    return;
  }
  int result = _sensor_S8->get_co2();
  if (result != 0) {
    snprintf(_data, sizeof(_data), "%d", result);
  } else {
    strcpy(_data, "null");
  }
}

const char* CO2::AQI() {
  float data = atoi(_data);
  int aqi;

  if (data <= 400)
    aqi = ((50 - 0) / (400 - .0) * (data - .0) + 0);
  else if (data <= 1000)
    aqi = ((100 - 50) / (1000 - 400.0) * (data - 400) + 50);
  else if (data <= 1500)
    aqi = ((150 - 100) / (1500 - 1000.0) * (data - 1000) + 100);
  else if (data <= 2000)
    aqi = ((200 - 150) / (2000 - 1500.0) * (data - 1500) + 150);
  else if (data <= 5000)
    aqi = ((300 - 200) / (5000 - 2000.0) * (data - 2000) + 200);
  else if (data <= 10000)
    aqi = ((500 - 300) / (10000 - 5000.0) * (data - 5000) + 300);
  else
    aqi = 500;
  snprintf(_AQI, sizeof(_AQI), "%d", aqi);
  return _AQI;
}

const char* CO2::data() {
  return _data;
}
