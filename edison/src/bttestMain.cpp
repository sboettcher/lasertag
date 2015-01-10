// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include <mraa.hpp>

#include <string>

#include "./edison_serial.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  edison_serial bluetooth("/dev/ttyMFD1");

  if (argc == 1)
    bluetooth.bt_slave_init("EdisonBTSlave");
  else if (argc == 2)
    bluetooth.bt_master_init("EdisonBTMaster", argv[1]);

  while (running == 1 && bluetooth.bt_connected()) {
    if (bluetooth.available(1)) {
      printf("%c", bluetooth.serial_read());
      fflush(stdout);
    }
    //bluetooth.serial_write("test");
  }

  return 0;
}
