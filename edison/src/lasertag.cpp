// Copyright 2014 Sebastian Boettcher

#include <string>

#include "./lasertag.h"

lasertag::lasertag()
  : m_ammo(75), m_active(false),
  m_groveOLED(NULL), m_ILI9225(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_groveOLED_init(false)
{
  m_i2c = new mraa::I2c(m_i2c_bus);
}

lasertag::lasertag(int bus)
  : m_ammo(75), m_active(false),
  m_groveOLED(NULL), m_ILI9225(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_groveOLED_init(false)
{
  if (bus != 1 && bus != 6) {
    printf("\n[LASERTAG] Wrong i2c bus number!. Using bus 6.\n");
  } else {
    m_i2c_bus = bus;
  }
  m_i2c = new mraa::I2c(m_i2c_bus);
}

lasertag::~lasertag() {
  if (m_groveOLED_init) {
    m_groveOLED->close();
    printf("\n[LASERTAG] grove OLED closed.\n");
  }
  delete m_i2c;
}

void lasertag::re_i2c() {
  delete m_i2c;
  m_i2c = new mraa::I2c(m_i2c_bus);
}

void lasertag::init_groveOLED() {
  printf("[LASERTAG] init grove OLED on bus %d... ", m_i2c_bus);
  fflush(stdout);
  m_groveOLED = new upm::SSD1327(m_i2c_bus);
  printf("Done.\n");
  fflush(stdout);
  m_groveOLED->setGrayLevel(255);

  m_groveOLED_init = true;

  // write_status();
  // m_groveOLED->setCursor(AMMO_X, 0);
  // m_groveOLED->write("ammo: ");
  // write_ammo();
}

void lasertag::init_ILI9225() {
  printf("[LASERTAG] init ILI9225... ");
  fflush(stdout);
  m_ILI9225 = new TFT_22_ILI9225(TFT_LED_PIN, TFT_RST_PIN, TFT_RS_PIN);
  m_ILI9225->begin();
  m_ILI9225->setOrientation(0);
  m_ILI9225->setFont(Terminal6x8);
  printf("Done.\n");
  fflush(stdout);

  m_ILI9225_init = true;
}

void lasertag::groveOLED_write(std::string s, int x, int y) {
  if (!m_groveOLED_init) {
    printf("[LASERTAG] groveOLED not initialized!\n");
    return;
  }
  m_groveOLED->setCursor(x, y);
  m_groveOLED->write(s);
}

void lasertag::write_ammo() {
  if (m_ammo < 100) {
    groveOLED_write("0", AMMO_X, 6);
    groveOLED_write(std::to_string(m_ammo), AMMO_X, 7);
  } else {
    groveOLED_write(std::to_string(m_ammo), AMMO_X, 6);
  }
}

void lasertag::write_status() {
  m_groveOLED->setCursor(STATUS_X, 2);
  if (m_active) {
    m_groveOLED->write(" active ");
  } else {
    m_groveOLED->write("inactive");
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

uint8_t lasertag::i2c_read_int() {
  uint8_t rx_tx_buf[1];
  m_i2c->read(rx_tx_buf, 1);
  return rx_tx_buf[0];
}


