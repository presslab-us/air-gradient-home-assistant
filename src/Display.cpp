/**
 * @file Display.cpp
 * @brief
 * 
 * @license MIT
 */

#include "Display.h"
#include "Connectivity.h"
#include "iot_iconset_16x16.h"

#define SCREEN_W 128
#define SCREEN_H 64
#define HALF_W SCREEN_W/2
#define HALF_H SCREEN_H/2

SH1106Wire Display::display(0x3c, SDA, SCL);
OLEDDisplayUi Display::ui(&display);
const SENSOR_CONFIG* Display::_sensors;
int Display::_num_sensors;
int Display::_num_frames;
char Display::_APName[20];

void Display::setSensors(const SENSOR_CONFIG* sensors, int numSensors) {
  _sensors = sensors;
  _num_sensors = numSensors;
  _num_frames = 0;
  for (int i = 0; i < _num_sensors; i ++) {
    if (_sensors[i].display) {
        _num_frames ++;
    }
  }
  ui.setFrames(frames, _num_frames);
}

void Display::Init(bool flip) {
  ui.setTargetFPS(30);
  ui.setIndicatorPosition(TOP);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_RIGHT);
  ui.setOverlays(overlays, 1);
  ui.disableAutoTransition();
  ui.setTimePerTransition(200);
  ui.init();
  display.setBrightness(128);
  if (flip) {
    display.flipScreenVertically();
  }
}

void Display::next() {
  ui.nextFrame();
}

void Display::update() {
  ui.update();
}

void Display::setAPName(const char* AP) {
  strncpy(_APName, AP, sizeof(_APName));
}

void Display::msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {

  // Draw WiFi (or crossed circle) icon
  if(!clientStatus) {
    display->drawIco16x16(SCREEN_W - 16, 0, (const uint8_t *)nocon_icon16x16);
  } else if (WiFi.RSSI() < -62) {
    display->drawIco16x16(SCREEN_W - 16, 0, (const uint8_t *)wifi2_icon16x16);
  } else {
    display->drawIco16x16(SCREEN_W - 16, 0, (const uint8_t *)wifi1_icon16x16);
  }
}

void Display::drawFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  static bool flip = false;
  int slide;
  int frame = state->currentFrame;

  if (state->frameState == FIXED) {
    flip = false;
  } else {
    frame = (frame + flip) % _num_frames;
    flip = !flip;
  }

  for (slide = 0; slide < _num_sensors; slide ++) {
    if (_sensors[slide].display == true) {
      frame --;
      if (frame < 0) {
        break;
      }
    }
  }

  const char* ln1 = _sensors[slide].sensorName;
  char ln2[64];
  snprintf(ln2, sizeof(ln2), "%s %s", _sensors[slide].sensor.data(), _sensors[slide].unit);

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 12 + y, ln1);
  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, HALF_H + y, ln2);
}

void Display::configModeCallback(WiFiManager *myWiFiManager)
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connect to AP\nto configure:");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 40, _APName);
  display.display();
}

void Display::showLoading()
{
  display.clear();
  // Draw top and bottom bulbs
  display.fillCircle(HALF_W, SCREEN_H/4, SCREEN_H/4);
  display.fillCircle(HALF_W, 3*(SCREEN_H/4), SCREEN_H/4);
  display.fillRect(HALF_W - 5, HALF_H - 5, 10, 10);
  display.setColor(BLACK);
  // Cut out center to form an outline
  display.fillCircle(HALF_W, SCREEN_H/4, SCREEN_H/4 - 2);
  display.fillCircle(HALF_W, 3*(SCREEN_H/4), SCREEN_H/4 - 2);
  display.fillRect(HALF_W - 3, HALF_H - 4, 6, 8);
  // Cut off top and bottom
  display.fillRect(0, 0, SCREEN_W, 8);
  display.fillRect(0, SCREEN_H - 8, SCREEN_W, 8);
  display.setColor(WHITE);
  // Fill in top and bottom outlines
  display.fillRect(HALF_W-HALF_W/4, 6, HALF_W/2, 2);
  display.fillRect(HALF_W-HALF_W/4, SCREEN_H - 8, HALF_W/2, 2);
  display.display();
}
