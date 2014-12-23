// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <syslog.h>

#include <string>

#include "./lasertag.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}


int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  lasertag lt(6);
  lasertag dsp(1);

  lt.i2c()->address(0x68);

  dsp.init_groveOLED();
  dsp.init_ILI9225();

  uint8_t i = 2;

  while (running == 1) {
    int rec = lt.i2c()->read();
    if (rec != 0) {
      printf("%d\n", rec);
      fflush(stdout);
    }
    if (rec != 0 && rec != 255) {
      dsp.groveOLED_write(std::to_string(rec), i++, 0);
      dsp.ILI9225()->drawText(i+10, 10, std::to_string(rec));
      if (i > 10)
        i = 2;
    }
    usleep(1000);
  }

  return 0;
}
