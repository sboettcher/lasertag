#include "./player.h"


// ___________________________________________________________________
Player::Player() {
  m_name = "name";
  m_id = -1;
  m_score = 0;
  m_color = 65535;  // COLOR_WHITE
  m_health = 7;
  m_ammo = 25;
  m_full_health = 10;
  m_full_ammo = 25;
  m_vest = false;
  m_health_lost_per = 1;
  m_ammo_lost_per = 1;
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
  //m_health -= m_full_health * (m_health_lost_perc / 100.0);
  m_health -= m_health_lost_per;
  if (m_health < 0)
    m_health = 0;
  return tmp;
}


// ___________________________________________________________________
int Player::fired() {
  int tmp = m_ammo;
  //m_ammo -= m_full_ammo * (m_ammo_lost_perc / 100.0);
  m_ammo -= m_ammo_lost_per;
  if (m_ammo < 0)
    m_ammo = 0;
  return tmp;
}


// ___________________________________________________________________
int Player::scored() {
  int tmp = m_score;
  m_score += m_score_per;
  return tmp;
}



// ___________________________________________________________________
int Player::set_health(int health) {
  if (health > m_full_health)
    m_full_health = health;

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
  if (ammo > m_full_ammo)
    m_full_ammo = ammo;

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
