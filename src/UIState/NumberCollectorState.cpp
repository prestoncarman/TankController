#include "NumberCollectorState.h"

#include <math.h>

#include "Devices/LiquidCrystal_TC.h"
#include "Devices/Serial_TC.h"
#include "MainMenu.h"
// useful for future debugging
#include "TC_util.h"

void NumCollectorState::clear() {
  hasDecimal = false;
  numDigits = 0;
  value = 0;
  factor = 10;
}

void NumCollectorState::handleKey(char key) {
  if (key >= '0' && key <= '9') {  // a digit
    numDigits++;
    handleDigit(key - '0');
  } else if (key == '*') {  // Decimal place (if we already have a decimal nothing happens)
    hasDecimal = !isInteger();
  } else if (key == 'A') {  // Accept
    setValue(value);
    return;                 // don't print the value again
  } else if (key == 'B') {  // Backspace
    backSpace();
  } else if (key == 'C') {  // Clear
    clear();
  } else if (key == 'D') {  // Disregard (escape/cancel)
    returnToMainMenu();
  }
  printValue();
}

void NumCollectorState::handleDigit(uint16_t digit) {
  if (hasDecimal) {
    value = value + (float)digit / factor;
    factor = factor * 10;
  } else {
    value = value * 10 + digit;
  }
}

void NumCollectorState::backSpace() {
  if (hasDecimal && factor == 10) {
    hasDecimal = false;
  } else if (hasDecimal) {
    numDigits--;
    factor = factor / 10;
    // we use factor/10 because factor is set to the next decimal not the current entered one
    value = floor(value * factor / 10) / (factor / 10);
  } else {
    numDigits--;
    value = floor(value / 10);
  }
}

void NumCollectorState::printValue() {
  char format[20], strValue[20];
  // The Arduino does not support variable widths, so we construct the format string at runtime!
  char buffer[8];
  floattostrf(getCurrentValue(), 7, getCurrentValuePrecision(), buffer, sizeof(buffer));
  memcpy(strValue, buffer, sizeof(buffer));
  snprintf_P(strValue + 7, 3, (PGM_P)F("->"));

  if (!hasDecimal) {
    // show user entry as an integer (no decimal yet)
    snprintf_P(format, sizeof(format), (PGM_P)F("%s%%6i"), strValue);
    snprintf(strValue, sizeof(strValue), format, (uint32_t)value);
  } else if (factor == 10) {
    // show user entry with a decimal but nothing beyond
    snprintf_P(format, sizeof(format), (PGM_P)F("%s%%6i."), strValue);
    snprintf(strValue, sizeof(strValue), format, (uint32_t)value);
  } else {
    // show user entry with appropriate precision (based on digits user has entered)
    uint16_t precision = log10(factor / 10);
    floattostrf(value, 7, precision, buffer, sizeof(buffer));
    memcpy(strValue + 9, buffer, sizeof(buffer));
    // TODO: Is there a test of this case?
  }
  LiquidCrystal_TC::instance()->writeLine(strValue, 1);
}

void NumCollectorState::start() {
  UIState::start();
  printValue();
}
