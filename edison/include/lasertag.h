// Copyright 2015 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mraa.hpp>

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <future>

#include "./TFT_22_ILI9225.h"
#include "./edison_serial.h"
#include "./tcp_client.h"
#include "./player.h"

#define I2C_BUS 6
#define TFT_CS_PIN 0
#define TFT_RST_PIN 32
#define TFT_RS_PIN 47

class lasertag {
  public:
    // constructor
    lasertag();
    // destructor
    ~lasertag();

    // initialize i2c on bus and address
    void i2c_init(int bus, int address);
    // read uint8 from i2c
    uint8_t i2c_read_int();

    // initializes some display stuff
    void dsp_init();
    void dsp_draw_init();
    // initialize bluetooth master
    void bt_init();
    // initialize tcp client server connection
    void tcp_init(std::string address);
    // initialize trigger gpio input
    void gpio_init(int pin);

    // threading
    void spawn_threads();
    void join_threads();

    // getter/setter functions
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

    void set_bt_slave(std::string slave) {
      m_bt_slave = slave;
    }

  private:
    // register a hit, manage related stuff
    void hit_register(int code, int pos);

    // writes text to coordinates on display
    // if a font is given, the old one will be restored when the function returns
    void dsp_write(int x, int y, std::string text, uint8_t* font = Terminal6x8, uint16_t color = COLOR_WHITE);

    // draw the difference of the arguments given on the status bars
    void draw_health(int old_h, int new_h);
    void draw_ammo(int old_a, int new_a);

    // clear the corresponding text boxes on the display
    void clear_hit();
    void clear_tagged();

    // read from i2c, threaded
    void t_read_i2c();
    // read from bluetooth, threaded
    void t_read_bt();
    // read from tcp server, threaded
    void t_read_tcp();
    // read from trigger gpio pin, threaded
    void t_read_gpio();

    // parse a command coming i.e. from the server and do the corresponding action
    void parse_cmd(std::string cmd);

    
    //________________________________________________________________________________
    TFT_22_ILI9225* m_dsp;
    mraa::I2c* m_i2c;
    mraa::Gpio* m_gpio;
    edison_serial* m_bluetooth;
    tcp_client* m_client;

    int m_i2c_bus;

    bool m_dsp_init;
    bool m_i2c_init;
    bool m_gpio_init;
    bool m_bt_init;
    bool m_tcp_init;

    std::string m_bt_slave;

    std::vector<std::thread> m_threads;
    bool m_active;
    std::recursive_mutex m_mtx_hitreg;
    std::recursive_mutex m_mtx_dsp;

    uint16_t m_h_coord[4];
    uint16_t m_a_coord[4];
    uint16_t m_t_coord[4];

    Player m_player;

    std::map<uint8_t,std::string> m_hit_pos;
};


