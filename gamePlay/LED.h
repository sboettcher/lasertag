/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */


#ifndef LED_H
#define LED_H

#include <FastLED.h>
#include "inttypes.h"

// The Pin on which the stripe is connected
#define LED_PIN A1
// The number of LEDs
#define NUMB_LEDS 7

// Shoot pattern delay
#define SHOOT_PATTERN_DELAY 40
// Boot pattern delay
#define BOOT_PATTERN_DELAY 100
// Hit pattern delay
#define HIT_PATTERN_DELAY 150
// Reload pattern delay
#define RELOAD_PATTERN_DELAY 300
// Reload pattern delay
#define AMMO_PATTERN_DELAY 200
// Reload pattern delay
#define DEAD_PATTERN_DELAY 200
// Reload pattern delay
#define HEALTH_PATTERN_DELAY 300
// The three Hit colors
#define HIT_COLOR_1 255, 255, 255
#define HIT_COLOR_2 255, 255, 255
#define HIT_COLOR_3 255, 255, 255
// The shoot color
#define SHOOT_COLOR 255, 0, 0
// The boot color
#define BOOT_COLOR 255, 255, 255
// The shoot color
#define SHOOT_COLOR 255, 0, 0
// The shoot color
#define DEAD_COLOR 255, 255, 255
// The shoot color
#define LOW_AMMO_COLOR 255, 255, 0

// defining some standard colors
#define BLACK 0, 0, 0
#define WHITE 255, 255, 255
#define RED 255, 0, 0
#define GREEN 0, 255, 0
#define BLUE 0, 0, 0

#define ERROR_PATTERN 9
#define HIT_PATTERN 1
#define RELOAD_PATTERN 2 
#define HEALTH_PATTERN 3
#define CURR_HEALTH_PATTERN 4
#define AMMO_PATTERN 5
#define BOOT_PATTERN 6
#define SHOOT_PATTERN 7
#define DEAD_PATTERN 8


class LED {
public:
  struct ledcolor{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  
  typedef struct ledcolor LedColor ;
  
  // Constructor
  LED(int maxHealth);
  
  // Set the team color
  void setTeamColorRGB(uint8_t R, uint8_t G, uint8_t B);
  void setTeamColor(CRGB teamColor);
  
  void showShootPattern();
  
  void showBootPattern();
  
  void showHitPattern();
  
  void showReloadPattern();
  
  void showHealthPattern(/*int health, int maxHealth*/);
  
  void showLowAmmoPattern(int health);
  
  void showDeadPattern();
  
  void showCurrentHealth(int health, int maxHealth);
  
  void showErrorPattern();
  
  void updateLED(int pattern, int health);
  
    
private:
  void allLEDsOneColor(CRGB color);
  void allLEDsOneColorRGB(uint8_t R, uint8_t G, uint8_t B);
  void sendAllLEDsOneColor(CRGB color);
  void sendAllLEDsOneColorRGB(uint8_t R, uint8_t G ,uint8_t B);
  void sendOneLEDColor(int led, CRGB color);
  void sendOneLEDRGB(int led, uint8_t R, uint8_t G, uint8_t B);
  void fade(CRGB startColor, CRGB goalColor, int fadeTime);
  
  CRGB teamColor;
  
  CRGB ledColors[NUMB_LEDS];
  
  uint8_t _fadeRed;
  uint8_t _fadeBlue;
  uint8_t _fadeGreen;
  int _pattern;
  int _error;
  int _maxHealth;
  long _timer;
  long _state;
  int changed;
};

#endif
