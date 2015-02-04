/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */


#ifndef DISPLAY_H
#define DISPLAY_H
#include <SoftwareSerial.h>
#include <SPI.h>
#include "TFT_22_ILI9225.h"
#include "config.h"

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 0   // 0 if wired to +5V directly

// Rect Sizes
#define AMMO_RECT_HEIGTH 25
#define HEALTH_RECT_HEIGTH 25
#define INFO_RECT_HEIGTH 50

// Rect Colors
#define RECT_COLOR RGB888_RGB565(0xffffffu)
#define AMMO_RECT_COLOR RGB888_RGB565(0xffff00u)
#define HEALTH_RECT_COLOR RGB888_RGB565(0xff0000u)
#define SEPERATOR_COLOR RGB888_RGB565(0xffffffu)

// Text colors
#define INFO_TEXT_COLOR RGB888_RGB565(0xffffffu)
#define PLAYER_TEXT_COLOR RGB888_RGB565(0xffffffu)
#define POINTS_TEXT_COLOR RGB888_RGB565(0xffff00u)
#define HEADLINE_TEXT_COLOR RGB888_RGB565(0xffffffu)
#define BACKGROUND_COLOR RGB888_RGB565(0x000000u)

// Text Font sizes
#define INFO_FONT Terminal6x8
#define HEADLINE_FONT Terminal12x16
#define PLAYER_FONT Terminal12x16
#define POINTS_FONT Terminal12x16

#define INFO_FONT_SIZE 8
#define HEADLINE_FONT_SIZE 16
#define PLAYER_FONT_SIZE 16
#define POINTS_FONT_SIZE 16

// Text
#define AMMO_TEXT "Ammo:"
#define HEALTH_TEXT "Health:"

// Positions
// Offsets
#define TOP_OFFSET 5
#define SIDE_OFFSET 5
#define RECT_SIDE_OFFSET 5
#define PLAYER_Y_SEPERATOR 5
#define POINTS_Y_SEPERATOR 5
#define SEPERATOR_Y_SEPERATOR 10
#define HEALTH_TEXT_Y_SEPERATOR 5
#define HEALTH_Y_SEPERATOR 10
#define AMMO_TEXT_Y_SEPERATOR 5
#define AMMO_Y_SEPERATOR 5
#define INFO_Y_SEPERATOR 5

// At first display the Player name
#define PLAYER_TEXT_X_OFFSET SIDE_OFFSET
#define PLAYER_TEXT_Y_OFFSET TOP_OFFSET

// Then the Points
#define POINTS_TEXT_X_OFFSET SIDE_OFFSET
#define POINTS_TEXT_Y_OFFSET PLAYER_TEXT_Y_OFFSET + PLAYER_FONT_SIZE + PLAYER_Y_SEPERATOR

// Then Seperator
#define SEPERATOR_X_OFFSET 0
#define SEPERATOR_Y_OFFSET POINTS_TEXT_Y_OFFSET + POINTS_FONT_SIZE + POINTS_Y_SEPERATOR


// Then Health Headline
#define HEALTH_TEXT_X_OFFSET SIDE_OFFSET
#define HEALTH_TEXT_Y_OFFSET SEPERATOR_Y_OFFSET + SEPERATOR_Y_SEPERATOR

// Then Health Box
#define HEALTH_RECT_OFFSET HEALTH_TEXT_Y_OFFSET + HEADLINE_FONT_SIZE + HEALTH_TEXT_Y_SEPERATOR

// Then Ammo Headline
#define AMMO_TEXT_X_OFFSET SIDE_OFFSET
#define AMMO_TEXT_Y_OFFSET HEALTH_RECT_OFFSET + HEALTH_RECT_HEIGTH + HEALTH_Y_SEPERATOR

// Then Ammo Box
#define AMMO_RECT_OFFSET AMMO_TEXT_Y_OFFSET + HEADLINE_FONT_SIZE + AMMO_TEXT_Y_SEPERATOR

// Then Info Box
#define INFO_RECT_OFFSET AMMO_RECT_OFFSET + AMMO_RECT_HEIGTH + AMMO_Y_SEPERATOR

// Then Info Text
#define INFO_TEXT_X_OFFSET SIDE_OFFSET + INFO_Y_SEPERATOR
#define INFO_TEXT_Y_OFFSET INFO_RECT_OFFSET + INFO_Y_SEPERATOR

#define HIT_POS_0 "front"
#define HIT_POS_1 "left shoulder"
#define HIT_POS_2 "right shoulder"
#define HIT_POS_3 "back"
#define HIT_POS_100 "tagger"

class Display {
public:
  // Constructor
  Display(uint8_t orientation, int maxAmmo, int maxHealth);
  
  void bootUp();
  
  // draw Player Name
  void drawPlayerName(/*String*/char * playerName);
  
  // draw ponts
  void drawPoints(int points);
  
  // Redraw complete Display
  void redraw(/*String*/char * playerName, int points, int ammo, int health);
  
  // update Ammo
  void updateAmmo(int ammo);
  
  // update health
  void updateHealth(int health);
  
  // update Info box
  bool updateInfo(char * infoText);
  
  bool updateInfo(char * playerName, uint8_t position);
  //bool updateInfo(String infoText);
  bool updateInfo(Hit hit);
  
private:
  uint8_t _orientation;
  int _ammo;
  int _health;
  int _maxAmmo;
  int _maxHealth;
  float _fracHealth;
  float _fracAmmo;
};

#endif
