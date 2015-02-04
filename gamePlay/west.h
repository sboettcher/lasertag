#ifndef WEST_H
#define WEST_H
#include <SoftwareSerial.h>
#include <FastLED.h>
#include "config.h"
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class West {
public:
  // Constructor
  West(HardwareSerial *serial);
  
  // Start Serial
  void begin(int baudRate, int id);
  
  // Init sequence
  //void init();
  
  // Look if command availalble
  bool available();
  
  // Returns from whome and where player got hit
  Hit getCode();
  
  // Set the teamColor also at the west
  void setTeamColorRGB(uint8_t R, uint8_t G, uint8_t B);
  void setTeamColor(CRGB teamColor);
  
  // West should show dead status
  void isDead(bool dead);
  
private:
  void setupBlueToothConnection(int baudRate, int id);
  HardwareSerial * _westSerialHW;
};

#endif
