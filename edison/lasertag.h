// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mraa.hpp>
#include <ssd1327.h>

#include <string>
#include <sstream>

#define I2C_BUS 6
#define AMMO_X 1
#define STATUS_X 0

class lasertag {
  public:
    // standard constructor, instantiates lcd and i2c
    lasertag();
    // destructor, closes lcd
    ~lasertag();

    // initializes some stuff, writes to lcd
    void init();

    // writes an int as a string to the given position
    void lcd_write_int(int i, int x, int y);
    // writes the current ammo to lcd
    void write_ammo();
    // writes the current status to lcd
    void write_status();

    // changes ammo amount, returns old value
    uint8_t ammo(uint8_t ammo);
    // changes status, returns old value
    bool active(bool active);

    uint8_t ammo() {
      return m_ammo;
    }

    bool active() {
      return m_active;
    }

    mraa::I2c* i2c() {
      return m_i2c;
    }

  private:
    uint8_t m_ammo;
    bool m_active;
    upm::SSD1327* m_lcd;
    mraa::I2c* m_i2c;
};
