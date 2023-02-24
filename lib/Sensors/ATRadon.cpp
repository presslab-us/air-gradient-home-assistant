/**
 * @file ATRadon.cpp
 * @license MIT
 */

#include "ATRadon.h"

// sample radon every 15 minutes
//  readSample is called 3 times every 2 seconds.
#define COUNT_MAX (15 * 3600 * 3 / 2)
//#define COUNT_MAX (1 * 3600 * 3 / 2)

char cmd_preamble[] = { 0x55, 0x55, 0x50 };
char cmd_init[] = { 0x3b, 0x6f, 0x69, 0x6f, 0x69, 0x6f, 0x69, 0x6f, 0x69, 0x6f, 0x69, 0x6f, 0x69, 0x6f, 0x69, 0x6f, 0x69,
                    0x3b, 0x6f, 0x6f, 0x6f, 0x6f, 0x3b, 0x69, 0x6f, 0x6f, 0x69, 0x6f, 0x6f, 0x6f, 0x6f, 0x69, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6d };

char cmd_init3[] = { 0x3b, 0x30, 0x30, 0x30, 0x30, 0x3b, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x21,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0x30, 0x30, 0x30, 0x30, 0x3b, 0x6f, 0x6f, 0x6f, 0x6f,
                     0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x3b, 0x69, 0x6f, 0x69, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f,
                     0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6d };

char ATRadon::_data[sizeof(ATRadon::_data)];
int ATRadon::dataLen = 0;

int ATRadon::Init(int pin_rx, int pin_tx, int pin_en) {
  Serial.println("Initializing Airthings Radon...");
  _serial = new SoftwareSerial(pin_rx, pin_tx);
  _pin_en = pin_en;
  pinMode(pin_en, OUTPUT);
  delay(500);

  int result = readSample();
  if (result){
    Serial.println("Airthings Radon failed to start.");
  } else {
    Serial.println("Airthings Radon Successfully Initialized.");
  }
  return result;
}

void ATRadon::requestSample() {
  static long counter = 0;
  if (counter >= COUNT_MAX) {
    counter = 0;
    readSample();
  }
}

int ATRadon::readSample() {
  int bytes;
  strcpy(_day, "null");
  strcpy(_week, "null");
  strcpy(_long, "null");

  // set GPIO high to enable serial mode
  digitalWrite(_pin_en, HIGH);
  delay(500);
  Serial.println("preamble");
  _serial->begin(4800);
  _serial->write(cmd_preamble, sizeof(cmd_preamble));
  _serial->flush();
  delay(2000);
  _serial->end();
  // make sure to initialize with > 144 byte bufCapacity.
  _serial->begin(115200, SWSERIAL_8N1, -1, -1, false, 150);
  _serial->write(cmd_init, sizeof(cmd_init));
  _serial->flush();
  bytes = _serial->readBytes(_data, sizeof(_data));
  delay(100);
  if (bytes != sizeof(ATRadon::_data)) {
    digitalWrite(_pin_en, LOW);
    return -1;
  }
  snprintf(_week, sizeof(_week), "%d", _data[16]);
  snprintf(_long, sizeof(_long), "%d", _data[58]);
/*  for (unsigned int i = 0; i < sizeof(_data); i ++ )
  {
     printf("%02x\n", _data[i]);
  }
  printf("---------------\n");
  _serial->flush();
  _serial->write(cmd_init2, sizeof(cmd_init2));
  bytes = _serial->readBytes(_data, sizeof(_data));
  snprintf(_day, sizeof(_day), "%d", _data[56]);
  for (unsigned int i = 0; i < sizeof(_data); i ++ )
  {
     printf("%02x\n", _data[i]);
  }
*/
  digitalWrite(_pin_en, LOW);
  _serial->end();

  return 0;
}

const char* ATRadon::getRadonWeek() {
  return _week;
}

const char* ATRadon::getRadonLong() {
  return _long;
}

int ATRadon::toAQI(int data) {
  int aqi;

  if (data <= 75.0)
    aqi = ((50 - 0) / (75.0 - .0) * (data - .0) + 0);
  else if (data <= 100.0)
    aqi = ((100 - 50) / (100.0 - 75.0) * (data - 75.0) + 50);
  else if (data <= 200.0)
    aqi = ((150 - 100) / (200.0 - 100.0) * (data - 100.0) + 100);
  else if (data <= 600.0)
    aqi = ((200 - 150) / (600.0 - 200.0) * (data - 200.0) + 150);
  else if (data <= 2000.0)
    aqi = ((500 - 200) / (2000.0 - 600.0) * (data - 600.0) + 200);
  else
    aqi = 500;

  return aqi;
}

ATRadon_weekSensor::ATRadon_weekSensor(class ATRadon& rad) : rad(rad) {}

void ATRadon_weekSensor::read() {
  rad.requestSample();
}

const char* ATRadon_weekSensor::data() {
  return rad.getRadonWeek();
}

const char* ATRadon_weekSensor::AQI() {
  int aqi = rad.toAQI(atoi(rad.getRadonWeek()));
  snprintf(_AQI, sizeof(_AQI), "%d", aqi);
  return _AQI;
}

ATRadon_longSensor::ATRadon_longSensor(class ATRadon& rad) : rad(rad) {}

void ATRadon_longSensor::read() {
  rad.requestSample();
}

const char* ATRadon_longSensor::data() {
  return rad.getRadonLong();
}

const char* ATRadon_longSensor::AQI() {
  int aqi = rad.toAQI(atoi(rad.getRadonWeek()));
  snprintf(_AQI, sizeof(_AQI), "%d", aqi);
  return _AQI;
}
