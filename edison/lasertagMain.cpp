// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>

#include <mraa.hpp>
#include <ssd1327.h>

#include <string>
#include <iostream>
#include <sstream>

#define I2C_BUS 6

int running = 1;
uint8_t ammo = 75;
bool active = false;

upm::SSD1327* lcd;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}

void lcd_write_int(int i, int x, int y) {
  lcd->setCursor(x, y);
  std::stringstream ss;
  ss << i;
  lcd->write(ss.str());
}

void write_ammo() {
  if (ammo < 100) {
    lcd_write_int(0, 1, 6);
    lcd_write_int(ammo, 1, 7);
  } else {
    lcd_write_int(ammo, 1, 6);
  }
}

void write_status() {
  lcd->setCursor(0, 2);
  if (active) {
    lcd->write(" active ");
  } else {
    lcd->write("inactive");
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  printf("init display... ");
  fflush(stdout);
  lcd = new upm::SSD1327(I2C_BUS);
  printf("Done.\n");
  fflush(stdout);
  lcd->setGrayLevel(255);

  write_status();
  lcd->setCursor(1, 0);
  lcd->write("ammo: ");
  write_ammo();
  active = true;
  write_status();

  mraa::I2c* i2c = new mraa::I2c(I2C_BUS);
  i2c->address(0x68);

  uint8_t i = 2;

  while (running == 1) {
    int rec = i2c->read();
    if (rec != 0) {
      printf("%d\n", rec);
      fflush(stdout);

      lcd_write_int(rec, i++, 0);
      if (i > 10)
        i = 2;
    }
    usleep(1000);
  }

  lcd->close();
  printf("\nDone.\n");
  return 0;
}
