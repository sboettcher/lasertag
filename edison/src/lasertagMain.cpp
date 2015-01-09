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

  dsp.init_ILI9225();
  dsp.ILI9225()->setOrientation(3);

  uint8_t i = 0;

  while (running == 1) {
    int rec = lt.i2c()->read();
    if (rec != 0) {
      printf("%d\n", rec);
      fflush(stdout);
    }
    if (rec != 0 && rec != 255) {
      dsp.ILI9225()->drawText(10, ((i++)*dsp.ILI9225()->fontY())+10, std::to_string(rec));
      if ((i*dsp.ILI9225()->fontY())+10 > dsp.ILI9225()->maxY()-2*dsp.ILI9225()->fontY())
        i = 0;
    }
    usleep(1000);
  }

  return 0;
}
