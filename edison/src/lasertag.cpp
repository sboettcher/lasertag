// Copyright 2015 Sebastian Boettcher

#include "./lasertag.h"

//________________________________________________________________________________
lasertag::lasertag()
  : m_dsp(NULL), m_i2c(NULL),
  m_i2c_bus(6), m_i2c_init(false), m_dsp_init(false),
  m_bt_init(false), m_tcp_init(false), m_gpio_init(false),
  m_bluetooth(NULL), m_client(NULL),
  m_active(false)
{
  m_health = MAX_HEALTH;
  m_ammo = MAX_AMMO;
}

//________________________________________________________________________________
lasertag::~lasertag() {
  delete m_i2c;
  delete m_gpio;
}



/*
 * I2C stuff
 */
 
//________________________________________________________________________________
void lasertag::i2c_init(int bus, int address) {
  // make sure i2c bus number is 1 or 6
  if (bus != 1 && bus != 6) {
    printf("\n[LASERTAG] Wrong i2c bus number!. Using bus 6.\n");
  } else {
    m_i2c_bus = bus;
  }
  m_i2c = new mraa::I2c(m_i2c_bus);
  m_i2c->address(0x68);

  m_i2c_init = true;
}

//________________________________________________________________________________
uint8_t lasertag::i2c_read_int() {
  if (!m_i2c_init)
    return 0;

  // read one integer from i2c bus and return it
  uint8_t rx_tx_buf[1];
  m_i2c->read(rx_tx_buf, 1);
  return rx_tx_buf[0];
}



/*
 * Display stuff
 */
 
//________________________________________________________________________________
void lasertag::dsp_init() {
  // constructor, init display, set rotation
  printf("[LASERTAG] init ILI9225... ");
  fflush(stdout);
  m_dsp = new TFT_22_ILI9225(TFT_CS_PIN, TFT_RST_PIN, TFT_RS_PIN);
  m_dsp->begin();
  m_dsp->setOrientation(0);
  printf("Done.\n");
  fflush(stdout);

  m_dsp_init = true;

  // draw the initial setup, fill status bars
  printf("[LASERTAG] draw init screen... ");
  fflush(stdout);
  dsp_draw_init();
  draw_health(0, MAX_HEALTH);
  draw_ammo(0, MAX_AMMO);
  printf("Done.\n");
  fflush(stdout);
}

//________________________________________________________________________________
void lasertag::dsp_draw_init() {
  m_dsp->setFont(Terminal12x16);  // larger font

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

  // draw top part with placeholders for player name and current score
  m_dsp->drawText(10, 10, "[Player]");
  m_dsp->drawText(10, 15 + fontY, "[Score]");
  m_dsp->drawLine(10, 20 + 2 * fontY, maxX - 10, 20 + 2 * fontY, COLOR_WHITE);

  // draw status bars
  m_dsp->drawText(10, 30 + 2 * fontY, "HEALTH:");
  m_dsp->drawRectangle(m_h_coord[0]-1, m_h_coord[1]-1, m_h_coord[2]+1, m_h_coord[3]+1, COLOR_WHITE);
  m_dsp->drawText(10, m_h_coord[3] + 10, "AMMO:");
  m_dsp->drawRectangle(m_a_coord[0]-1, m_a_coord[1]-1, m_a_coord[2]+1, m_a_coord[3]+1, COLOR_WHITE);

  // draw text boxes for hit, tagged status
  m_dsp->setFont(Terminal6x8);
  fontY = m_dsp->fontY();

  m_t_coord[0] = 10;
  m_t_coord[1] = m_a_coord[3] + 11 + fontY;
  m_t_coord[2] = maxX - 10;
  m_t_coord[3] = maxY - 5;

  m_dsp->drawText(10, m_a_coord[3] + 10, "hit by");
  m_dsp->drawText(maxX/2, m_a_coord[3] + 10, "tagged");
  m_dsp->drawRectangle(m_t_coord[0], m_t_coord[1], m_t_coord[2], m_t_coord[3], COLOR_WHITE);
  m_dsp->drawLine(maxX/2, m_t_coord[1], maxX/2, m_t_coord[3], COLOR_WHITE);
}



/*
 * other initializations
 */
 
//________________________________________________________________________________
void lasertag::bt_init(std::string slave) {
  m_bluetooth = new edison_serial("/dev/ttyMFD1", B38400);  // UART1, bt module needs 38400 baud
  m_bluetooth->bt_master_init("EdisonBTMaster01", slave);
  
  m_bt_init = true;
}

//________________________________________________________________________________
void lasertag::tcp_init(std::string address) {
  m_client = new tcp_client;
  m_client->tcp_connect(address);
  
  m_tcp_init = true;
}

//________________________________________________________________________________
void lasertag::gpio_init(int pin) {
  m_gpio = new mraa::Gpio(pin);
  if (m_gpio == NULL) {
    printf("[LASERTAG] gpio init failed.\n");
    fflush(stdout);
    return;
  }

  mraa_result_t response;
  response = m_gpio->dir(mraa::DIR_IN);
  if (response != MRAA_SUCCESS) {
    mraa::printError(response);
    return;
  }

  m_gpio_init = true;
}



/*
 * Functions called as threads. These handle all the IO stuff etc.
 */

//________________________________________________________________________________
void lasertag::t_read_i2c() {
  int i2c_rec = 0;

  std::vector<std::future<void>> handles;  // collect all handles for async calls
  while (m_active) {
    if (m_i2c_init) {
      // read from i2c
      i2c_rec = i2c_read_int();
      // filter out idle, error bytes
      if (i2c_rec != 0 && i2c_rec != 255) {
        // asynchronously call hitreg function so i2c communication can continue during
        handles.push_back(std::async(std::launch::async, &lasertag::hit_register, this, i2c_rec, 4));
        i2c_rec = 0;
      }
    }
  }
  for (auto& h : handles) h.get();  // make sure all async calls return
}

//________________________________________________________________________________
void lasertag::t_read_bt() {
  int bt_rec = 0;
  int code = 0;
  int pos = 0;

  std::vector<std::future<void>> handles;  // collect all handles for async calls
  while (m_active) {
    if (!m_bt_init || !m_bluetooth->bt_connected())
      continue;
    if (m_bluetooth->available(0,1000)) {
      // read byte from bluetooth serial
      bt_rec = (uint8_t)m_bluetooth->serial_read();
      // 255 indicates new hit, read next 2 bytes for code and position
      if (bt_rec == 255) {
        while (!m_bluetooth->available(0,1000));
        code = (uint8_t)m_bluetooth->serial_read();
        while (!m_bluetooth->available(0,1000));
        pos = (uint8_t)m_bluetooth->serial_read();
        // asynchronously call hitreg function so bt communication can continue during
        handles.push_back(std::async(std::launch::async, &lasertag::hit_register, this, code, pos));
        bt_rec = 0;
      }
    }
  }
  for (auto& h : handles) h.get();  // make sure all async calls return
}

//________________________________________________________________________________
void lasertag::t_read_tcp() {
  std::vector<std::future<void>> handles;  // collect all handles for async calls
  while (m_active) {
    if (!m_tcp_init || !m_client->connected())
      continue;
    if (m_client->tcp_available(0, 1000) > 0) {
      // read string from tcp, doesn't return terminating characters
      std::string tcp_rec = m_client->tcp_read_string("\n");
      printf("[LASERTAG] Hit by %s \n", tcp_rec.c_str());
      fflush(stdout);
      // asynchronously draw on display so tcp communication can continue
      handles.push_back(std::async(std::launch::async, &lasertag::dsp_write, this, m_t_coord[0] + 5, m_t_coord[1] + 15, tcp_rec, Terminal11x16, COLOR_WHITE));
    }
  }
  for (auto& h : handles) h.get();  // make sure all async calls return
}

//________________________________________________________________________________
void lasertag::t_read_gpio() {
  std::vector<std::future<void>> handles;  // collect all handles for async calls
  while (m_active) {
    if (m_gpio_init && m_gpio->read() == 0) {
      if (m_ammo > 0) {
        // decrement ammo on trigger activation
        // asynchronously draw on display so tcp communication can continue
        handles.push_back(std::async(std::launch::async, &lasertag::draw_ammo, this, m_ammo, m_ammo - 1));
        --m_ammo;
      }
      usleep(100000);  // wait some time so tagger can't trigger continuously
    }
  }
  for (auto& h : handles) h.get();  // make sure all async calls return
}



/*
 * Threading management
 */
 
//________________________________________________________________________________
void lasertag::spawn_threads() {
  printf("[LASERTAG] Spawning threads.\n");
  fflush(stdout);
  m_active = true;
  m_threads.push_back(std::thread(&lasertag::t_read_i2c, this));
  m_threads.push_back(std::thread(&lasertag::t_read_bt, this));
  m_threads.push_back(std::thread(&lasertag::t_read_tcp, this));
  m_threads.push_back(std::thread(&lasertag::t_read_gpio, this));
  printf("[LASERTAG] Done.\n");
  fflush(stdout);
}

//________________________________________________________________________________
void lasertag::join_threads() {
  printf("[LASERTAG] Joining threads.\n");
  fflush(stdout);
  m_active = false;
  for (auto& th : m_threads) th.join();
  m_threads.clear();
  printf("[LASERTAG] Done.\n");
  fflush(stdout);
}



/*
 * Gameplay functions
 */

//________________________________________________________________________________
void lasertag::hit_register(int code, int pos) {
  // make sure there is only one execution of the function at a time
  std::lock_guard<std::recursive_mutex> hitreg_lock(m_mtx_hitreg);
  printf("[LASERTAG] Hit by %i at %i\n", code, pos);
  fflush(stdout);
  
  // send the player code to the server to recieve player name
  std::stringstream ss;
  ss << code << "\n"; //<< ":" << pos << "\n";
  if (m_tcp_init && m_client->connected())
    m_client->tcp_send(ss.str());

  // decrement health by one
  if (m_health > 0) {
    //auto handle1 = std::async(std::launch::async, &lasertag::draw_health, this, m_health, m_health - 1);
    draw_health(m_health, m_health - 1);
    --m_health;
  }

  // write hit position to display
  //auto handle2 = std::async(std::launch::async, &lasertag::clear_hit, this);
  clear_hit();
  std::stringstream text;
  //text << code << " -> " << pos;
  switch (pos) {
    case 0: text << "Front"; break;
    case 1: text << "Back"; break;
    case 2: text << "Left"; break;
    case 3: text << "Right"; break;
    case 4: text << "Tagger"; break;
    default: text << "N/A";
  }
  //auto handle3 = std::async(std::launch::async, &lasertag::dsp_write, this, m_t_coord[0] + 5, m_t_coord[1] + 5, text.str(), Terminal6x8, COLOR_WHITE);
  dsp_write(m_t_coord[0] + 5, m_t_coord[1] + 5, text.str());
}



/*
 * Display control functions
 * All functions must make sure only one draws to the display at a time.
 */
 
//________________________________________________________________________________
void lasertag::clear_hit() {
  if (!m_dsp_init)
    return;
  std::lock_guard<std::recursive_mutex> dsp_lock(m_mtx_dsp);
  m_dsp->fillRectangle(m_t_coord[0]+1, m_t_coord[1]+1, m_dsp->maxX()/2-1, m_t_coord[3]-1, COLOR_BLACK);
}

//________________________________________________________________________________
void lasertag::clear_tagged() {
  if (!m_dsp_init)
    return;
  std::lock_guard<std::recursive_mutex> dsp_lock(m_mtx_dsp);
  m_dsp->fillRectangle(m_dsp->maxX()/2+1, m_t_coord[1]+1, m_t_coord[2]-1, m_t_coord[3]-1, COLOR_BLACK);
}

//________________________________________________________________________________
void lasertag::dsp_write(int x, int y, std::string text, uint8_t* font, uint16_t color) {
  if (!m_dsp_init)
    return;
  std::lock_guard<std::recursive_mutex> dsp_lock(m_mtx_dsp);

  // reset font to the previous one after draw
  uint8_t tmp = m_dsp->setFont(font);
  m_dsp->drawText(x, y, text, color);
  m_dsp->setFont(&tmp);
}

//________________________________________________________________________________
void lasertag::draw_health(int old_h, int new_h) {
  if (!m_dsp_init)
    return;
  std::lock_guard<std::recursive_mutex> dsp_lock(m_mtx_dsp);

  // calculate the correct horizontal coordinates for the rectangle start and end point
  float perc_old = old_h * (1.0 / MAX_HEALTH);
  float perc_new = new_h * (1.0 / MAX_HEALTH);
  int h_old = ((m_h_coord[2] - m_h_coord[0]) * perc_old) + m_h_coord[0];
  int h_new = ((m_h_coord[2] - m_h_coord[0]) * perc_new) + m_h_coord[0];

  // black if decrement, color if increment
  if (h_old > h_new) {
    m_dsp->fillRectangle(h_new, m_h_coord[1], h_old, m_h_coord[3], COLOR_BLACK);
  } else if (h_old < h_new) {
    m_dsp->fillRectangle(h_old, m_h_coord[1], h_new, m_h_coord[3], COLOR_RED);
  }
}

//________________________________________________________________________________
void lasertag::draw_ammo(int old_a, int new_a) {
  if (!m_dsp_init)
    return;
  std::lock_guard<std::recursive_mutex> dsp_lock(m_mtx_dsp);

  // calculate the correct horizontal coordinates for the rectangle start and end point
  float perc_old = old_a * (1.0 / MAX_AMMO);
  float perc_new = new_a * (1.0 / MAX_AMMO);
  int a_old = ((m_a_coord[2] - m_a_coord[0]) * perc_old) + m_a_coord[0];
  int a_new = ((m_a_coord[2] - m_a_coord[0]) * perc_new) + m_a_coord[0];

  // black if decrement, color if increment
  if (a_old > a_new) {
    m_dsp->fillRectangle(a_new, m_a_coord[1], a_old, m_a_coord[3], COLOR_BLACK);
  } else if (a_old < a_new) {
    m_dsp->fillRectangle(a_old, m_a_coord[1], a_new, m_a_coord[3], COLOR_GREEN);
  }
}
