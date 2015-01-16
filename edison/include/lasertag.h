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
    // standard constructor, instantiates i2c on bus 6
    lasertag();
    // constructor that specifies a i2c bus
    lasertag(int bus);
    // destructor
    ~lasertag();

    // reinitialize i2c
    void re_i2c();
    // read uint8 from i2c
    uint8_t i2c_read_int();

    // initializes some stuff (display), writes to lcd
    void init_ILI9225();

    mraa::I2c* i2c() {
      return m_i2c;
    }
    TFT_22_ILI9225* ILI9225() {
      return m_ILI9225;
    }

  private:
    TFT_22_ILI9225* m_ILI9225;
    mraa::I2c* m_i2c;
    int m_i2c_bus;

    bool m_ILI9225_init;
};
