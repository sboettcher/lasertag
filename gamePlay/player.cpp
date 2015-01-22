#include "player.h"


// ___________________________________________________________________
Player::Player() {
}

// ___________________________________________________________________
Player::Player(int identifier) {
  _id = identifier;
  _health = HEALTH_AT_START;
  _ammunition = FULL_AMMUNITION;
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

// ___________________________________________________________________
int Player::getHealth() {
  return _health;
}

// ___________________________________________________________________
int Player::getAmmo() {
  return _ammunition;
}

// ___________________________________________________________________
int Player::getID() {
  return _id;
}