// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <string>

#include "./TFT_22_ILI9225.h"

#define TFT_LED_PIN 31
#define TFT_RST_PIN 32
#define TFT_RS_PIN 33

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nclosing nicely\n");
    running = 0;
  }
}

int main(int argc, char** argv) {
  //signal(SIGINT, sig_handler);

  TFT_22_ILI9225 tft(TFT_LED_PIN, TFT_RST_PIN, TFT_RS_PIN);

  return 0;
}
