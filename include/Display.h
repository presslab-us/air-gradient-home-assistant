#ifndef Display_h
#define Display_h

#include <WiFiManager.h>
#include <PubSubClient.h>
#include "QualitySensor.h"

#if defined(ESP32)
#include <WiFi.h>
#endif

#include <Wire.h>
#include <SH1106Wire.h>
#include <OLEDDisplayUi.h>

//struct SENSOR_CONFIG;

class Display {
public:
  void Init(bool flip);
  void setSensors(const SENSOR_CONFIG* sensors, int numSensors);
  void next();
  void update();
  void showLoading();
  void setAPName(const char* AP);
  static void configModeCallback(WiFiManager *myWiFiManager);
private:
  static SH1106Wire display;
  static OLEDDisplayUi ui;
  static void drawFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  static void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
  static const SENSOR_CONFIG* _sensors;
  static int _num_sensors;
  static int _num_frames;
  static char _APName[20];
  OverlayCallback overlays[1] = { msOverlay };
  FrameCallback frames[12] = {drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame, drawFrame };
};

#endif
