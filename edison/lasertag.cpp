// Copyright 2014 Sebastian Boettcher

#include "./lasertag.h"

lasertag::lasertag()
  : m_ammo(75), m_active(false),
  m_lcd(NULL), m_i2c(NULL) {
  printf("init display... ");
  fflush(stdout);
  m_lcd = new upm::SSD1327(I2C_BUS);
  printf("Done.\n");
  fflush(stdout);

  m_i2c = new mraa::I2c(I2C_BUS);
}

lasertag::~lasertag() {
  m_lcd->close();
  printf("\nLCD closed.\n");
}

void lasertag::init() {
  m_lcd->setGrayLevel(255);
  write_status();
  m_lcd->setCursor(AMMO_X, 0);
  m_lcd->write("ammo: ");
  write_ammo();
}

void lasertag::lcd_write_int(int i, int x, int y) {
  m_lcd->setCursor(x, y);
  std::stringstream ss;
  ss << i;
  m_lcd->write(ss.str());
}

void lasertag::write_ammo() {
  if (m_ammo < 100) {
    lcd_write_int(0, AMMO_X, 6);
    lcd_write_int(m_ammo, AMMO_X, 7);
  } else {
    lcd_write_int(m_ammo, AMMO_X, 6);
  }
}

void lasertag::write_status() {
  m_lcd->setCursor(STATUS_X, 2);
  if (m_active) {
    m_lcd->write(" active ");
  } else {
    m_lcd->write("inactive");
  }
}

uint8_t lasertag::ammo(uint8_t ammo) {
  uint8_t tmp = m_ammo;
  m_ammo = ammo;
  write_ammo();
  return tmp;
}

bool lasertag::active(bool active) {
  bool tmp = m_active;
  m_active = active;
  write_status();
  return tmp;
}

