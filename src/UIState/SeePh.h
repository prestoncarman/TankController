/**
 * SeePh.h
 *
 * See PID Constants
 */
#pragma once
#include "TC_util.h"
#include "UIState.h"

class SeePh : public UIState {
public:
  enum phSetTypeTypes {
    FLAT_TYPE,
    RAMP_TYPE,
    SINE_TYPE,
  };

  SeePh(TankController *tc) : UIState(tc) {
  }
  void loop();
  const __FlashStringHelper *name() {
    return F("SeePh");
  }
  void start() {
    startTime = millis();
  }

private:
  void loadHeader(uint16_t line);
  void loadPhSetType(uint16_t line);
  void loadTypeVariables(uint16_t line);
  void loadValues(uint16_t line);

  uint32_t startTime;
};
