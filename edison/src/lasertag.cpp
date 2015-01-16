// Copyright 2014 Sebastian Boettcher

#include <string>

#include "./lasertag.h"

lasertag::lasertag()
  : m_ILI9225(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_ILI9225_init(false)
{
  m_i2c = new mraa::I2c(m_i2c_bus);
}

lasertag::lasertag(int bus)
  : m_ILI9225(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_ILI9225_init(false)
{
  if (bus != 1 && bus != 6) {
    printf("\n[LASERTAG] Wrong i2c bus number!. Using bus 6.\n");
  } else {
    m_i2c_bus = bus;
  }
  m_i2c = new mraa::I2c(m_i2c_bus);
}

lasertag::~lasertag() {
  delete m_i2c;
}

void lasertag::re_i2c() {
  delete m_i2c;
  m_i2c = new mraa::I2c(m_i2c_bus);
}

uint8_t lasertag::i2c_read_int() {
  uint8_t rx_tx_buf[1];
  m_i2c->read(rx_tx_buf, 1);
  return rx_tx_buf[0];
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



