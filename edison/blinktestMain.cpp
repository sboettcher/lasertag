// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <mraa.hpp>

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
  if (argc < 2) {
    printf("Using default pin %d. Provide int arg for different pin.\n", DEFAULT_IOPIN);
    iopin = DEFAULT_IOPIN;
  } else {
    iopin = strtol(argv[1], NULL, 10);
    printf("Using pin %d.\n", iopin);
  }

  signal(SIGINT, sig_handler);

  mraa::Gpio* gpio = new mraa::Gpio(iopin);
  if (gpio == NULL) {
    return MRAA_ERROR_UNSPECIFIED;
  }

  mraa_result_t response = gpio->dir(mraa::DIR_OUT);
  if (response != MRAA_SUCCESS) {
    mraa::printError(response);
    return 1;
  }

  while (running == 1) {
    response = gpio->write(1);
    usleep(100000);
    response = gpio->write(0);
    usleep(100000);
  }

  delete gpio;
  return response;
}
