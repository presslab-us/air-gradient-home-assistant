/**
 * @file SGP41.h
 * @license MIT
 */

#ifndef SGP41_h
#define SGP41_h

#include <Wire.h>
#include <SensirionI2CSgp41.h>
#include <VOCGasIndexAlgorithm.h>
#include <NOxGasIndexAlgorithm.h>
#include "QualitySensor.h"

class SGP41 {
  friend class VOCSensor;
  friend class NOxSensor;
public:
    uint16_t Init(float temp, float humidity);
    int getRaw();
    void readSample();
    void setAmbient(float temp, float humidity);

private:
    SensirionI2CSgp41 sgp41;
    VOCGasIndexAlgorithm voc_algorithm;
    NOxGasIndexAlgorithm nox_algorithm;
    uint16_t ambientRh = 0x8000;
    uint16_t ambientT = 0x6666;
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;
    char errorMessage[256];
    char _VOC[10];
    char _NOx[10];
};

class VOCSensor : public QualitySensor {
public:
  VOCSensor(class SGP41& sgp);
  void read() override;
  const char* data() override;
private:
  class SGP41& sgp;
};

class NOxSensor : public QualitySensor {
public:
  NOxSensor(class SGP41& sgp);
  void read() override;
  const char* data() override;
private:
  class SGP41& sgp;
};

#endif