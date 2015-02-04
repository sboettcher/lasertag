/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */


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
  _healthDecrease = HIT_LOST_PERCENT;  
  _maxHealth = HEALTH_AT_START;
  _maxAmmo = FULL_AMMUNITION;
}


// ___________________________________________________________________
void Player::reload(int amount, bool increment) {
  if (increment) {
    _ammunition += amount;
    if (_ammunition > _maxAmmo) {
      _ammunition = _maxAmmo;
    }
  } else {
    _ammunition -= amount;
    if (_ammunition <= 0) {
      _ammunition = 0;
    }
  }
}


// ___________________________________________________________________
void Player::refillHealth(int amount, bool increment) {
  if (increment) {
    _health += amount;
    if (_health > _maxHealth) {
      _health = _maxHealth;
    }
  } else {
    _health -= amount;
    if (_health <= 0) {
      _health = 0;
    }
  }
}

// ___________________________________________________________________
void Player::gotPoints(int amount, bool increment) {
  if (increment) {
    _points += amount;
  } else {
    _points -= amount;
    if (_points <= 0) {
      _points = 0;
    }
  }
}

// ___________________________________________________________________
int Player::gotHit() {
  _health -= _healthDecrease;
  if (_health <= 0) {
    _health = 0;
    return -1;
  } else {
    return 0;
  }
}

// ___________________________________________________________________
int Player::shooted() {
  _ammunition -= _ammoDecrease;
  if (_ammunition <= 0) {
    _ammunition = 0;
    return -1;
  } else {
    return 0;
  }
}
