// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <mraa.hpp>
#include <ssd1327.h>

#define BUS_NUMBER 0

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nclosing nicely\n");
    running = 0;
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  upm::SSD1327* lcd = new upm::SSD1327(BUS_NUMBER);

  for (uint8_t i = 0; i < 12; ++i) {
    lcd->setCursor(i, 0);
    lcd->setGrayLevel(i);
    lcd->write("Hello World");
  }

  lcd->close();

  return 0;
}
