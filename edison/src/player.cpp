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
void Player::reload(int amount) {
  m_ammo += amount;
  if (m_ammo > FULL_AMMO || amount < 0)
    m_ammo = FULL_AMMO;
}


// ___________________________________________________________________
void Player::refill_health(int amount) {
  m_health += amount;
  if (m_health > FULL_HEALTH || amount < 0)
    m_health = FULL_HEALTH;
}


// ___________________________________________________________________
int Player::hit() {
  m_health -= FULL_HEALTH * (HIT_LOST_PERCENT / 100.0);
  if (m_health < 0)
    m_health = 0;
  return m_health;
}


// ___________________________________________________________________
int Player::fired() {
  m_ammo -= 1;
  if (m_ammo < 0)
    m_ammo = 0;
  return m_ammo;
}
