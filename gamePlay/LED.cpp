#include "LED.h"
// ___________________________________________________________________
LED::LED(int maxHealth) {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(ledColors, NUMB_LEDS);
  teamColor = CRGB(BLACK);
  changed = 1;
  _pattern = 0;
  _error = 0;
  _maxHealth = maxHealth;
}

// ___________________________________________________________________
void LED::setTeamColorRGB(uint8_t R, uint8_t G, uint8_t B) {
  teamColor = CRGB(R, G, B);
}

// ___________________________________________________________________
void LED::setTeamColor(CRGB theTeamColor) {
  teamColor = theTeamColor;
}

// ___________________________________________________________________
void LED::allLEDsOneColor(CRGB color) {
  int i = 0;
  for (i = 0; i < NUMB_LEDS; i++) {
    ledColors[i] = color;
  }
}

// ___________________________________________________________________
void LED::allLEDsOneColorRGB(uint8_t R, uint8_t G, uint8_t B) {
  CRGB color = CRGB(R, G, B);
  allLEDsOneColor(color);
}

// ___________________________________________________________________
void LED::sendAllLEDsOneColor(CRGB color) {
  allLEDsOneColor(color);  
  FastLED.show();
}

// ___________________________________________________________________
void LED::sendAllLEDsOneColorRGB(uint8_t R, uint8_t G ,uint8_t B) {
  allLEDsOneColorRGB(R,G,B);
  FastLED.show();
}

// ___________________________________________________________________
void LED::sendOneLEDColor(int led, CRGB color) {
  ledColors[led] = color;
  FastLED.show();
}

// ___________________________________________________________________
void LED::sendOneLEDRGB(int led, uint8_t R, uint8_t G, uint8_t B) {
  CRGB color = CRGB(R, G, B);
  ledColors[led] = color;
  FastLED.show();
}

// ___________________________________________________________________
void LED::fade(CRGB startColor, CRGB goalColor, int fadeTime) {
  
  if (_state == 0) {
    _state++;
    _fadeRed = startColor.red;
    _fadeBlue = startColor.blue;
    _fadeGreen = startColor.green;
  }
  if (millis() - _timer > fadeTime) {
    uint8_t changedSth = 0;
    if (_fadeRed < goalColor.red) {
      _fadeRed = _fadeRed + 1;
      changedSth = changedSth + 1;
    } else if (_fadeRed > goalColor.red) {
      _fadeRed = _fadeRed - 1;
      changedSth = changedSth + 1;
    }
    if (_fadeGreen < goalColor.green) {
      _fadeGreen = _fadeGreen + 1;
      changedSth = changedSth + 1;
    } else if (_fadeGreen > goalColor.green) {
      _fadeGreen = _fadeGreen - 1;
      changedSth = changedSth + 1;
    }
    if (_fadeBlue < goalColor.blue) {
      _fadeBlue = _fadeBlue + 1;
      changedSth = changedSth + 1;
    } else if (_fadeBlue > goalColor.blue) {
      _fadeBlue = _fadeBlue - 1;
      changedSth = changedSth + 1;
    }
    if (changedSth == 0) {
      _error++;
      _state = 0;
      _timer = millis();
    }
    
    sendAllLEDsOneColorRGB(_fadeRed, _fadeGreen, _fadeBlue);
    _timer = millis();
  }
}

// ___________________________________________________________________
void LED::showShootPattern() {
  if (millis() - _timer > SHOOT_PATTERN_DELAY) {
    if (_state == NUMB_LEDS) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _state = 0;
      _pattern = 0;
      _timer = millis();
    } else {
      allLEDsOneColor(CRGB(BLACK));
      ledColors[_state] = CRGB(SHOOT_COLOR);
      FastLED.show();
      _state++;
    }
    _timer = millis();
  }
}


// ___________________________________________________________________
void LED::showBootPattern() {
  if (millis() - _timer > BOOT_PATTERN_DELAY) {
    if (_state > NUMB_LEDS) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _state = 0;
      _pattern = 0;
      _timer = millis();
    } else {
      allLEDsOneColor(CRGB(BLACK));
      for (int state = 0; state < _state; state++) {
        ledColors[state] = CRGB(BOOT_COLOR);
      }
      FastLED.show();
      _state++;
    }
    _timer = millis();
  }
}



// ___________________________________________________________________
void LED::showHitPattern() {
  
  if (millis() - _timer > HIT_PATTERN_DELAY) {
    if (_state == 0) { 
      sendAllLEDsOneColor(CRGB(HIT_COLOR_1));
      _timer = millis();
      _state++;
    } else if (_state == 1) {
      sendAllLEDsOneColor(CRGB(HIT_COLOR_2));
      _timer = millis();
      _state++;
    } else if (_state == 2) {
      sendAllLEDsOneColor(CRGB(HIT_COLOR_3));
      _timer = millis();
      _state++;
    } else if (_state == 3) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _timer = millis();
      _state++;
    } else {
      _timer = millis();
      _state = 0;
      _pattern = 0;
    }
  }
}

// ___________________________________________________________________
void LED::showReloadPattern() {
  if (millis() - _timer > RELOAD_PATTERN_DELAY) {
    if (_state > NUMB_LEDS) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _state = 0;
      _pattern = 0;
      _timer = millis();
    } else {
      allLEDsOneColor(CRGB(BLACK));
      for (int state = 0; state < _state; state++) {
        ledColors[state] = CRGB(LOW_AMMO_COLOR);
      }
      FastLED.show();
      _state++;
    }
    _timer = millis();
  } 
}

// ___________________________________________________________________
void LED::showHealthPattern() {
  if (millis() - _timer > HEALTH_PATTERN_DELAY) {
    if (_state > NUMB_LEDS) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _state = 0;
      _pattern = 0;
      _timer = millis();
    } else {
      allLEDsOneColor(CRGB(BLACK));
      for (int state = 0; state < _state; state++) {
        ledColors[state] = teamColor;
      }
      FastLED.show();
      _state++;
    }
    _timer = millis();
  }
  
}

// ___________________________________________________________________
void LED::showLowAmmoPattern() {
  if (millis() - _timer > AMMO_PATTERN_DELAY) {
    if (_state == 0) { 
      sendAllLEDsOneColor(CRGB(LOW_AMMO_COLOR));
      _timer = millis();
      _state++;
    } else if (_state == 1) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _timer = millis();
      _state++;
    } else {
      _timer = millis();
      _state = 0;
      _pattern = 0;
    }
  }
}

// ___________________________________________________________________
void LED::showDeadPattern() {
  if (millis() - _timer > DEAD_PATTERN_DELAY) {
    if (_state == 0) { 
      sendAllLEDsOneColor(CRGB(DEAD_COLOR));
      _timer = millis();
      _state++;
    } else if (_state == 1) {
      sendAllLEDsOneColor(CRGB(BLACK));
      _timer = millis();
      _state++;
    } else {
      _timer = millis();
      _state = 0;
      _pattern = 0;
    }
  }
}

// ___________________________________________________________________
void LED::showCurrentHealth(int health, int maxHealth) {
  if (changed) {
    int numLedOn = (int) ((float)NUMB_LEDS / (float) maxHealth * health);
    if (numLedOn == 0) numLedOn++;
    if (numLedOn > NUMB_LEDS) numLedOn = NUMB_LEDS;
    int state = 0;
    
    for (state = 0; state < numLedOn; state++) {
      ledColors[state] = teamColor;
    }
    FastLED.show();
  }
  changed = 0;
  _pattern = 0;
}

// ___________________________________________________________________
void LED::showErrorPattern() {
  if (_error == 0) {
    fade(CRGB(BLACK), CRGB(RED), 5);
  } else if (_error == 1) {
    fade(CRGB(RED), CRGB(GREEN), 5);
  } else if (_error == 2) {
    fade(CRGB(GREEN), CRGB(BLUE), 5);
  } else if (_error == 3) {
    fade(CRGB(BLUE), CRGB(BLACK), 5);
  } else if (_error == 4) {
    _pattern = 0;
    _error = 0;
  }
}

// ___________________________________________________________________
void LED::updateLED(int pattern, int health) {
  if (_pattern == 0) _pattern = pattern;
  if (pattern == HIT_PATTERN) {
    _pattern = pattern;
    _state = 0;
    _error = 0;
    _timer = 0;
  }
  if (_pattern != HEALTH_PATTERN) changed = 1;
  switch (_pattern) {
    case ERROR_PATTERN: 
      showErrorPattern();
      break;
    case HIT_PATTERN:
      showHitPattern();
      break;
    case RELOAD_PATTERN: 
      showReloadPattern();
      break;
    case HEALTH_PATTERN: 
      showHealthPattern();
      break;
    case CURR_HEALTH_PATTERN: 
      showCurrentHealth(health, _maxHealth);
      break;
    case AMMO_PATTERN: 
      showLowAmmoPattern();
      break;
    case BOOT_PATTERN: 
      showBootPattern();
      break;
    case SHOOT_PATTERN: 
      showShootPattern();
      break;
    case DEAD_PATTERN: 
      showDeadPattern();
      break;
  }
}
