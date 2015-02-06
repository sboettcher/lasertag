#include "./player.h"


// ___________________________________________________________________
Player::Player() {
  m_name = "name";
  m_id = -1;
  m_score = 0;
  m_color = 65535;  // COLOR_WHITE
  m_health = FULL_HEALTH;
  m_ammo = FULL_AMMO;
  m_full_health = FULL_HEALTH;
  m_full_ammo = FULL_AMMO;
  m_vest = false;
}


// ___________________________________________________________________
int Player::reload(int amount) {
  int tmp = m_ammo;
  m_ammo += amount;
  if (m_ammo > m_full_ammo || amount < 0)
    m_ammo = m_full_ammo;
  return tmp;
}


// ___________________________________________________________________
int Player::refill_health(int amount) {
  int tmp = m_health;
  m_health += amount;
  if (m_health > m_full_health || amount < 0)
    m_health = m_full_health;
  return tmp;
}


// ___________________________________________________________________
int Player::hit() {
  int tmp = m_health;
  m_health -= m_full_health * (HIT_LOST_PERCENT / 100.0);
  if (m_health < 0)
    m_health = 0;
  return tmp;
}


// ___________________________________________________________________
int Player::fired() {
  int tmp = m_ammo;
  m_ammo -= 1;
  if (m_ammo < 0)
    m_ammo = 0;
  return tmp;
}

// ___________________________________________________________________
int Player::set_health(int health) {
  int tmp = m_health;
  m_health = health;
  return tmp;
}
// ___________________________________________________________________
int Player::set_max_health(int health) {
  int tmp = m_full_health;
  m_full_health = health;
  return tmp;
}

// ___________________________________________________________________
int Player::set_ammo(int ammo) {
  int tmp = m_ammo;
  m_ammo = ammo;
  return tmp;
}
// ___________________________________________________________________
int Player::set_max_ammo(int ammo) {
  int tmp = m_full_ammo;
  m_full_ammo = ammo;
  return tmp;
}
