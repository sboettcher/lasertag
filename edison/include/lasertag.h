// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mraa.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>

#include "./TFT_22_ILI9225.h"
#include "./edison_serial.h"
#include "./tcp_client.h"

#define I2C_BUS 6
#define TFT_LED_PIN 31
#define TFT_RST_PIN 32
#define TFT_RS_PIN 33

#define MAX_HEALTH 10
#define MAX_AMMO 10

class lasertag {
  public:
    // standard constructor, instantiates i2c on bus 6
    //lasertag();
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
    void dsp_draw_init();
    // initialize bluetooth master
    void bt_init(std::string slave);
    // initialize tcp client server connection
    void tcp_init(std::string address);

    // threading
    void spawn_threads();
    void join_threads();

    // getter functions
    mraa::I2c* i2c() {
      return m_i2c;
    }
    TFT_22_ILI9225* dsp() {
      return m_dsp;
    }
    edison_serial* bt() {
      return m_bluetooth;
    }
    tcp_client* tcp() {
      return m_client;
    }

  private:
    // register a hit, manage related stuff
    void hit_register(int code, int pos);

    // writes text to coordinates on display
    void dsp_write(std::string text, uint16_t color = COLOR_WHITE);

    void draw_health(int health);
    void draw_ammo(int ammo);


    // read from i2c, threaded
    void t_read_i2c();
    // read from bluetooth, threaded
    void t_read_bt();
    // read from tcp server, threaded
    void t_read_tcp();


    TFT_22_ILI9225* m_dsp;

    mraa::I2c* m_i2c;
    int m_i2c_bus;

    bool m_dsp_init;
    bool m_bt_init;
    bool m_tcp_init;

    edison_serial* m_bluetooth;
    tcp_client* m_client;

    std::vector<std::thread> m_threads;
    bool m_active;
    std::mutex m_mtx_hitreg;
    std::mutex m_mtx_dsp;

    uint16_t m_h_coord[4];
    uint16_t m_a_coord[4];
    uint16_t m_t_coord[4];

    int m_health;
    int m_ammo;

    int m_tln;
};


