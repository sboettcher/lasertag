/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */


#include <FastLED.h>

#ifndef LASERTAG_PLAYER_H
#define LASERTAG_PLAYER_H

#define HIT_LOST_PERCENT 20
#define AMMO_LOST 1
#define HEALTH_AT_START 70
#define FULL_AMMUNITION 50

#define MAX_NAME_LENGTH 12
#define MAX_INFO_TEXT 16 + MAX_NAME_LENGTH

class Player {
public:
  
  // Constructor
  Player(uint8_t identifier, /*String*/ char * name, CRGB teamColor);
 
  // The player is getting full ammo again
  void reload(int amount, bool increment);
  
    // The player is getting full ammo again
  void gotPoints(int amounts, bool increment);
  
  // The player is getting full ammo again
  void refillHealth(int amount, bool increment);
  
  // If Player got hit he will loose a defined amount of health
  int gotHit();
  
  // If Player makes one shot he will loose 1 ammunition
  int shooted();
  
  // Internal variable
  int _points;
  int _health;
  uint8_t _id;
  CRGB _teamColor;
  int _ammunition;
  char _infoText[MAX_INFO_TEXT+1];
  /*String*/ char * _playerName;
  /*String*/ char * _otherPlayerName;
  bool _taggerActive;  
  uint8_t  _gameMode;  
  uint8_t  _gameSpecial; 
  uint8_t  _ammoDecrease;  
  uint8_t  _healthDecrease;
  uint8_t  _maxHealth;
  uint8_t  _maxAmmo;
};

#endif
