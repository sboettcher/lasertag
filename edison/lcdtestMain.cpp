// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <mraa.hpp>
#include "/home/root/code/upm/src/lcd/ssd1327.h"

#define DEFAULT_IOPIN 8

static int iopin;
int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nclosing IO %d nicely\n", iopin);
    running = 0;
  }
}

int main(int argc, char** argv) {

  signal(SIGINT, sig_handler);

  upm::SSD1327 *lcd = new upm::SSD1327(BUS_NUMBER, 0x3C);

  return 0;
}
