// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mraa.hpp>
#include <ssd1327.h>

#include <string>
#include <sstream>

#define I2C_BUS 6

class lasertag {
  public:
    lasertag();
    ~lasertag();

    void lcd_write_int(int i, int x, int y);
    void write_ammo();
    void write_status();
    uint8_t ammo(uint8_t ammo);
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
