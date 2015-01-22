// Copyright 2014 Sebastian Boettcher

#include "./lasertag.h"

//lasertag::lasertag()
//  : m_dsp(NULL), m_i2c(NULL),
//  m_i2c_bus(6), m_dsp_init(false),
//  m_bt_init(false), m_tcp_init(false),
//  m_bluetooth(NULL), m_client(NULL),
//  m_active(false)
//{
//  m_i2c = new mraa::I2c(m_i2c_bus);
//}

lasertag::lasertag(int bus)
  : m_dsp(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_dsp_init(false),
  m_bt_init(false), m_tcp_init(false),
  m_bluetooth(NULL), m_client(NULL),
  m_active(false)
{
  if (bus != 1 && bus != 6) {
    printf("\n[LASERTAG] Wrong i2c bus number!. Using bus 6.\n");
  } else {
    m_i2c_bus = bus;
  }
  m_i2c = new mraa::I2c(m_i2c_bus);

  m_health = MAX_HEALTH;
  m_ammo = MAX_AMMO;

  m_tln = 0;
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
  m_dsp = new TFT_22_ILI9225(TFT_LED_PIN, TFT_RST_PIN, TFT_RS_PIN);
  m_dsp->begin();
  m_dsp->setOrientation(0);
  m_dsp->setFont(Terminal12x16);
  dsp_draw_init();
  m_dsp->setFont(Terminal6x8);
  printf("Done.\n");
  fflush(stdout);

  m_dsp_init = true;
}

void lasertag::dsp_draw_init() {
  //int fontX = m_dsp->fontX();
  int fontY = m_dsp->fontY();
  int maxX = m_dsp->maxX();
  int maxY = m_dsp->maxY();

  int barH = 25;

  m_h_coord[0] = 10;
  m_h_coord[1] = 35 + 3 * fontY;
  m_h_coord[2] = maxX - 10;
  m_h_coord[3] = m_h_coord[1] + barH;
  m_a_coord[0] = 10;
  m_a_coord[1] = m_h_coord[3] + 15 + fontY;
  m_a_coord[2] = maxX - 10;
  m_a_coord[3] = m_a_coord[1] + barH;
  m_t_coord[0] = 10;
  m_t_coord[1] = m_a_coord[3] + 10;
  m_t_coord[2] = maxX - 10;
  m_t_coord[3] = maxY - 10;

  m_dsp->drawText(10, 10, "[Player]");
  m_dsp->drawText(10, 15 + fontY, "[Points]");
  m_dsp->drawLine(10, 20 + 2 * fontY, maxX - 10, 20 + 2 * fontY, COLOR_WHITE);

  m_dsp->drawText(10, 30 + 2 * fontY, "HEALTH:");
  m_dsp->drawRectangle(m_h_coord[0], m_h_coord[1], m_h_coord[2], m_h_coord[3], COLOR_WHITE);
  m_dsp->fillRectangle(m_h_coord[0]+1, m_h_coord[1]+1, m_h_coord[2]-1, m_h_coord[3]-1, COLOR_RED);
  m_dsp->drawText(10, m_h_coord[3] + 10, "AMMO:");
  m_dsp->drawRectangle(m_a_coord[0], m_a_coord[1], m_a_coord[2], m_a_coord[3], COLOR_WHITE);
  m_dsp->fillRectangle(m_a_coord[0]+1, m_a_coord[1]+1, m_a_coord[2]-1, m_a_coord[3]-1, COLOR_GREEN);

  m_dsp->drawRectangle(m_t_coord[0], m_t_coord[1], m_t_coord[2], m_t_coord[3], COLOR_WHITE);
}


void lasertag::bt_init(std::string slave) {
  m_bluetooth = new edison_serial("/dev/ttyMFD1");
  m_bluetooth->bt_master_init("EdisonBTMaster01", slave);
  
  m_bt_init = true;
}

void lasertag::tcp_init(std::string address) {
  m_client = new tcp_client;
  m_client->tcp_connect(address);
  
  m_tcp_init = true;
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
    if (!m_bt_init || !m_bluetooth->bt_connected())
      continue;
    if (m_bluetooth->available(0,1000)) {
      bt_rec = (uint8_t)m_bluetooth->serial_read();
      // 255 indicates new hit, read next 2 bytes for code and position
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
    if (!m_tcp_init || !m_client->connected())
      continue;
    if (m_client->tcp_available(0, 1000) > 0) {
      std::string tcp_rec = m_client->tcp_read_string("\n");
      printf("[LASERTAG] Hit by %s \n", tcp_rec.c_str());
      fflush(stdout);
    }
  }
}


void lasertag::spawn_threads() {
  printf("[LASERTAG] Spawning threads.\n");
  fflush(stdout);
  m_active = true;
  m_threads.push_back(std::thread(&lasertag::t_read_i2c, this));
  m_threads.push_back(std::thread(&lasertag::t_read_bt, this));
  m_threads.push_back(std::thread(&lasertag::t_read_tcp, this));
  printf("[LASERTAG] Done.\n");
  fflush(stdout);
}

void lasertag::join_threads() {
  printf("[LASERTAG] Joining threads.\n");
  fflush(stdout);
  m_active = false;
  for (auto& th : m_threads) th.join();
  m_threads.clear();
  printf("[LASERTAG] Done.\n");
  fflush(stdout);
}




void lasertag::hit_register(int code, int pos) {
  std::lock_guard<std::mutex> hitreg_lock(m_mtx_hitreg);
  printf("[LASERTAG] Hit by %i at %i\n", code, pos);
  fflush(stdout);
  std::stringstream ss;
  ss << code << "\n"; //<< ":" << pos << "\n";
  if (m_tcp_init && m_client->connected())
    m_client->tcp_send(ss.str());

  if (m_health > 0)
    draw_health(--m_health);

  std::stringstream text;
  text << "Hit by " << code << " at " << pos;
  dsp_write(text.str());
}


void lasertag::dsp_write(std::string text, uint16_t color) {
  if (!m_dsp_init)
    return;
  std::lock_guard<std::mutex> dsp_lock(m_mtx_dsp);

  int posY = 5 + m_t_coord[1] + m_tln++ * m_dsp->fontY();
  m_dsp->drawText(5 + m_t_coord[0], posY, text, color);

  if (posY >= m_t_coord[3] - 5 - 1.5 * m_dsp->fontY())
    m_tln = 0;

  printf("%d\n", m_tln);
  fflush(stdout);
}


void lasertag::draw_health(int health) {
  float perc = health * (1.0 / MAX_HEALTH);
  int h = ((m_h_coord[2] - m_h_coord[0]) * perc) + m_h_coord[0];

  m_dsp->fillRectangle(m_h_coord[0]+1, m_h_coord[1]+1, h, m_h_coord[3]-1, COLOR_RED);
  m_dsp->fillRectangle(h+1, m_h_coord[1]+1, m_h_coord[2]-1, m_h_coord[3]-1, COLOR_BLACK);
}
void lasertag::draw_ammo(int ammo) {
  float perc = ammo * (1.0 / MAX_AMMO);
  int a = (m_a_coord[2] - m_a_coord[0]) * perc + m_a_coord[0];

  m_dsp->fillRectangle(m_a_coord[0]+1, m_a_coord[1]+1, a, m_a_coord[3]-1, COLOR_GREEN);
  m_dsp->fillRectangle(a+1, m_a_coord[1]+1, m_a_coord[2]-1, m_a_coord[3]-1, COLOR_BLACK);
}
