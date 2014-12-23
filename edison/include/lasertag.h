// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mraa.hpp>
#include <ssd1327.h>

#include "./TFT_22_ILI9225.h"

#include <string>

#define I2C_BUS 6
#define AMMO_X 1
#define STATUS_X 0
#define TFT_LED_PIN 31
#define TFT_RST_PIN 32
#define TFT_RS_PIN 33

class lasertag {
  public:
    // standard constructor, instantiates lcd and i2c on bus 6
    lasertag();
    // constructor that specifies a i2c bus
    lasertag(int bus);
    // destructor, closes lcd
    ~lasertag();

    // initializes some stuff (display), writes to lcd
    void init_groveOLED();
    void init_ILI9225();

    // writes a string to the given position
    void groveOLED_write(std::string s, int x, int y);
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
    upm::SSD1327* groveOLED() {
      return m_groveOLED;
    }
    TFT_22_ILI9225* ILI9225() {
      return m_ILI9225;
    }

    void re_i2c();

  private:
    uint8_t m_ammo;
    bool m_active;

    upm::SSD1327* m_groveOLED;
    TFT_22_ILI9225* m_ILI9225;
    mraa::I2c* m_i2c;
    int m_i2c_bus;

    bool m_groveOLED_init;
};
