// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include <mraa.hpp>

#include <string>

#include "./bt_serial.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  bt_serial bluetooth("/dev/ttyMFD1");

  bluetooth.bt_setup();

  while (running == 1) {
    if (bluetooth.bt_available(1)) {
      printf("%c", bluetooth.bt_read());
    }
  }

  return 0;
}
