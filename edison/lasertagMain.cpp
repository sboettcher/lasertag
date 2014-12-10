// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>

#include <mraa.hpp>

#include <string>
#include <iostream>


int running = 1;


void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}


int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  mraa::I2c* i2c = new mraa::I2c(6);
  i2c->address(0x68);

  while (running == 1) {
    int rec = i2c->read();
    if (rec != 0) {
      printf("%d\n", rec);
      fflush(stdout);
    }
    usleep(1000);
  }

  printf("\nDone.\n");
  return 0;
}
