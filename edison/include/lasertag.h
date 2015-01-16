// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mraa.hpp>

#include "./TFT_22_ILI9225.h"
#include "./edison_serial.h"
#include "./tcp_client.h"

#define I2C_BUS 6
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

    // initializes some stuff (display)
    void dsp_init();
    // initialize bluetooth master
    void bt_init(std::string slave);
    // initialize tcp client server connection
    void tcp_init(std::string address);

    // read from i2c, threaded
    void t_read_i2c();
    // read from bluetooth, threaded
    void t_read_bt();
    // read from tcp server, threaded
    void t_read_tcp();


    mraa::I2c* i2c() {
      return m_i2c;
    }
    TFT_22_ILI9225* dsp() {
      return m_ILI9225;
    }
    edison_serial* bt() {
      return m_bluetooth;
    }
    tcp_client* tcp() {
      return m_client;
    }

  private:
    TFT_22_ILI9225* m_ILI9225;

    mraa::I2c* m_i2c;
    int m_i2c_bus;

    bool m_ILI9225_init;

    edison_serial* m_bluetooth;
    tcp_client* m_client;
};


