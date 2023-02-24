/**
 * @file SGP41.cpp
 * @brief
 * 
 * @license MIT
 */

#include "SGP41.h"
#include "Arduino.h"
#include <Wire.h>

uint16_t SGP41::Init(float temp, float humidity) {
  Serial.println("Initializing SGP41...");

  // time in seconds needed for NOx conditioning
  uint16_t conditioning_s = 10;
  uint16_t error;

  uint16_t testResult;

  setAmbient(temp, humidity);

  sgp41.begin(Wire);

  error = sgp41.executeSelfTest(testResult);
  if (error) {
    Serial.println("SGP41 Sensor Failed to Initialize ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    return error;
  } else if (testResult != 0xD400) {
    Serial.println("SGP41 executeSelfTest failed with error: ");
    Serial.println(String(testResult));
  }

  Serial.println("SGP41 Successfully Initialized. Conditioning for 10s");

  while (conditioning_s) {
    delay(1000);

    // During NOx conditioning (10s) SRAW NOx will remain 0
    error = sgp41.executeConditioning(ambientRh, ambientT, srawVoc);
    conditioning_s--;
  }
  return error;
}

void SGP41::setAmbient(float temp, float humidity) {
  ambientT = (temp + 45.0) * 65535.0 / 175.0;
  ambientRh = humidity * 65535.0 / 100.0;
}

void SGP41::readSample() {
  int retryLimit = 5;
  int ctr = 0;
  int voc_index;
  int nox_index;

  int result = getRaw();
  while(result == -1) {
    result = getRaw();
    if((ctr >= retryLimit) || (result == -1)){
      strcpy(_VOC, "null");
      strcpy(_NOx, "null");
      return;
    }
    ctr++;
  }

  voc_index = voc_algorithm.process(srawVoc);
  nox_index = nox_algorithm.process(srawNox);

  snprintf(_VOC, sizeof(_VOC), "%d", voc_index);
  snprintf(_NOx, sizeof(_NOx), "%d", nox_index);
}

int SGP41::getRaw()
{
  uint16_t error;

  error = sgp41.measureRawSignals(ambientRh, ambientT, srawVoc, srawNox);
  if (error) {
    Serial.println("Error trying to execute TVOC measureRawSignals(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    return -1;
  }
  return 0;
}

VOCSensor::VOCSensor(class SGP41& sgp) : sgp(sgp) {}

void VOCSensor::read() {
  sgp.readSample();
}

const char* VOCSensor::data() {
    return sgp._VOC;
}

NOxSensor::NOxSensor(class SGP41& sgp) : sgp(sgp) {}

void NOxSensor::read() {
  sgp.readSample();
}

const char* NOxSensor::data() {
    return sgp._NOx;
}