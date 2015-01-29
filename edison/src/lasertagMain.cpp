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

  lasertag lt;

  lt.dsp_init();
  lt.i2c_init(6, 0x68);
  lt.gpio_init(36);
  lt.tcp_init(argv[1]);
  lt.set_bt_slave(argv[2]);

  printf("\n");
  fflush(stdout);

  lt.spawn_threads();
  while (running == 1) {
  }
  lt.join_threads();

  return 0;
}


