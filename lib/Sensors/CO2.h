/**
 * @file CO2.h
 * @license MIT
 */

#ifndef CO2_h
#define CO2_h

#include <s8_uart.h>
#include "QualitySensor.h"

#ifdef ARDUINO_LOLIN_S2_MINI
  #include <HardwareSerial.h>
  #define PIN_CO2_RX 18
  #define PIN_CO2_TX 16
#endif


struct CO2_READ_RESULT {
    int co2 = -1;
    bool success = false;
};

class CO2: public QualitySensor {
public:
    void Init(int rx_pin = PIN_CO2_TX, int tx_pin = PIN_CO2_RX);
    void read() override;
    const char* data() override;
    const char* AQI() override;
private:
    int getCO2_Raw();
    Stream *_serial;
    S8_UART * _sensor_S8;
    char _data[10];
    char _AQI[10];
};

#endif