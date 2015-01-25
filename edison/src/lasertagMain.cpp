// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <syslog.h>

#include <string>
#include <iostream>
#include <sstream>

#include "./lasertag.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}


int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  if (argc != 3) {
    printf("usage: lasertagMain [server host] [BT slave]\n");
    exit(1);
  }

  lasertag lt(6);
  lt.i2c()->address(0x68);

  lt.dsp_init();

  lt.tcp_init(argv[1]);
  lt.bt_init(argv[2]);
  lt.gpio_init(36);

  printf("\n");
  fflush(stdout);

  lt.spawn_threads();
  while (running == 1) {
  }
  lt.join_threads();

  return 0;
}


