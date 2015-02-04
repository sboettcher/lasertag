#include "player.h"


// ___________________________________________________________________
Player::Player(uint8_t identifier, char * name/*String name*/, CRGB teamColor) {
  _playerName = name;
  _id = identifier;
  _health = HEALTH_AT_START;
  _ammunition = FULL_AMMUNITION;
  _points = 0;
  _teamColor = teamColor;
  // Internal variable
  //_infoText[MAX_INFO_TEXT+1];
  //_otherPlayerName;
  _taggerActive = true;  
  _gameMode = 0;  
  _gameSpecial = 0; 
  _ammoDecrease = AMMO_LOST;  
  _incrementPointAmount = 0;
  _incrementAmmoAmount = 0;
  _incrementHealthAmount = 0;  
}


// ___________________________________________________________________
void Player::reload(int amount) {
  _ammunition += amount;
  if (_ammunition > FULL_AMMUNITION) {
    _ammunition = FULL_AMMUNITION;
  }
}


// ___________________________________________________________________
void Player::refillHealth(int amount) {
  _health += amount;
  if (_health > HEALTH_AT_START) {
    _health = HEALTH_AT_START;
  }
}


// ___________________________________________________________________
int Player::gotHit() {
  _health -= HIT_LOST_PERCENT;
  if (_health <= 0) {
    _health = 0;
    return -1;
  } else {
    return 0;
  }
}

// ___________________________________________________________________
int Player::shooted() {
  _ammunition -= 1;
  if (_ammunition <= 0) {
    _ammunition = 0;
    return -1;
  } else {
    return 0;
  }
}
