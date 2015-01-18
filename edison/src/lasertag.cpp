// Copyright 2014 Sebastian Boettcher

#include "./lasertag.h"

lasertag::lasertag()
  : m_ILI9225(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_ILI9225_init(false),
  m_bluetooth(NULL), m_client(NULL),
  m_active(false)
{
  m_i2c = new mraa::I2c(m_i2c_bus);
}

lasertag::lasertag(int bus)
  : m_ILI9225(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_ILI9225_init(false),
  m_bluetooth(NULL), m_client(NULL),
  m_active(false)
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

void lasertag::dsp_init() {
  printf("[LASERTAG] init ILI9225... ");
  fflush(stdout);
  m_ILI9225 = new TFT_22_ILI9225(TFT_LED_PIN, TFT_RST_PIN, TFT_RS_PIN);
  m_ILI9225->begin();
  m_ILI9225->setOrientation(0);
  //m_ILI9225->setFont(Terminal6x8);
  m_ILI9225->setFont(Terminal12x16);
  printf("Done.\n");
  fflush(stdout);

  m_ILI9225_init = true;
}

void lasertag::bt_init(std::string slave) {
  m_bluetooth = new edison_serial("/dev/ttyMFD1");
  m_bluetooth->bt_master_init("EdisonBTMaster01", slave);
}

void lasertag::tcp_init(std::string address) {
  m_client = new tcp_client;
  m_client->tcp_connect(address);
}


void lasertag::t_read_i2c() {
  int i2c_rec = 0;
  while (m_active) {
    i2c_rec = i2c_read_int();

    if (i2c_rec != 0 && i2c_rec != 255) {
      hit_register(i2c_rec, 0);
      i2c_rec = 0;
    }
  }
}

void lasertag::t_read_bt() {
  int bt_rec = 0;
  int code = 0;
  int pos = 0;
  while (m_active) {
    if (m_bluetooth->available(0,1000)) {
      bt_rec = (uint8_t)m_bluetooth->serial_read();
      if (bt_rec == 255) {
        while (!m_bluetooth->available(0,1000));
        code = (uint8_t)m_bluetooth->serial_read();
        while (!m_bluetooth->available(0,1000));
        pos = (uint8_t)m_bluetooth->serial_read();
        hit_register(code, pos);
        bt_rec = 0;
      }
    }
  }
}

void lasertag::t_read_tcp() {
  while (m_active) {
    if (m_client->tcp_available(0, 1000) > 0) {
      std::string tcp_rec = m_client->tcp_read_string("\n");
      printf("Hit by %s \n", tcp_rec.c_str());
    }
  }
}


void lasertag::spawn_threads() {
  m_active = true;
  m_threads.push_back(std::thread(&lasertag::t_read_i2c, this));
  m_threads.push_back(std::thread(&lasertag::t_read_bt, this));
  m_threads.push_back(std::thread(&lasertag::t_read_tcp, this));
}

void lasertag::join_threads() {
  m_active = false;
  for (auto& th : m_threads) th.join();
  m_threads.clear();
}




void lasertag::hit_register(int code, int pos) {
  std::lock_guard<std::mutex> hitreg_lock(m_mtx_hitreg);
  printf("Hit by %i at %i\n", code, pos);
  fflush(stdout);
}


void lasertag::dsp_write(int x, int y, std::string text, uint16_t color) {
  std::lock_guard<std::mutex> dsp_lock(m_mtx_dsp);
  m_ILI9225->drawText(x, y, text, color);
}


